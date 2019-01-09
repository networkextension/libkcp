//
//  BlockCrypt.m
//  libkcp
//
//  Created by 孔祥波 on 04/05/2017.
//  Copyright © 2017 Kong XiangBo. All rights reserved.
//

#import "Crypt.h"

static NSString *saltxor = @"sH3CIVoF#rWLtJo6";
@implementation Crypt
-(instancetype)initWithKey:(NSData*)key crypto:(NSString*)crypto{
    if (self = [super init]){
        uint8_t iv[] =  {167, 115, 79, 156, 18, 172, 27, 1, 164, 21, 242, 193, 252, 120, 230, 107};
        self.initialVector = [NSData dataWithBytes:iv length:16];
        const void *ckey = key.bytes;
        const void *ivPtr = iv;
        size_t keyLen = 0;
        
        if ([crypto isEqualToString:@"aes"]){
            keyLen = 32;
            
        }else if ([crypto isEqualToString:@"aes-128"]){
            keyLen = 16;
        }else {
            keyLen = 24;
        }
        CCCryptorRef s ;
        CCCryptorStatus st;
        st = CCCryptorCreateWithMode(kCCEncrypt,
                                     kCCModeCFB,
                                     kCCAlgorithmAES,
                                     CCPadding(0),
                                     ivPtr,
                                     ckey,
                                     keyLen, nil, 0, 0, 0, &s);
        if (st != kCCSuccess){
            NSLog(@"send_ctx create error");
        }
        self.send_ctx = s;
        CCCryptorRef r;
 
        st = CCCryptorCreateWithMode(kCCDecrypt,
                                kCCModeCFB,
                                kCCAlgorithmAES,
                                CCPadding(0),
                                ivPtr,
                                ckey,
                                keyLen, nil, 0, 0, 0, &r);
        if (st != kCCSuccess){
            NSLog(@"recv_ctx create error");
        }
        self.recv_ctx = r;
    }
    return self;
}
-(NSData*)encrypt:(NSData*)data
{
    const void *dataIn = data.bytes;
    size_t length = CCCryptorGetOutputLength(self.send_ctx, [data length], true)  ;
    size_t updateLength = 0;
    NSMutableData *encryptedData = [NSMutableData dataWithLength:length];
    void *dataOut =(void *) encryptedData.bytes;
    CCCryptorStatus st  = CCCryptorUpdate(self.send_ctx, dataIn, data.length, dataOut, length, &updateLength);
    if (st != kCCSuccess){
        NSLog(@"encrypt data error");
        return  nil;
    }else {
        char *finalDataPointer = (char *)[encryptedData mutableBytes] + updateLength;
        size_t remainingLength = [encryptedData length] - updateLength;
        size_t finalLength;
        CCCryptorFinal(self.recv_ctx,
                       finalDataPointer,
                       remainingLength,
                       &finalLength);
        
        // The amount of data emitted may have been less than
        // GetOutputLength said, so truncate
        [encryptedData setLength: length + finalLength];
        return encryptedData;
    }
}
-(NSData*)decrypt:(NSData*)data
{//00153296d7244b89d4e4c2c19fe560ec986fc3d57effa2e17a5c0de80027e962876d113ee2213237
    const void *dataIn = data.bytes;
    //size_t len = data.length + kCCBlockSizeAES128;
    //int outLen = 0;
    size_t length = CCCryptorGetOutputLength(self.recv_ctx, [data length], true);
    NSMutableData *encryptedData = [NSMutableData dataWithLength:length];
    void *dataOut =(void *) encryptedData.bytes;
    
     size_t updateLength;
    CCCryptorStatus st  = CCCryptorUpdate(self.recv_ctx, dataIn, data.length, dataOut, length, &updateLength);
    if (st != kCCSuccess){
        NSLog(@"decrypt data error");
        //perror("%s",errno(st));
        return  nil;
    }else {
        // Final may emit data, put it on the end
//        char *finalDataPointer = (char *)[o mutableBytes] + updateLength;
//        size_t remainingLength = [encryptedData length] - updateLength;
        // Final may emit data, put it on the end
        char *finalDataPointer = (char *)[encryptedData mutableBytes] + updateLength;
        size_t remainingLength = [encryptedData length] - updateLength;
        size_t finalLength;
        st = CCCryptorFinal(self.recv_ctx,
                       finalDataPointer,
                       remainingLength,
                       &finalLength);
        if (st != kCCSuccess){
            NSLog(@"CCCryptorFinal data error");
        }
        // The amount of data emitted may have been less than
        // GetOutputLength said, so truncate
        [encryptedData setLength: length + finalLength];
       
        return encryptedData;
    }
}
-(void)dealloc
{
    CCCryptorRelease(self.send_ctx);
    CCCryptorRelease(self.recv_ctx);
    //super.dealloc();
}
@end
