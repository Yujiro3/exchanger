/**
 * exch_server.h
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


#ifndef __EXCH_SERVER_H__
#define __EXCH_SERVER_H__

#include <cerrno>
#include <csignal>

#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/socket.h>

#include <event2/bufferevent_ssl.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include "mem_rediscli.h"
#include "fcgi_client.h"
#include "exch_client.h"

namespace exch {
    /**
     * 2次元配列コンテナの型定義
     * @typedef typedef std::map<unsigned long int, exch::client>
     */
    typedef std::map<unsigned long int, exch::client*> clients_t;

    /**
     * FCGIプロトコル開始リクエスト本文クラス
     *
     * @package     FCGIClient
     * @author      Yujiro Takahashi <yujiro3@gmail.com>
     */
    class server {
    public:
        /**
         * イベントベース
         * @var struct
         */
        struct event_base *base;

        /**
         * クライアントリスト
         * @var struct
         */
        struct bufferevent *bev;

        /**
         * SSLコンテキスト
         * @var struct
         */
        SSL_CTX *ssl_ctx;

        /**
         * リスナー
         * @var struct
         */
        struct evconnlistener *listener;

        /**
         * シグナルベース
         * @var struct
         */
        struct event *signal_event;

        /**
         * FCGIクライアント
         * @var fcgi::client
         */
        fcgi::client *fcgicli;

        /**
         * Redisクライアント
         * @var mem::rediscli
         */
        mem::rediscli *rcli;

        /**
         * クライアントリスト
         * @var clients_t
         */
        clients_t clients;

    public:
        /**
         * コンストラクタ
         *
         * @access public
         */
        server();
    
        /**
         * コンストラクタ
         *
         * @access public
         */
        ~server();
    
        /**
         * ループ処理
         *
         * @access public
         */
        void roop();

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
        static void listenercb(
            struct evconnlistener *listener, 
            evutil_socket_t fd,
            struct sockaddr *sa, 
            int socklen, 
            void *argv
        );
        
        /**
         * シグナルコールバック
         *
         * @access public
         * @param evutil_socket_t sig
         * @param short events
         * @param void *argv
         * @return void
         */
        static void signalcb(evutil_socket_t sig, short events, void *argv);
    
    private:
        /**
         * 待ち受け設定
         *
         * @access private
         * @return bool
         */
        bool _setListener();
    
        /**
         * FCGIクライアント設定
         *
         * @access private
         * @return bool
         */
        bool _setFCGIcli();
    
        /**
         * CTRL + C割り込み受付設定
         *
         * @access private
         * @return bool
         */
        bool _setSignal();
        
        /**
         * Redisサーバと接続
         *
         * @access private
         * @return bool
         */
        bool _setRedis();

        /**
         * SSL設定
         *
         * @access private
         * @return bool
         */
        bool _setSSL();
    };
}

#endif // #ifndef __EXCH_SERVER_H__
