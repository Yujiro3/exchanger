/**
 * mem_redis.cpp
 *
 * C++ versions 4.4.5
 *
 *      mem_redis : https://github.com/Yujiro3/rediscli
 *      Copyright (c) 2011-2013 sheeps.me All Rights Reserved.
 *
 * @package         mem_redis
 * @copyright       Copyright (c) 2011-2013 sheeps.me
 * @author          Yujiro Takahashi <yujiro3@gmail.com>
 * @filesource
 */

#include <iostream>
#include <string>

#include <cstring>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/event.h>

#include "log_logger.h"
#include "mem_rediscli.h"

namespace mem {
    /**
     * コンストラクタ
     *
     * @access public
     * @param struct event_base *base
     * @param string _listen
     * @param int _port
     */
    rediscli::rediscli(struct event_base *base, std::string _listen, int _port) {
        event_assign(&timeout, base, -1, EV_PERSIST, timeoutcb, this);

        bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
        if (!bev) {
            log::error("Could not initialize bufferevent_socket_new!");
            return ;
        }

        bufferevent_setcb(bev, NULL, NULL, eventcb, this);
        bufferevent_disable(bev, EV_READ | EV_WRITE);

        connected = false;
        listen    = _listen;
        port      = _port;
    }

    /**
     * デストラクタ
     *
     * @access public
     */
    rediscli::~rediscli() {
        bufferevent_free(bev);
    }

    /**
     * Redisサーバへ接続
     *
     * @access public
     * @return bool
     */
    bool rediscli::connection() {
        struct sockaddr *address;
        int length;

        if (port >= 1 && port <= 65535) {
            struct sockaddr_in sin;
            bzero(&sin, sizeof(sin));
            sin.sin_family = AF_INET;
            sin.sin_addr.s_addr = inet_addr(listen.c_str());
            sin.sin_port = htons(port);
            
            address = (struct sockaddr *)&sin;
            length  = sizeof(sin);
        } else {
            struct sockaddr_un sun;
            bzero(&sun, sizeof(sun));
            sun.sun_family = AF_LOCAL;
            strcpy(sun.sun_path, listen.c_str());

            address = (struct sockaddr *)&sun;
            length  = sizeof(sun);
        }


        if (bufferevent_socket_connect(bev, address, length) < 0) {
            log::error("Could not connect a redis server!");
            return false;
        }

        return true;
    }

    /**
     * イベントコールバック
     *
     * @access public
     * @param bufferevent *bev
     * @param short events
     * @param void *argv
     * @return void
     */
    void rediscli::eventcb(bufferevent *bev, short events, void *argv) {
        rediscli *self = (rediscli *) argv;
    
        if (events & (BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)) {
            self->connected = false;
            struct timeval tv;
            evutil_timerclear(&tv);
            tv.tv_sec = 3;
            event_add(&self->timeout, &tv);
        } else if (events & BEV_EVENT_CONNECTED) {
            self->connected = true;
        }
    }
    
    /**
     * コマンド送信
     *
     * @access public
     * @param evutil_socket_t fd
     * @param short event
     * @param void *argv
     * @return void
     */
    void rediscli::timeoutcb(evutil_socket_t fd, short event, void *argv) {
        rediscli *self = (rediscli *) argv;
        
        if (self->connected) {
            self->connected = true;
            self->connection();
        }
    }
    
    /**
     * コマンド送信
     *
     * @access public
     * @param string cmd
     * @return void
     */
    void rediscli::query(std::string cmd) {
        if (cmd.find(BULK_SEPARATOR) == std::string::npos) {
            cmd += BULK_SEPARATOR;
        }
        evutil_socket_t fd = bufferevent_getfd(bev);
        write(fd, cmd.c_str(), cmd.length());
        _result();
    }

    /**
     * 結果の取得
     *
     * @access public
     * @return void
     */
    void rediscli::_result() {
        char buf[1024*16];
        int nread;

        evutil_socket_t fd = bufferevent_getfd(bev);
        nread = read(fd, buf, sizeof(buf));
        if (nread <= 0) {
            return ;;
        }
        _parse(buf);
    }
    
    /**
     * 結果の解析
     *
     * @access public
     * @param string response
     * @return void
     */
    void rediscli::_parse(char *buf) {
        std::string response(buf);
        size_t startline = 1;
        size_t endline   = response.find(BULK_SEPARATOR);
        std::string bulk = response.substr(startline, endline);
        result.erase();

        if (buf[0] == '+') {
            result = bulk;
        } else if (buf[0] == '-') {
            /* エラー */
            result = response.substr(startline, endline);
        } else if (buf[0] == ':') {
            result = atoi(bulk.c_str());
        } else if (buf[0] == '*') {
            int rows = atoi(bulk.c_str()) * 2;
            member.clear();

            for (int row=0; row<=rows; row++) {
                if (row%2 == 0 && row > 1) {
                    member.push_back(response.substr(startline, (endline - startline)));
                }
        
                startline = endline + 2;
                endline   = response.find(BULK_SEPARATOR, startline);            
            }
        } else if (buf[0] == '$') {
            startline = endline + 2;
            endline   = response.find(BULK_SEPARATOR, startline);            
            result    = response.substr(startline, endline);
        }
    }
} // namespace mem