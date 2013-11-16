<?php
/**
 * Exchanger.php
 *
 * PHP versions 5.3.x
 *
 *      Exchanger : https://github.com/Yujiro3/exchanger
 *      Copyright (c) 2011-2013 sheeps.me All Rights Reserved.
 *
 * @package         Exchanger
 * @copyright       Copyright (c) 2011-2013 sheeps.me
 * @author          Yujiro Takahashi <yujiro3@gmail.com>
 * @filesource
 */

/**
 * Exchangerクラス
 *
 * @package         Exchanger
 * @author          Yujiro Takahashi <yujiro3@gamil.com>
 */
class Exchanger {
    /**
     * Redisセッションキー
     * @const string
     */
    const SID_KEY = 'session:%s';

    /**
     * Redisルームキー
     * @const string
     */
    const ROOM_KEY = 'room:%s';

    /**
     * コマンド：送信
     * @const integer
     */
    const CMD_SEND = 1;

    /**
     * コマンド：配信
     * @const integer
     */
    const CMD_PUBLISH = 2;

    /**
     * コマンド：TLS
     * @const integer
     */
    const CMD_TLS = 4;

    /**
     * コマンド：接続
     * @const integer
     */
    const CMD_CONNECT = 8;

    /**
     * コマンド：切断
     * @const integer
     */
    const CMD_DISCONNECT = 16;

    /**
     * Redisサーバ
     * @var object
     */
    public $redis;

    /**
     * Exchangerからのデータ
     * @var string
     */
    public $buff;

    /**
     * 送信相手
     * @var integer
     */
    public $to;

    /**
     * SID
     * @var integer
     */
    public $sid;

    /**
     * コマンド
     * @var integer
     */
    public $cmd;

    /**
     * コンストラクタ
     *
     * @access public
     * @param string  $host
     * @param integer $port
     * @return void
     */
    public function __construct($host='localhost', $port=6379) {
        /**
         * Exchangerからデータ取得(PUT Method)
         */
        $this->buff = '';
        $handle = fopen('php://input', 'r');
        while ($row = fread($handle, 4096)) {
            $this->buff .= $row;
        }
        fclose($handle);

        $this->cmd = empty($_SERVER['Exchanger-Command']) ? self::CMD_SEND:$_SERVER['Exchanger-Command'];
        $this->sid = empty($_SERVER['Exchanger-SID']) ? '0':$_SERVER['Exchanger-SID'];
        $this->to  = $this->sid;

        /**
         * RedisサーバにSession情報を保存
         */
        $this->redis = new Redis();    
        $this->redis->connect($host, $port);
        
        $key = sprintf(self::SID_KEY, $this->sid);
        $this->session = json_decode($this->redis->get($key), true);
        if (!is_array($this->session)) {
            $this->session = array(
                'to' => $this->to
            );
        }
    }

    /**
     * デストラクタ
     *
     * @access public
     * @return void
     */
    public function __destruct() {
        $key = sprintf(self::SID_KEY, $this->sid);
        if ($this->cmd == self::CMD_DISCONNECT) {
            $this->redis->del($key);
        } else {
            $this->redis->set($key, json_encode($this->session));
        }
    }

    /**
     * Exchangerへ送信
     *
     * @access public
     * @param string $buf
     * @return void
     */
    public function send($buf) {
        header("Exchanger-Command: {$this->cmd}");
        header("Exchanger-to: {$this->to}");
        header("Content-length: ".strlen($buf));
        echo $buf;
    }

    /**
     * コマンド設定
     *
     * @access public
     * @param string $cmd
     * @return void
     */
    public function setCommand($cmd) {
        if ($cmd = 'publish') {
            $this->cmd = self::CMD_PUBLISH;
        } else if ($cmd = 'tls') {
            $this->cmd = self::CMD_TLS;
        } else {
            $this->cmd = self::CMD_SEND;
        }
    }

    /**
     * データ取得
     *
     * @access public
     * @return string
     */
    public function getBuffer() {
        return $this->buff;
    }

    /**
     * 接続ユーザー一覧取得
     *
     * @access public
     * @return array
     */
    public function getUsers() {
        $key = sprintf(self::SID_KEY, "*");
        $keys = $redis->getKeys($key);
        $len  = strlen($key) - 1;
        foreach ($keys as $key) {
            $list[] = substr($key, $len);
        }
        return $list;
    }

    /**
     * ルーム一覧取得
     *
     * @access public
     * @return array
     */
    public function getRooms() {
        $key = sprintf(self::ROOM_KEY, "*");
        $keys = $redis->getKeys($key);
        $len  = strlen($key) - 1;
        foreach ($keys as $key) {
            $list[] = substr($key, $len);
        }
        return $list;
    }

    /**
     * セッションデータ保存
     *
     * @access public
     * @param string $key
     * @param mixed $value
     * @return void
     */
    public function setSession($key, $value) {
        $this->session[$key] = $value;
    }

    /**
     * セッションデータ取得
     *
     * @access public
     * @return array
     */
    public function getSession($key) {
        return empty($this->session[$key])? null : $this->session[$key];
    }

} // class Exchanger
