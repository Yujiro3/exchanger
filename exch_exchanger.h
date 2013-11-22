/**
 * exch_exchanger.h
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

#ifndef __EXCH_EXCHANGER_H__
#define __EXCH_EXCHANGER_H__

#include <string>

namespace exch {
    /**
     * ヘッダー
     * @typedef struct header_t
     */
    typedef struct {
        int cmd;
        int to;
        int sid;
    } header_t;

    /**
     * コマンド：送信
     * @const integer
     */
    const int CMD_SEND = 1;

    /**
     * コマンド：配信
     * @const integer
     */
    const int CMD_PUBLISH = 2;

    /**
     * コマンド：TLS
     * @const integer
     */
    const int CMD_TLS = 4;

    /**
     * コマンド：切断
     * @const integer
     */
    const int CMD_DISCONNECT = 8;

    /**
     * セパレーター
     * @const char
     */
    const char SEPARATOR[] = "\r\n\r\n";

    /**
     * 改行
     * @const char
     */
    const char ENDLINE[] = "\r\n";

    /**
     * Exchanger交換器クラス
     *
     * @package     Exchanger
     * @author      Yujiro Takahashi <yujiro3@gmail.com>
     */
    class exchanger {
    public:

        /**
         * 読み込みコールバック
         *
         * @access public
         * @param bufferevent *bev
         * @param void *argv
         * @return void
         */
        static void readcb(struct bufferevent *bev, void *argv);
        
        /**
         * 書き込みコールバック
         *
         * @access public
         * @param bufferevent *bev
         * @param void *argv
         * @return void
         */
        static void writecb(struct bufferevent *bev, void *argv);
        
        /**
         * ステータス変化コールバック
         *
         * @access public
         * @param struct bufferevent *bev
         * @param short events
         * @param void *argv
         * @return void
         */
        static void eventcb(struct bufferevent *bev, short events, void *argv);

        /**
         * FCGIデータの解析
         *
         * @access public
         * @param header_t parsed
         * @param string response
         * @return string
         */
        static std::string parse(header_t *parsed, std::string *response);
    };
} // namespace exch

#endif // #ifndef __EXCH_EXCHANGER_H__
