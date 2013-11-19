Persistent Connections Server.
======================
libevent2を利用した持続接続サーバー。

![alt exchanger](http://blog.sheeps.me/wp-content/uploads/2013/11/exchanger.png)

利用方法
------

### libevent2モジュールのインストール ###
    
    $ wget http://jaist.dl.sourceforge.net/project/levent/libevent/libevent-2.0/libevent-2.0.21-stable.tar.gz
    $ tar xzvf libevent-2.0.21-stable.tar.gz
    $ cd libevent-2.0.21-stable
    $ ./configure
    $ make
    $ sudo make install

### コンパイル ###
    
    $ g++ -L/usr/local/lib -I/usr/local/include -lcrypto -lssl -levent -levent_openssl -o exchanger ./*.cpp
    
### インストール ###
    
    $ git clone https://github.com/Yujiro3/exchanger.git
    $ cd exchanger
    $ make
    $ sudo make install
    $
    $ exchanger -c /etc/exchanger/exch.conf
    
### フロー ###

![alt exchanger](http://blog.sheeps.me/wp-content/uploads/2013/11/001.png)


### WebSocketフロー ###

![alt exchanger](http://blog.sheeps.me/wp-content/uploads/2013/11/002.png)


### チャットアプリ ###

[![MONEY](http://chat.sheeps.me/)](http://blog.sheeps.me/wp-content/uploads/2013/08/chat001.png)



### exchangerヘッダーコマンド ###

```
■ ヘッダー：コマンド

  Exchanger-Command: <Command>

    tls       暗号化
    publish   複数送信
    send      単体送信


■ ヘッダー：リストキー

  Exchanger-to: <Number>

    Number      送信先番号
    Number      送信先ルーム番号


■ ヘッダー：セッションID

  Exchanger-SID: <SessionID>

    sid       123
```

### FastCGIアプリ側（php） ###

```php
<?php
/* 自分自身に返信 */
$to = $_SERVER['Exchanger-SID'];

header("Exchanger-Command: send");
header("Exchanger-to: {$to}");
header("Content-length: ".strlen($buffer));

echo $buffer;
```



ライセンス
----------
Copyright &copy; 2013 Yujiro Takahashi  
Licensed under the [MIT License][MIT].  
Distributed under the [MIT License][MIT].  

[MIT]: http://www.opensource.org/licenses/mit-license.php
