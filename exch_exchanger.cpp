/**
 * exch_exchanger.cpp
 *
 * C++ versions 4.4.5
 *
 *      exchanger : https://github.com/Yujiro3/exchanger
 *      Copyright (c) 2011-2013 sheeps.me All Rights Reserved.
 *
 * @package         exchanger
 * @copyright       Copyright (c) 2011-2013 sheeps.me
 * @author          Yujiro Takahashi <yujiro3@gmail.com>
 * @filesource
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/util.h>

#include "log_logger.h"
#include "exch_server.h"
#include "exch_client.h"
#include "exch_exchanger.h"

namespace exch {
    /**
     * 読み込みコールバック
     *
     * @access public
     * @param bufferevent *bev
     * @param void *argv
     * @return void
     */
    void exchanger::readcb(struct bufferevent *bev, void *argv) {
        int current = (int) bufferevent_getfd(bev);
        exch::server *serv = (exch::server *)argv;
        std::stringstream numtostr;
        std::string content;

        char buf[4096];
        struct evbuffer *input = bufferevent_get_input(bev);
        while (evbuffer_remove(input, buf, sizeof(buf)) > 0) {
            content.append(buf);
            bzero(&buf, sizeof(buf));
        }

        /* FastCGIへ送信 */
        numtostr << CMD_SEND;
        serv->fcgicli->params["Exchanger-Command"] = numtostr.str();
        numtostr.str("");
        numtostr << current;
        serv->fcgicli->params["Exchanger-SID"] = numtostr.str();
        std::string response = serv->fcgicli->request(content);
        if (response.length() == 0) {
            log::debug("No response.");
            return ;
        }
        
        /* FastCGIからの内容をクライアントへ送信 */
        header_t header;
        content.erase();
        content = parse(&header, &response);
        if (header.cmd == CMD_TLS) {
            log::info("CMD_TLS");
            serv->clients[current]->sslFilter(serv->ssl_ctx);
        } else if (header.cmd == CMD_PUBLISH) {
            std::stringstream cmd;
            cmd << "SMEMBERS room:" << header.to;
            log::info(cmd.str());

            serv->rcli->query(cmd.str());

            mem::list_t::iterator cursor = serv->rcli->member.begin();
            while (cursor != serv->rcli->member.end()) {
                int pos = atoi(cursor->c_str());
                if (current != pos) {
                    serv->clients[pos]->send(content);
                }
                ++cursor;
            }
        } else {
            header.to = (header.to > 0) ? header.to : current;
            numtostr << "  -->  " << header.to;
            log::info(numtostr.str());

            serv->clients[header.to]->send(content);
        }
    }

    /**
     * 書き込みコールバック
     *
     * @access public
     * @param bufferevent *bev
     * @param void *argv
     * @return void
     */
    void exchanger::writecb(struct bufferevent *bev, void *argv) {
        //exch::server *serv = (exch::server *)argv;

        struct evbuffer *output = bufferevent_get_output(bev);
        if (evbuffer_get_length(output) == 0) {
            log::debug("sent to the client.");
            return ;
        }

        /* 送信漏れがあれば残り全て送信 */
        char buf[1024];
        int num;
        while ((num = evbuffer_remove(output, buf, sizeof(buf))) > 0) {
            bufferevent_write(bev, buf, num);
        }
    }
    
    /**
     * ステータス変化コールバック
     *
     * @access public
     * @param struct bufferevent *bev
     * @param short events
     * @param void *argv
     * @return void
     */
    void exchanger::eventcb(struct bufferevent *bev, short events, void *argv) {
        exch::server *serv = (exch::server *)argv;

        if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
            int current = (int) bufferevent_getfd(bev);
            std::stringstream numtostr;
            /* FastCGIへ送信 */
            numtostr << CMD_DISCONNECT;
            serv->fcgicli->params["Exchanger-Command"] = numtostr.str();
            numtostr.str("");
            numtostr << current;
            serv->fcgicli->params["Exchanger-SID"] = numtostr.str();

            serv->fcgicli->request(numtostr.str());
            serv->clients[current]->free();
            serv->clients.erase(current);
        }
    }

    /**
     * FCGIデータの解析
     *
     * @access public
     * @param header_t parsed
     * @param string response
     * @return string
     */
    std::string exchanger::parse(header_t *parsed, std::string *response) {
        size_t hpos = response->find(SEPARATOR);
        size_t pos, length, exend, conlen;
        std::string header = response->substr(0, hpos);

        pos = header.find("Exchanger-Command:");
        if (pos != std::string::npos) {
            pos += sizeof("Exchanger-Command:");
            exend = header.find(ENDLINE, pos);
            length = (exend != std::string::npos) ? (exend - pos) : std::string::npos;
            parsed->cmd = atoi(response->substr(pos, length).c_str());
        }

        pos = header.find("Exchanger-To:");
        if (pos != std::string::npos) {
            pos += sizeof("Exchanger-To:");
            exend = header.find(ENDLINE, pos);
            length = (exend != std::string::npos) ? (exend - pos) : std::string::npos;
            parsed->to = atoi(response->substr(pos, length).c_str());
        }

        pos = header.find("Exchanger-SID:");
        if (pos != std::string::npos) {
            pos += sizeof("Exchanger-SID:");
            exend = header.find(ENDLINE, pos);
            length = (exend != std::string::npos) ? (exend - pos) : std::string::npos;
            parsed->sid = atoi(response->substr(pos, length).c_str());
        }

        pos = header.find("Content-Length:");
        if (pos != std::string::npos) {
            pos += sizeof("Content-Length:");
            exend = header.find(ENDLINE, pos);
            length = (exend != std::string::npos) ? (exend - pos) : std::string::npos;
            conlen = atoi(response->substr(pos, length).c_str());
        } else {
             conlen = response->length() - hpos + 4;
        }

        return response->substr(hpos + 4, conlen);
    }
} // namespace exch 

