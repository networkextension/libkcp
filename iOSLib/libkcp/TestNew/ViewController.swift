//
//  ViewController.swift
//  TestNew
//
//  Created by yarshure on 8/1/2019.
//  Copyright © 2019 Kong XiangBo. All rights reserved.
//

import Cocoa
import KCP
class ViewController: NSViewController {

    var tun:KCP?
    let salt = "kcp-go"
    var recvData:Data = Data()
    let queue = DispatchQueue.init(label: "queue")
    override func viewDidLoad() {
        super.viewDidLoad()

        
        // Do any additional setup after loading the view.
    }

    
    @IBAction func send(_ sender: Any) {
        guard let tun = self.tun else {
            return
        }
        var tmp = Data()
        for i in 0..<100{
            let d = "message \(i)\n".data(using: .utf8)!
            tmp.append(d)
        }
        for _ in 0..<100 {
            
            tun.input(data: tmp)
        }
        
    }
    
    @IBAction func startAction(_ sender: Any) {
        let s = salt.data(using: .utf8)!
        let kk =  KeyManage().pkbdf2Key(pass: "it's a secrect", salt: s)
        var c = KcpConfig()
        c.dataShards = 2;
        c.parityShards = 2;
        c.iptos = 46;
        c.crypt = .ase128;
        c.key = kk
        //<25d7d7bd 51050742 d8d791f2 b653c6c8 b2366b7e 25a124cf 7a2e12ea f4ffa444>
        //[37 215 215 189 81 5 7 66 216 215 145 242 182 83 198 200 178 54 107 126 37 161 36 207 122 46 18 234 244 255 164 68]
        print(kk as NSData?)
        self.tun = KCP.init(config: c, ipaddr: "192.168.123.199", port: "9999", queue: self.queue)
        
        tun!.start({ (t) in
            
            print("started")
            
        }, recv: { (t, d) in
            self.recvData.append(d)
            let msg = String.init(data: self.recvData, encoding: .utf8)!
            print(Date(),"recv",msg)
        }, disconnect: { (t) in
            print("disconnect")
        })
    }
    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }


}

