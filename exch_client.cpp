/**
 * exch_client.cpp
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
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <csignal>

#include <event2/bufferevent_ssl.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/event.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include "ini_parse.h"
#include "log_logger.h"
#include "exch_client.h"

namespace exch {
    /**
     * コンストラクタ
     *
     * @access public
     * @param struct event_base *base    イベントベース
     * @param struct vutil_socket_t fd   ソケット
     * @param SSL_CTX *ssl_ctx           SSLコンテキスト
     * @param int socktype               SSLコンテキスト
     * @param struct sockaddr *sa        ソケットアドレス
     * @return void
     */
    client::client(struct event_base *base, evutil_socket_t fd, SSL_CTX *ssl_ctx, int socktype, struct sockaddr *sa) {
        struct bufferevent *(*socket_new[])(struct event_base *base, evutil_socket_t fd, SSL_CTX *ssl_ctx, int options) = {
            exch_socket_new,
            exch_openssl_socket_new
        };

        sid = (int)fd;
        // bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
        bev = (*socket_new[socktype])(base, fd, ssl_ctx, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
        if (!bev) {
            log::error("Error constructing bufferevent!");
            event_base_loopbreak(base);
            return ;
        }

        address = parseAddr(sa);
        if (address.length() > 0) {
            std::stringstream access;
            access << "connected\t" << address;
            log::log(access.str());
            log::debug(access.str());
        }
    }

    /**
     * デストラクタ
     *
     * @access public
     */
    client::~client() {
        bufferevent_free(bev);
    }

    /**
     * 送信
     *
     * @access public
     * @param string buf
     * @return void
     */
    void client::send(std::string buf) {
        bufferevent_write(bev, buf.c_str(), buf.length());
    }

    /**
     * 開放
     *
     * @access public
     * @return void
     */
    void client::free() {
        if (address.length() > 0) {
            std::stringstream access;
            access << "disconnect\t" << address;
            log::log(access.str());
            log::debug(access.str());
        }

        bufferevent_free(bev);
    }

    /**
     * プレーンソケットの作成
     *
     * @access public
     * @param struct event_base *base    イベントベース
     * @param struct evutil_socket_t fd  ソケット
     * @param struct SSL_CTX *ssl_ctx    SSLコンテキスト
     * @param struct int options         オプション
     * @return void
     */
    struct bufferevent *client::exch_socket_new(struct event_base *base, evutil_socket_t fd, SSL_CTX *ssl_ctx, int options) {
        return bufferevent_socket_new(base, fd, options);
    }

    /**
     * SSLソケットの作成
     *
     * @access public
     * @param struct event_base *base    イベントベース
     * @param struct evutil_socket_t fd  ソケット
     * @param struct SSL_CTX *ssl_ctx    SSLコンテキスト
     * @param struct int options         オプション
     * @return void
     */
    struct bufferevent *client::exch_openssl_socket_new(struct event_base *base, evutil_socket_t fd, SSL_CTX *ssl_ctx, int options) {
        SSL *ssl = SSL_new(ssl_ctx);
        return bufferevent_openssl_socket_new(base, fd, ssl, BUFFEREVENT_SSL_ACCEPTING, options);
    }

    /**
     * SSL通信設定
     *
     * @access public
     * @return bool
     */
    bool client::sslFilter(SSL_CTX *ssl_ctx) {
        if (!ssl_ctx) {
            return false;
        }
        SSL *ssl = SSL_new(ssl_ctx);
        struct event_base *base = bufferevent_get_base(bev);

        struct bufferevent *b_ssl = bufferevent_openssl_filter_new(
            base,
            bev, 
            ssl, 
            BUFFEREVENT_SSL_ACCEPTING,
            BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS
        );
        if (!b_ssl) {
            log::error("Error Bufferevent_openssl_new!");
            return false;
        }
        bev = b_ssl;

        return true;
    }

    /**
     * アクセスユーザーログ保存
     *
     * @access public
     * @param sockaddr *in_addr
     * @return string
     */
    std::string client::parseAddr(const struct sockaddr *in_addr) {
        char buf[256];
        std::stringstream result;
    
        switch (in_addr->sa_family) {
        case AF_INET:
            if (evutil_inet_ntop(
                in_addr->sa_family, 
                &((struct sockaddr_in *) in_addr)->sin_addr,
                buf, 
                sizeof(buf)
            )) {
                result << buf << ":" << ntohs(((struct sockaddr_in *) in_addr)->sin_port);
            }
            break;
#if HAVE_IPV6
        case AF_INET6:
            if (evutil_inet_ntop(
                in_addr->sa_family, 
                &((struct sockaddr_in *) in_addr)->sin6_addr,
                buf, 
                sizeof(buf)
            )) {
                result << buf << ":" << ntohs(((struct sockaddr_in6 *) in_addr)->sin6_port);
            }
            break;
#endif
        }
        return result.str();
    }
} // namespace eve 

