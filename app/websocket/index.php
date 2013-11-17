<?php
/**
 * index.php
 *
 * PHP versions 5.4.x
 *
 *      exchanger : https://github.com/Yujiro3/exchanger
 *      Copyright (c) 2011-2013 sheeps.me All Rights Reserved.
 *
 * @package         exchanger
 * @copyright       Copyright (c) 2011-2013 sheeps.me
 * @author          Yujiro Takahashi <yujiro3@gmail.com>
 * @filesource
 */

include dirname(__FILE__).'/../Exchanger.php';

$exch = new Exchanger('localhost', 6379);

$exch->redis->set('debug', $exch->cmd);
if ($exch->cmd == Exchanger::CMD_CONNECT) {
    /**
     * 接続処理
     */
    exit();
} else if ($exch->cmd == Exchanger::CMD_DISCONNECT) {
    /**
     * 切断処理
     */
    exit();
}

$handshake = $exch->getSession('handshake');
if (empty($handshake)) {
    /**
     * ハンドシェイクの確立
     */
    if (preg_match('/Sec-WebSocket-Key: ([^\s]+)\r\n/', $exch->buff, $matches)) {
        $key = $matches[1];
    } else {
        $key = '';
    }

    /* 認証キー生成 */
    $accept = $key.'258EAFA5-E914-47DA-95CA-C5AB0DC85B11';
    $accept = base64_encode(sha1($accept, true));

    $header = "HTTP/1.1 101 Switching Protocols\r\n".
              "Upgrade: WebSocket\r\n".
              "Connection: Upgrade\r\n".
              "Sec-WebSocket-Accept: {$accept}\r\n".
              "\r\n";

    $exch->send($header);
    $exch->setSession('handshake', true);
} else {
    $msg  = json_decode(decode($exch->buff), true);
    $user = $msg['user'];

    $output = encode(json_encode($msg));
    $exch->send($output);
}


/**
 * メッセージのデコード
 *
 * @link http://d.hatena.ne.jp/uuwan/20130121/p1
 * @param string $frame
 * @return string
 */
function decode($frame) {
    /* データ本体のサイズ分類を取得 */
    $length = ord($frame[1]) & 127;

    /* データ本体とマスクの取得 */        
    if ($length == 126) {               // medium message
        $masks   = substr($frame, 4, 4);
        $payload = substr($frame, 8);
    } elseif ($length == 127) {         // large message
        $masks   = substr($frame, 10, 4);
        $payload = substr($frame, 14);
    } else {                            // small message
        $masks   = substr($frame, 2, 4);
        $payload = substr($frame, 6);
    }
    
    /* マスクの解除 */
    $length = strlen($payload);
    $string   = '';
    for ($pos=0; $pos<$length; $pos++) {
        $string .= $payload[$pos] ^ $masks[$pos % 4];
    }
    
    return $string;
}

/**
 * メッセージのエンコード
 *
 * @link http://d.hatena.ne.jp/uuwan/20130201/p1
 * @param string $string
 * @return string
 */
function encode($message) {
    $ftoo    = 0x81; // 1000 0001 先頭8bit
    $mask    = 0x00; // *000 0000 マスクフラグ
    $length  = strlen($message);
    $payload = '';

    if ($length < 126) {
        $mplen = $mask | $length;
    } elseif ($length <= 65536) {
        $mplen = $mask | 126;

        $octet = $length;
        while ($octet) {
            $payload = chr(0xFF & $octet) . $payload;
            $octet   = $octet >> 8;
        }

        while (strlen($payload) < 2) {
            $payload = chr(0) . $payload;
        }
    } else {
        $mplen = $mask | 127;

        $octet = $length;
        while ($octet) {
            $payload = chr(0xFF & $octet) . $payload;
            $octet   = $octet >> 8;
        }

        while (strlen($payload) < 8) {
            $payload = chr(0) . $payload;
        }
    }

    return chr($ftoo) . chr($mplen) . $payload . $message;
}

