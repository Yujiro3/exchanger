/**
 * exch_client.h
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

#ifndef __EXCH_CLIENT_H__
#define __EXCH_CLIENT_H__

#include <string>

#include <event2/bufferevent_ssl.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#ifdef  __cplusplus
extern "C" {
#endif

    struct bufferevent *bufferevent_openssl_filter_new(
        struct event_base *base,
        struct bufferevent *underlying,
        struct ssl_st *ssl,
        enum bufferevent_ssl_state state,
        int options
    );

    struct bufferevent *bufferevent_openssl_socket_new(
        struct event_base *base,
        evutil_socket_t fd,
        struct ssl_st *ssl,
        enum bufferevent_ssl_state state,
        int options
    );

#ifdef  __cplusplus
}
#endif

namespace exch {
    /**
     * Exchangerクライアントクラス
     *
     * @package     Exchanger
     * @author      Yujiro Takahashi <yujiro3@gmail.com>
     */
    class client {
    public:
        /**
         * イベントバッファー
         * @var struct
         */
        struct bufferevent *bev;

        /**
         * クライアントアドレス
         * @var string
         */
        std::string address;

        /**
         * SID
         * @var int
         */
        int sid;

    public:
        /**
         * コンストラクタ
         *
         * @access public
         * @param struct event_base *base    イベントベース
         * @param struct vutil_socket_t fd   ソケット
         * @param SSL_CTX *ssl_ctx           SSLコンテキスト
         * @param struct sockaddr *sa        ソケットアドレス
         * @return void
         */
        client(struct event_base *base, evutil_socket_t fd, SSL_CTX *ssl_ctx, int socktype, struct sockaddr *sa);

        /**
         * デストラクタ
         *
         * @access public
         */
        ~client();
    
        /**
         * 送信
         *
         * @access public
         * @param string buf
         * @return void
         */
        void send(std::string buf);

        /**
         * 開放
         *
         * @access public
         * @return void
         */
        void free();

        /**
         * プレーンソケットの作成
         *
         * @access public
         * @param struct event_base *base    イベントベース
         * @param struct evutil_socket_t fd  ソケット
         * @param struct struct ssl_st *ssl  ssl
         * @param struct int options         オプション
         * @return void
         */
        static struct bufferevent *exch_socket_new(struct event_base *base, evutil_socket_t fd, SSL_CTX *ssl_ctx, int options);

        /**
         * SSLソケットの作成
         *
         * @access public
         * @param struct event_base *base    イベントベース
         * @param struct evutil_socket_t fd  ソケット
         * @param struct struct ssl_st *ssl  ssl
         * @param struct int options         オプション
         * @return void
         */
        static struct bufferevent *exch_openssl_socket_new(struct event_base *base, evutil_socket_t fd, SSL_CTX *ssl_ctx, int options);

        /**
         * SSL通信設定
         *
         * @access public
         * @return boolean
         */
        bool sslFilter(SSL_CTX *ssl_ctx);

        /**
         * アクセスユーザーログ保存
         *
         * @access public
         * @param sockaddr *in_addr
         * @return string
         */
        std::string parseAddr(const struct sockaddr *in_addr);
    };
} // namespace exch

#endif // #ifndef __EXCH_CLIENT_H__
