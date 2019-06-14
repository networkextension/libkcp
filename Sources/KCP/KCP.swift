//
//  KCP.swift
//  KCPOSX
//
//  Created by yarshure on 8/1/2019.
//  Copyright © 2019 Kong XiangBo. All rights reserved.
//

import Foundation

import libkcp


open class KCP {
    
    private let sess : CPPUDPSession
    private var queue:DispatchQueue
    private var socketqueue:DispatchQueue = DispatchQueue.init(label: "com.yarshure.kcp")

    public init(config:KcpConfig,ipaddr:String,port:String,queue: DispatchQueue) {
        self.queue = queue
        if config.crypt == .none {
            sess = DialWithOptions(ipaddr, port, config.dataShards, config.parityShards,config.nodelay,config.interval,config.resend,config.nc,config.sndwnd,config.rcvwnd,config.mtu,config.iptos,nil)
        }else {
            var block:CPPBlockCrypt?
            guard let key =  config.key else {
                fatalError("key must not nil")
                
            }
            key.withUnsafeBytes { (ptr:UnsafeRawBufferPointer)  in
                //let rawPtr = UnsafeRawPointer(u8Ptr)
                
                block = blockWith(ptr.baseAddress,config.crypt.rawValue)
                
                // ... use `rawPtr` ...
            }
            sess = DialWithOptions(ipaddr, port, config.dataShards, config.parityShards,config.nodelay,config.interval,config.resend,config.nc,config.sndwnd,config.rcvwnd,config.mtu,config.iptos,block)
            
        }
        start_connection(sess,self.socketqueue)
        
    }
    public func start(_ didConnect:@escaping (_:KCP)->Void ,recv:@escaping (_:KCP,_:Data)->Void,disconnect:@escaping (_:KCP)->Void){
    
       
        self.queue.async { [weak self] in
            guard let self = self else {return }
            didConnect(self)
        }
        
        start_send_receive_loop(sess) { [weak self] (buff, size) in
            guard let buff = buff else {return}
            guard let self = self else {return }
            let data = Data.init(bytes: buff, count: size)
            
            self.queue.async {
                recv(self,data)
            }
            
        }
    }
    public func input(data:Data){
    
        self.socketqueue.async {
            //sess
            let size = data.count
            _ = data.withUnsafeBytes { (ptr:UnsafeRawBufferPointer)  in
                let saltptr:UnsafeBufferPointer<Int8> = ptr.bindMemory(to: Int8.self)
                Write(self.sess, saltptr.baseAddress, size)
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
    deinit {
        
    }
    
}
