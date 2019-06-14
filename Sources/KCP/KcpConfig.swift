//
//  KcpConfig.swift
//  KCPOSX
//
//  Created by yarshure on 8/1/2019.
//  Copyright © 2019 Kong XiangBo. All rights reserved.
//

import Foundation

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
    public var dataShards:Int = 0
    public var parityShards:Int = 0
    public var nodelay:Int = 0
    public var interval:Int = 0
    public var resend:Int = 0
    public var  nc:Int = 0
    public var sndwnd:Int = 0
    public var  rcvwnd:Int = 0
    public var  mtu:Int = 0
    public var iptos:Int = 0
    public var keepAliveInterval:Int = 0
    public var keepAliveTimeout:Int = 0
    public var key:Data?
    public  var crypt:KcpCryptoMethod = .none
//    public init(key:Data,crypto:KcpCryptoMethod){
//        self.key = key
//        self.crypt = crypt
//    }
    public init() {}
}
