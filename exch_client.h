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

namespace exch {
    /**
     * FCGIプロトコル開始リクエスト本文クラス
     *
     * @package     FCGIClient
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
         * @param struct bufferevent *inpbev イベントバッファー
         * @param struct vutil_socket_t fd   ソケット
         * @param struct sockaddr *sa        ソケットアドレス
         * @return void
         */
        client(struct event_base *base, evutil_socket_t fd, struct sockaddr *sa);
    
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
         * SSL通信設定
         *
         * @access public
         * @return void
         */
        void sslFilter(SSL_CTX *ssl_ctx);

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
