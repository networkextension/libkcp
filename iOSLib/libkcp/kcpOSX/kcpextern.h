//
//  kcpextern.h
//  libkcp
//
//  Created by yarshure on 8/1/2019.
//  Copyright © 2019 Kong XiangBo. All rights reserved.
//

#ifndef kcpextern_h
#define kcpextern_h
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
//#include
typedef void* CPPUDPSession;
typedef void* CPPBlockCrypt;
typedef void (^recvBlock)(char* buffer,size_t len);
#ifdef __cplusplus
extern "C"{
#endif
    //在这里写上c的代码
    
    //初始化一个Person的实例
//    sess->NoDelay(nodelay, interval, resend, nc);
//    sess->WndSize(sndwnd, rcvwnd);
//    sess->SetMtu(mtu);
//    sess->SetStreamMode(true);
//    sess->SetDSCP(iptos);
    //UDPSession DialWithOptions(const char *ip, const char *port, size_t dataShards, size_t parityShards,BlockCrypt *block)
    //CPPUDPSession DialWithOptions(const char *ip, const char *port, size_t dataShards, size_t parityShards,size_t nodelay,size_t interval,size_t resend ,size_t nc,size_t sndwnd,size_t rcvwnd,size_t mtu,size_t iptos);
    CPPUDPSession DialWithOptions(const char *ip, const char *port, size_t dataShards, size_t parityShards,size_t nodelay,size_t interval,size_t resend ,size_t nc,size_t sndwnd,size_t rcvwnd,size_t mtu,size_t iptos,CPPBlockCrypt block);
    void NWUpdate(CPPUDPSession sess);
    ssize_t Write(CPPUDPSession sess,const char *buf, size_t sz);
    void start_send_receive_loop(CPPUDPSession sess,recvBlock didRecv);
    CPPBlockCrypt blockWith(const void* key,const char* crypt);
    
    //dec test
    void decrypt(CPPBlockCrypt block ,void *buffer, size_t length,size_t *outlen);
    //person调用自我介绍的方法
    //int NoDelay(CPPUDPSession session,int nodelay, int interval, int resend, int nc);
    //void person_introduceMySelf(UDPSession person);
    //void person_hello(UDPSession person , UDPSession other);
    
    
    //带参构造器
    //UDPSession person_init_name_age_sex(const char * name , const int age , const bool sex);
    
    //析构函数
    //void person_deinit(UDPSession kcp);
    
    
#ifdef __cplusplus
}
#endif

#endif /* kcpextern_h */
