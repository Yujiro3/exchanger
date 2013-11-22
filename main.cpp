/**
 * main.cpp
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
#include <fstream>
#include <string>

#include <errno.h>
#include <paths.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

#include "ini_parse.h"
#include "log_logger.h"
#include "exch_server.h"

using namespace std;

/**
 * ヘルプメッセージ
 *
 * @package     exchanger
 * @param  int  argc
 * @param  char **argv
 * @return int
 */
static void syntax(void) {
    cout << "Usage: exchanger [options]" << endl;
    cout << "    -c, --config PATH   Config file path" << endl;
    cout << "    -d, --daemon Daemonize the process" << endl;
    cout << "    -h, --help          This Help." << endl;
    exit(1);
}

/**
 * デーモンプロセス
 *
 * @access public
 */
int daemonize() {
    /* 実行プロセスの開放. */
    switch (fork()) {
    case -1:
        /* Error */
        return -1;
    case 0:
        /* Continue if child */
        break;
    default:
        /* Exit if parent */
        exit(0);
    }

    /* 新規Session生成 PIDの取得. */
    pid_t newgrp = setsid();

    ofstream ofs(ini::get("pid"), ios::out | ios::trunc);
    ofs << newgrp;
    ofs.close();

    /* Session生成チェック */
    if (-1 == newgrp) {
        return -1;
    }
    return 1;
}

/**
 * メイン関数
 *
 * @access public
 * @param int  argc
 * @param char **argv
 * @return int
 */
int main(int argc, char **argv) {
    /* 引数チェック */
    if (argc < 2) {
        syntax();
    }

    bool isdaemon = false;
    bool loaded = false;

    /* 設定ファイル名取得 */
    for (int pos=1; pos < argc; pos++) {
        if (!strcmp(argv[pos], "-c") || !strcmp(argv[pos], "--config")) {
            ini::load(argv[++pos]);
            loaded = true;
        } else if (!strcmp(argv[pos], "-d") || !strcmp(argv[pos], "--daemon")) {
            isdaemon = true;
        } else {
            syntax();
        }
    }

    /* 設定ファイルの読込チェック */
    if (!loaded) {
        syntax();
    }

    /* プロセスIDの保存 */
    if (isdaemon) {
        daemonize();
    }

    /* ログの設定 */
    log::init(ini::get("log"), ini::get("error.log"), ini::value("log.type"));

    try {
        /* サーバ */
        exch::server *eserver;
        eserver = new exch::server();
        eserver->roop();
        delete eserver;
    } catch (const char* errmsg) {
        log::error(errmsg);
    }
    
    unlink(ini::get("pid"));

    return 0;
}

