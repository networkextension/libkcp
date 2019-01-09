package main

import (
	"crypto/sha1"
	"golang.org/x/crypto/pbkdf2"
	"fmt"
	//"github.com/xtaci/kcp-go"

	"time"
	kcp "github.com/xtaci/kcp-go"
	//"github.com/xtaci/smux"

	 "strings"
	 // "encoding/base64"
	 "encoding/hex"
)

const port = ":9999"
var (
	// VERSION is injected by buildflags
	VERSION = "SELFBUILD"
	// SALT is use for pbkdf2 key expansion
	SALT = "kcp-go"
)
func addBase64Padding(value string) string {
	m := len(value) % 4
	if m != 0 {
		value += strings.Repeat("=", 4-m)
	}
	return value
}
func removeBase64Padding(value string) string {
	return strings.Replace(value, "=", "", -1)
}
func ListenTest() (*kcp.Listener, error) {
	var block kcp.BlockCrypt
	var blockTest kcp.BlockCrypt
	pass := pbkdf2.Key([]byte("it's a secrect"), []byte(SALT), 4096, 32, sha1.New)
	//fmt.Println("key",pass)
	//block, _ = kcp.NewAESBlockCrypt([]byte("1234567890123456789012345678901234567890"))
  block, _ = kcp.NewAESBlockCrypt(pass[:16])
	blockTest, _ = kcp.NewAESBlockCrypt([]byte("0123456789ABCDEF0123456789ABCDEF")[:24])
	//block, _ = kcp.NewNoneBlockCrypt(pass)
	//fmt.Println(hex.Dump(pass))
	//fmt.Println("key 2222 :",hex.EncodeToString([]byte("0123456789ABCDEF0123456789ABCDEF"[:16])))
	//
	// fmt.Println("eciphertext ",hex.EncodeToString(eciphertext))
	// finalMsg := base64.StdEncoding.EncodeToString(eciphertext)
	// fmt.Println("result ",finalMsg)
	// fmt.Println("org ",hex.EncodeToString([]byte("123456789012345678901234567890123456789")))
  encryptBytes(blockTest)
	return kcp.ListenWithOptions(port, block, 2, 2)
}
func encryptBytes(block kcp.BlockCrypt){

	for i:=1; i < 40; i++ {
		eciphertext := make([]byte, i)
		block.Encrypt(eciphertext, []byte("1234567890123456789012345678901234567890")[:i])
		fmt.Println("eciphertext ",hex.EncodeToString(eciphertext))

		pleciphertext := make([]byte, i)
		block.Decrypt(pleciphertext,eciphertext)
		fmt.Println("eciphertext ",hex.EncodeToString(pleciphertext))
	}

}
func server() {
	l, err := ListenTest()
	if err != nil {
		panic(err)
	}
	l.SetDSCP(46)
	for {
		s, err := l.AcceptKCP()
		if err != nil {
			panic(err)
		}

		go handle_client(s)
	}
}
func handle_client(conn *kcp.UDPSession) {
	conn.SetWindowSize(1024, 1024)
	conn.SetNoDelay(1, 20, 2, 1)
	conn.SetStreamMode(false)
	fmt.Println("new client", conn.RemoteAddr())
	buf := make([]byte, 65536)
	count := 0
	for {
		n, err := conn.Read(buf)
		if err != nil {
			panic(err)
		}
		count++

		fmt.Println("received :" ,time.Now().Format("2006-01-02 15:04:05"),n)
		fmt.Println("received:", string(buf[:n]),n)
		conn.Write(buf[:n])
	}
}

func main() {
	fmt.Println("start :" ,time.Now().Format("2006-01-02 15:04:05"))
	server()
}
