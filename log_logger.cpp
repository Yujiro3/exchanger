/**
 * log_logger.cpp
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

#include <iostream>
#include <fstream>
#include <string>

#include <cstdio>
#include <ctime>

namespace log {    
    /**
     * ログファイル名
     * @var string
     */
    std::string logfile;

    /**
     * エラーログファイル名
     * @var string
     */
    std::string errfile;

    /**
     * タイプ
     * @var short
     */
    int flag;

    /**
     * 初期設定
     *
     * @access public
     * @param std::string logfile 
     * @param std::string errfile 
     * @param int flag
     * @return void
     */
    void init(std::string _logfile, std::string _errfile, int _flag) {
        logfile = _logfile;
        errfile = _errfile;
        flag    = _flag;
    }

    /**
     * 日付の取得
     *
     * @access public
     * @return void
     */
    std::string getDate() {
        time_t timestamp = time(NULL);   // get time now
        struct tm *now = localtime(&timestamp);

        char sdate[20];
        strftime(sdate, 20, "%Y-%m-%d %H:%M:%S", now);
        
        return std::string(sdate);
    }

    /**
     * 何もしない
     *
     * @access public
     * @param string msg
     * @param int color
     * @return void
     */
    void silent(std::string msg, short color) { }

    /**
     * ログの保存
     *
     * @access public
     * @param string msg
     * @param int color
     * @return void
     */
    void logging(std::string msg, short color) {
        std::ofstream ofs(logfile.c_str(), std::ofstream::out | std::ofstream::app);
        ofs << getDate() << "\t" << msg << "\n";
        ofs.close();
    }

    /**
     * ログの出力
     *
     * @access public
     * @param string msg
     * @param int color
     * @return void
     */
    void echo(std::string msg, short color) {
        // Error   31 : 赤
        // Info    32 : 緑
        // Notice  33 : 黄
        // Warning 34 : 青
        // Debug   37 : 白
        std::cout << "\033[" << color << "m" << msg << "\033[0m" << std::endl;
    }

    /**
     * ログの出力と保存
     *
     * @access public
     * @param string msg
     * @param int color
     * @return void
     */
    void logecho(std::string msg, short color) {
        echo(msg, color);
        logging(msg, 0);
    }

    /**
     * ログ関数ポインタ
     * @var void
     */
    void (*logger[])(std::string msg, short color) = {
        silent,
        logging,
        echo,
        logecho
    };

    /**
     * メッセージ
     *
     * @access public
     * @return void
     */
    void msg(std::string msg) {
        (*logger[flag])(msg, 37);
    }

    /**
     * インフォメーション
     *
     * @access public
     * @return void
     */
    void info(std::string msg) {
        (*logger[flag])(msg, 32);
    }

    /**
     * デバッグメッセージ
     *
     * @access public
     * @return void
     */
    void debug(std::string msg) {
        (*logger[flag])(msg, 33);
    }

    /**
     * エラー出力
     *
     * @access public
     * @return void
     */
    void error(std::string msg) {
        echo(msg, 31);
        
        std::ofstream ofs(errfile.c_str(), std::ofstream::out | std::ofstream::app);
        ofs << getDate() << "\t" << msg << std::endl;
        ofs.close();
    }

    /**
     * ログの保存
     *
     * @access public
     * @return void
     */
    void log(std::string msg) {
        std::ofstream ofs(logfile.c_str(), std::ofstream::out | std::ofstream::app);
        ofs << getDate() << "\t" << msg << "\n";
        ofs.close();
    }
};
