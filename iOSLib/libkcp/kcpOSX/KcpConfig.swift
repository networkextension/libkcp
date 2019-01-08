//
//  KcpConfig.swift
//  KCPOSX
//
//  Created by yarshure on 8/1/2019.
//  Copyright © 2019 Kong XiangBo. All rights reserved.
//

import Foundation
//@property (nonatomic) int dataShards;
//@property (nonatomic) int parityShards;
//
//@property (nonatomic) int nodelay;
//@property (nonatomic) int interval;
//@property (nonatomic) int resend;
//@property (nonatomic) int nc;
//
//@property (nonatomic) int sndwnd;
//@property (nonatomic) int rcvwnd;
//
//@property (nonatomic) int mtu;
//
//@property (nonatomic) int iptos;
//
//@property (nonatomic) int keepAliveInterval;
//@property (nonatomic) int keepAliveTimeout;
//@property (strong,nonatomic) NSData *key; //pkbdf2Key driven
////"aes, aes-128, aes-192, salsa20, blowfish, twofish, cast5, 3des, tea, xtea, xor, none",
//@property (strong,nonatomic) NSString *crypt;
public enum KcpCryptoMethod:String {
    
    
    case ase = "aes"
    case ase128 = "aes-128"
    case ase192 = "aes-192"
    case salsa20
    case blowfish
    case twofish
    case cast5
    case des3 = "3des"
    case tea
    case xtea
    case xor
    case none
}
public struct KcpConfig {
    let dataShards:Int
    let parityShards:Int
    let nodelay:Int
    let interval:Int
    let resend:Int
    let nc:Int
    let sndwnd:Int
    let rcvwnd:Int
    let mtu:Int
    let iptos:Int
    let keepAliveInterval:Int
    let keepAliveTimeout:Int
    let key:Data
    let crypt:KcpCryptoMethod
    
}
