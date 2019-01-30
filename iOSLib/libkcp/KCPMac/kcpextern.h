//
//  kcpextern.h
//  libkcp
//
//  Created by yarshure on 30/1/2019.
//  Copyright © 2019 Kong XiangBo. All rights reserved.
//

#ifndef kcpextern_h
#define kcpextern_h


#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <Network/Network.h>

typedef void* CPPUDPSession;
typedef void* CPPBlockCrypt;
typedef void (^recvBlock)(char* buffer,size_t len);
#ifdef __cplusplus
extern "C"{
#endif
    CPPUDPSession DialWithOptions(const char *ip, const char *port, size_t dataShards, size_t parityShards,size_t nodelay,size_t interval,size_t resend ,size_t nc,size_t sndwnd,size_t rcvwnd,size_t mtu,size_t iptos,CPPBlockCrypt block);
    void start_connection(CPPUDPSession sess,dispatch_queue_t kcptunqueue);
    ssize_t Write(CPPUDPSession sess,const char *buf, size_t sz);
    void start_send_receive_loop(CPPUDPSession sess,recvBlock didRecv);
    CPPBlockCrypt blockWith(const void* key,const char* crypt);
    //dec test
    void decrypt(CPPBlockCrypt block ,void *buffer, size_t length,size_t *outlen);
#ifdef __cplusplus
}
#endif

#endif /* kcpextern_h */
