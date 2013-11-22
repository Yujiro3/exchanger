/**
 * mem_redis.h
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

#ifndef __MEM_REDISCLI_H__
#define __MEM_REDISCLI_H__

#include <string>
#include <vector>

#include <event2/event-config.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

namespace mem {
    /**
     * メンバーリストの格納
     * @typedef std::vector<std::string> list_t;
     */
    typedef std::vector<std::string> list_t;

    /**
     * バルクセパレーター
     * @const char
     */
    const char BULK_SEPARATOR[] = "\r\n";

    /**
     * redisクライアントクラス
     *
     * @package     redis
     * @author      Yujiro Takahashi <yujiro3@gmail.com>
     */
    class rediscli {
    public:
        /**
         * イベントバッファー
         * @var struct
         */
        struct bufferevent *bev;

        /**
         * イベントタイムアウト
         * @var struct
         */
        struct event timeout;

        /**
         * 接続フラグ
         * @var boolean
         */
        bool connected;

        /**
         * 接続先
         * @var string
         */
        std::string listen;

        /**
         * ポート番号
         * @var int
         */
        int port;

        /**
         * ヘッダー文字列
         * @var struct
         */
        std::string result;

        /**
         * メンバーリスト
         * @var list_t
         */
        list_t member;
        
    public:
        /**
         * コンストラクタ
         *
         * @access public
         * @param struct event_base *base
         * @param string _listen
         * @param int _port
         */
        rediscli(struct event_base *base, std::string _listen, int _port);
    
        /**
         * デストラクタ
         *
         * @access public
         */
        ~rediscli();
    
        /**
         * Redisサーバへ接続
         *
         * @access public
         * @return bool
         */
        bool connection();
    
        /**
         * イベントコールバック
         *
         * @access public
         * @param bufferevent *bev
         * @param short events
         * @param void *argv
         * @return void
         */
        static void eventcb(bufferevent *bev, short events, void *argv);
        
        /**
         * コマンド送信
         *
         * @access public
         * @param evutil_socket_t fd
         * @param short event
         * @param void *argv
         * @return void
         */
        static void timeoutcb(evutil_socket_t fd, short event, void *argv);
        
        /**
         * コマンド送信
         *
         * @access public
         * @param string cmd
         * @return void
         */
        void query(std::string cmd);

    private:
        /**
         * 結果の取得
         *
         * @access public
         * @return void
         */
        void _result();
        
        /**
         * 結果の解析
         *
         * @access public
         * @param string response
         * @return void
         */
        void _parse(char *buf);
    };
} // namespace mem

#endif // #ifndef __MEM_REDISCLI_H__
