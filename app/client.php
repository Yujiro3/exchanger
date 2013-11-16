<?php
/**
 * client.php
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

// Connect to Server
$socket = stream_socket_client("tcp://0.0.0.0:8888", $errno, $errstr);
//$socket = stream_socket_client("tcp://127.0.0.1:5222", $errno, $errstr, 30);

if($socket) {
    stream_set_blocking ($socket, false);
    /* WebSocketリクエスト */
    fwrite(
        $socket, 
        "GET /chat HTTP/1.1\r\n".
        "Upgrade: websocket\r\n".
        "Connection: Upgrade\r\n".
        "Host: chat.sheeps.me:8888\r\n".
        "Origin: http://chat.sheeps.me\r\n".
        "Pragma: no-cache\r\n".
        "Cache-Control: no-cache\r\n".
        "Sec-WebSocket-Key: NE9XXGhAW10auxrB7iFM4g==\r\n".
        "Sec-WebSocket-Version: 13\r\n".
        "Sec-WebSocket-Extensions: permessage-deflate; client_max_window_bits, x-webkit-deflate-frame\r\n".
        "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/32.0.1700.14 Safari/537.36\r\n".
        "\r\n"
    );
    echo fread($socket, 8192);
/*
    // TLS開始通知
    fwrite(
        $socket, 
        '<starttls xmlns="urn:ietf:params:xml:ns:xmpp-tls"/>'."\n"
    );


    echo fread($socket, 8192)."\n";        // <proceed xmlns="urn:ietf:params:xml:ns:xmpp-tls"/>
    echo "\n";

    // Start SSL
    echo "TLS/SSL ハンドシェイク\n";
    stream_set_blocking ($socket, true);
    stream_socket_enable_crypto ($socket, true, STREAM_CRYPTO_METHOD_SSLv3_CLIENT);
    stream_set_blocking ($socket, false);

*/    
    // sleep(10);
    // close connection
    fclose($socket);
}
echo "\n";