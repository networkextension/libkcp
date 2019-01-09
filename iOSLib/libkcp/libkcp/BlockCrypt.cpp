//
//  BlockCrypt.cpp
//  libkcp
//
//  Created by 孔祥波 on 04/05/2017.
//  Copyright © 2017 Kong XiangBo. All rights reserved.
//

#include "BlockCrypt.h"
#import <Security/Security.h>
#include "kcpextern.h"
#include <Security/SecRandom.h>
const  uint8_t iv[] =  {167, 115, 79, 156, 18, 172, 27, 1, 164, 21, 242, 193, 252, 120, 230, 107};
BlockCrypt*
BlockCrypt::blockWith(const void* key,const char* crypto){
    if (strcmp(crypto, "none") == 0) {
        return NULL;
    }
    BlockCrypt *block = new (BlockCrypt);
    
    
    const void *ivPtr = iv;
    
    
    if (strcmp(crypto,"aes") == 0){
        block->keyLen = 32;
        
    }else if (strcmp(crypto,"aes-128") == 0){
        block->keyLen = 16;
        
    }else {
        block->keyLen = 24;
        
    }
    void *newKey = malloc(block->keyLen);
    memcpy(newKey, key, block->keyLen);
    block->key = newKey;
    printf("enc keylen:%d,key:%s",block->keyLen, key);
//    CCCryptorStatus st = CCCryptorCreateWithMode(
//                            kCCEncrypt,
//                            kCCModeCFB,
//                            kCCAlgorithmAES,
//                            ccNoPadding,
//                            ivPtr, key, block->keyLen,
//                            NULL, 0, 0, 0,
//                            &(block->send_ctx));
//
//    if (st != kCCSuccess){
//        printf("send_ctx create error\n");
//    }
//    st =  CCCryptorCreateWithMode(
//                                  kCCDecrypt,
//                                  kCCModeCFB,
//                                  kCCAlgorithmAES,
//                                  ccNoPadding,
//                                  ivPtr, key, block->keyLen,
//                                  NULL, 0, 0, 0,
//                                  &(block->recv_ctx));
//    if (st != kCCSuccess){
//        printf("recv_ctx create error \n");
//    }
    
    return block;
    
}
// output udp packet
void 
BlockCrypt::encrypt(void *buffer, size_t length,size_t *outlen)
{
    CCCryptorRef send_ctx;
    
    CCCryptorStatus st  = kCCSuccess;
    const void *ivPtr = iv;
    st =  CCCryptorCreateWithMode(
                                  kCCEncrypt,
                                  kCCModeCFB,
                                  kCCAlgorithmAES,
                                  ccNoPadding,
                                  ivPtr, this->key, this->keyLen,
                                  NULL, 0, 0, 0,
                                  &(send_ctx));
    if (st != kCCSuccess){
        printf("dec CCCryptorCreateWithMode  error \n");
    }
    
    size_t nlength = CCCryptorGetOutputLength(send_ctx,  length, true)  ;
    void *dataOut = malloc(nlength);
    size_t updateLength = 0;
    st  = CCCryptorUpdate(send_ctx, buffer, length, dataOut, nlength, &updateLength);
    if (st != kCCSuccess){
        printf("encrypt data error");
       
    }else {
        char *finalDataPointer = (char *)dataOut + updateLength;
        size_t remainingLength = nlength - updateLength;
        
        size_t finalLength;
        CCCryptorFinal(send_ctx,
                       finalDataPointer,
                       remainingLength,
                       &finalLength);
        *outlen = length + finalLength;
        memcpy(buffer, dataOut, length + finalLength);
        
    }
    CCCryptorRelease(send_ctx);
    free(dataOut);
}
void
BlockCrypt::decrypt(void *buffer, size_t length,size_t *outlen)
{
   
    CCCryptorRef recv_ctx;
    CCCryptorStatus st  = kCCSuccess;
    const void *ivPtr = iv;
    st =  CCCryptorCreateWithMode(
                                  kCCDecrypt,
                                  kCCModeCFB,
                                  kCCAlgorithmAES,
                                  ccNoPadding,
                                  ivPtr, this->key, this->keyLen,
                                  NULL, 0, 0, 0,
                                  &(recv_ctx));
    if (st != kCCSuccess){
        printf("dec CCCryptorCreateWithMode  error \n");
    }
    
    size_t nlength = CCCryptorGetOutputLength(recv_ctx,  length, true)  ;
    void *dataOut = malloc(nlength);
    size_t updateLength = 0;
    st  = CCCryptorUpdate(recv_ctx, buffer, length, dataOut, nlength, &updateLength);
    if (st != kCCSuccess){
        printf("encrypt data error");
        
    }else {
        char *finalDataPointer = (char *)dataOut + updateLength;
        size_t remainingLength = nlength - updateLength;
        
        size_t finalLength;
        CCCryptorFinal(recv_ctx,
                       finalDataPointer,
                       remainingLength,
                       &finalLength);
        *outlen = length + finalLength;
        memcpy(buffer, dataOut, length + finalLength);
    }
    CCCryptorRelease(recv_ctx);
    free(dataOut);
}
void
BlockCrypt::Destroy(BlockCrypt *block) {
//    if (block->send_ctx != NULL) {
//        CCCryptorRelease(block->send_ctx);
//    }
//    if (block->recv_ctx != NULL) {
//        CCCryptorRelease(block->recv_ctx);
//    }
    delete block;
}
uint8_t *
BlockCrypt::ramdonBytes(size_t len){
    
    uint8_t *ptr = (uint8_t *)malloc(len);
    // Gen random bytes
    if  (SecRandomCopyBytes(kSecRandomDefault, len, (uint8_t *)ptr) == 0){
        return ptr;
    }
    free(ptr);
    return nil;
}
CPPBlockCrypt blockWith(const void* key,const char* crypt){
    BlockCrypt *block = BlockCrypt::blockWith(key, crypt);
    return (CPPBlockCrypt)block;
}
void decrypt(CPPBlockCrypt block ,void *buffer, size_t length,size_t *outlen){
    BlockCrypt *b =(BlockCrypt*)block;
    b->decrypt(buffer, length, outlen);
}
