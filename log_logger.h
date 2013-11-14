/**
 * log_logger.h
 *
 * C++ versions 4.4.5
 *
 *      log_logger : https://github.com/Yujiro3/logger
 *      Copyright (c) 2011-2013 sheeps.me All Rights Reserved.
 *
 * @package         log_logger
 * @copyright       Copyright (c) 2011-2013 sheeps.me
 * @author          Yujiro Takahashi <yujiro3@gmail.com>
 * @filesource
 */

#ifndef __LOG_LOGGER_H__
#define __LOG_LOGGER_H__

#include <string>

namespace log {
    /**
     * 何もしない
     * @const integer
     */
    const int SILENT = 0;

    /**
     * ログの保存
     * @const integer
     */
    const int LOGGING = 1;

    /**
     * ログの出力
     * @const integer
     */
    const int DISPLAY = 2;

    /**
     * ログの出力と保存
     * @const integer
     */
    const int LOGDISP = 3;
    
    /**
     * 初期設定
     *
     * @access public
     * @param std::string logfile 
     * @param std::string errfile 
     * @param int flag
     * @return void
     */
    void init(std::string _logfile, std::string _errfile, int _flag);

    /**
     * メッセージ
     *
     * @access public
     * @return void
     */
    void msg(std::string msg);

    /**
     * インフォメーション
     *
     * @access public
     * @return void
     */
    void info(std::string msg);

    /**
     * デバッグメッセージ
     *
     * @access public
     * @return void
     */
    void debug(std::string msg);

    /**
     * エラー出力
     *
     * @access public
     * @return void
     */
    void error(std::string msg);

    /**
     * ログの保存
     *
     * @access public
     * @return void
     */
    void log(std::string msg);
};

#endif // #ifndef __LOG_LOGGER_H__
