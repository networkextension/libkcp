//
//  KCP.swift
//  KCPOSX
//
//  Created by yarshure on 8/1/2019.
//  Copyright © 2019 Kong XiangBo. All rights reserved.
//

import Foundation

import SwiftCppExtern


open class KCP {
    
    private let sess : CPPUDPSession
    private var queue:DispatchQueue
    private var socketqueue:DispatchQueue = DispatchQueue.init(label: "com.yarshure.kcp")
//CPPUDPSession DialWithOptions(const char *ip, const char *port, size_t dataShards, size_t parityShards,size_t nodelay,size_t interval,size_t resend ,size_t nc,size_t sndwnd,size_t rcvwnd,size_t mtu,size_t iptos);
    public init(config:KcpConfig,ipaddr:String,port:String,queue: DispatchQueue) {
        self.queue = queue
        if config.crypt == .none {
            sess = DialWithOptions(ipaddr, port, config.dataShards, config.parityShards,config.nodelay,config.interval,config.resend,config.nc,config.sndwnd,config.rcvwnd,config.mtu,config.iptos,nil)
        }else {
            var block:CPPBlockCrypt?
            guard let key =  config.key else {
                fatalError("key must not nil")
                
            }
            key.withUnsafeBytes { ptr  in
                //let rawPtr = UnsafeRawPointer(u8Ptr)
                
                block = blockWith(ptr,config.crypt.rawValue)
                
                // ... use `rawPtr` ...
            }
            sess = DialWithOptions(ipaddr, port, config.dataShards, config.parityShards,config.nodelay,config.interval,config.resend,config.nc,config.sndwnd,config.rcvwnd,config.mtu,config.iptos,block)
            
        }
        start_connection(sess,socketqueue)
        
    }
//    public init() {
//        
//    }
    public func start(_ didConnect:@escaping (_:KCP)->Void ,recv:@escaping (_:KCP,_:Data)->Void,disconnect:@escaping (_:KCP)->Void){
    
       
        self.queue.async {
            didConnect(self)
        }
        
        start_send_receive_loop(sess) { (buff, size) in
            guard let buff = buff else {return}
            let data = Data.init(bytes: buff, count: size)
            self.socketqueue.async {
                recv(self,data)
            }
            
        }
    }
    public func input(data:Data){
    
        self.queue.async {
            //sess
            let size = data.count
            _ = data.withUnsafeBytes { ptr  in
                //let rawPtr = UnsafeRawPointer(u8Ptr)
                Write(self.sess, ptr, size)
                // ... use `rawPtr` ...
            }
            
        }
    }
    public func useCell() ->Bool {
        return false
    }
    public func  localAddress() ->String {
        return ""
    }
    public func localPort() ->Int{
        return 0
    }
    public func shutdownUDPSession(){
        
    }
//    public init(name : String , age : Int , sex : Bool){
//        kcp = person_init_name_age_sex(name, Int32(age), sex)
//    }
//
//    open func introduceMySelf(){
//        person_introduceMySelf(kcp)
//    }
//
//    open func hello(other : KCP){
//        person_hello(kcp, other.kcp)
//    }
//
//    deinit {
//        person_deinit(kcp)
//    }
    
}
