/**
 * exch_server.cpp
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
#include <sstream>

#include <cstring>
#include <cstdio>
#include <unistd.h>

#include "ini_parse.h"
#include "log_logger.h"
#include "exch_server.h"
#include "exch_exchanger.h"

namespace exch {
    /**
     * コンストラクタ
     *
     * @access public
     */
    server::server() {
        base = event_base_new();
        if (!base) {
            throw "Could not initialize libevent!";
        }

        _setListener();
        _setFCGIcli();
        _setSignal();
        _setRedis();
        _setSSL();
    }

    /**
     * コンストラクタ
     *
     * @access public
     */
    server::~server() {
        std::stringstream numtostr;

        clients_t::iterator row = clients.begin();
        while (row != clients.end()) {
            /* FastCGIへ送信 */
            numtostr.str("");
            numtostr << exch::CMD_DISCONNECT;
            fcgicli->params["Exchanger-Command"] = numtostr.str();
            numtostr.str("");
            numtostr << ((*row).second)->sid;
            fcgicli->params["Exchanger-SID"] = numtostr.str();

            fcgicli->request(numtostr.str());
            ((*row).second)->free();
            ++row;
        }

        delete rcli;
        delete fcgicli;

        /* 各種オブジェクトの開放 */
        evconnlistener_free(listener);
        event_free(signal_event);
        event_base_free(base);
    }

    /**
     * ループ処理
     *
     * @access public
     */
    void server::roop() {
        /* イベント駆動ループ処理 */
        event_base_dispatch(base);
    }

    /**
     * 待ち受けコールバック
     *
     * @access public
     * @param struct evconnlistener *listener, 
     * @param evutil_socket_t fd,
     * @param struct sockaddr *sa, 
     * @param int socklen, 
     * @param void *argv
     * @return void
     */
    void server::listenercb(
        struct evconnlistener *listener, 
        evutil_socket_t fd,
        struct sockaddr *sa, 
        int socklen, 
        void *argv
    ) {
        server *self = (server *)argv;

        self->clients[fd] = new client(self->base, fd, sa);

        bufferevent_setcb(
            self->clients[fd]->bev, 
            exchanger::readcb, 
            NULL, 
            exchanger::eventcb, 
            argv
        );
        bufferevent_enable(self->clients[fd]->bev, EV_READ | EV_WRITE);
    }
    
    /**
     * シグナルコールバック
     *
     * @access public
     * @param evutil_socket_t sig
     * @param short events
     * @param void *argv
     * @return void
     */
    void server::signalcb(evutil_socket_t sig, short events, void *argv) {
        server *self = (server *)argv;
        struct timeval delay = { 2, 0 };

        log::log("Caught an interrupt signal; stop the server.");
        unlink(ini::get("pid"));
    
        event_base_loopexit(self->base, &delay);
    }

    /**
     * 待ち受け設定
     *
     * @access private
     * @return bool
     */
    bool server::_setListener() {
        struct sockaddr_in sin;
        bzero(&sin, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(ini::value("port"));

        listener = evconnlistener_new_bind(
            base, 
            listenercb, 
            this,
            LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC,
            -1,
            (struct sockaddr*)&sin,
            sizeof(sin)
        );

        if (!listener) {
            throw "Could not create a listener!";
        } else {
            log::debug("Created listener.");
        }
        return true;
    }

    /**
     * FCGIクライアント設定
     *
     * @access private
     * @return bool
     */
    bool server::_setFCGIcli() {
        int port = ini::value("fcgi.port");
        if (port >= 1 && port <= 65535) {
            fcgicli = new fcgi::client(ini::get("fcgi.addr"), port);
        } else {
            fcgicli = new fcgi::client(ini::get("fcgi.addr"));
        }

        char dir[255];
        getcwd(dir, sizeof(dir));

        fcgicli->params["GATEWAY_INTERFACE"] = "FastCGI/1.0";
        fcgicli->params["REQUEST_METHOD"]    = "PUT";
        fcgicli->params["SCRIPT_FILENAME"]   = ini::get("fcgi.app");
std::cout << ini::get("fcgi.app") << std::endl;
        fcgicli->params["SERVER_SOFTWARE"]   = "cpp/fcgi_client";
        fcgicli->params["SERVER_PROTOCOL"]   = "HTTP/1.1";
        fcgicli->params["CONTENT_TYPE"]      = "application/x-www-form-urlencoded";

        return true;
    }

    /**
     * CTRL + C割り込み受付設定
     *
     * @access private
     * @return bool
     */
    bool server::_setSignal() {
        signal_event = evsignal_new(base, SIGINT, signalcb, this);
        if (!signal_event || event_add(signal_event, NULL) < 0) {
            throw "Could not create/add a signal event!";
        }
        return true;
    }

    /**
     * Redisサーバと接続
     *
     * @access private
     * @return bool
     */
    bool server::_setRedis() {
        rcli = new mem::rediscli(base, ini::get("redis.addr"), ini::value("redis.port"));

        if (!rcli->connection()) {
            throw "Could not connect a redis server!";
        } else {
            log::debug("Connected to redis server.");
        }
        return true;
    }

    /**
     * SSL設定
     *
     * @access private
     * @return bool
     */
    bool server::_setSSL() {
        if (!ini::value("ssl.support")) {
            return false;
        }
        SSL_library_init();
        ERR_load_crypto_strings();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();

        int random = RAND_poll();
        if (random == 0) {
            throw "RAND_poll() failed.\n";
        }
        ssl_ctx = SSL_CTX_new(SSLv3_server_method());
        SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, NULL);
        if (!SSL_CTX_use_certificate_chain_file(ssl_ctx, ini::get("ssl.local_cert"))) {
            throw "Can’t read certificate file";
        }

        if (!SSL_CTX_use_PrivateKey_file(ssl_ctx, ini::get("ssl.local_pk"), SSL_FILETYPE_PEM)) {
            throw "Can’t read key file";
        }
        
        SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, NULL);

        return true;
    }
} // namespace exch
