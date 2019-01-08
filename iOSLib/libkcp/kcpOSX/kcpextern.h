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

//#include
typedef void* UDPSession;

#ifdef __cplusplus
extern "C"{
#endif
    //在这里写上c的代码
    
    //初始化一个Person的实例
    //UDPSession DialWithOptions(const char *ip, const char *port, size_t dataShards, size_t parityShards,BlockCrypt *block)
    UDPSession DialWithOptions(const char *ip, const char *port, size_t dataShards, size_t parityShards);
    //person调用自我介绍的方法
    int NoDelay(int nodelay, int interval, int resend, int nc);
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
