//
//  ViewController.m
//  kcptest
//
//  Created by 孔祥波 on 28/04/2017.
//  Copyright © 2017 Kong XiangBo. All rights reserved.
//

#import "ViewController.h"
#import "BlockCrypt.h"
#import "Crypt.h"
@interface ViewController ()
@property (weak, nonatomic) IBOutlet UITextField *addr;
@property (weak, nonatomic) IBOutlet UITextField *port;
@property (strong,nonatomic) SFKcpTun *tun;
@property (strong,nonatomic) dispatch_queue_t dispatchqueue;
@property (nonatomic,strong) NSTimer *t;

@end

void dump(char *tag,  char *text, size_t len)
{

    int i;
    printf("%s: \n", tag);
    for (i = 0; i < len; i++){
        if ((i % 16) == 0 && i != 0){
            printf("\n");
        }
        if ((i % 4) == 0 && ((i%16) != 0)){
            printf(" ");
            
        }
        printf("%02x", (uint8_t)text[i]);
        
        
    }
    printf("\n");
}
@implementation ViewController
{
    NSDate *last;
    dispatch_source_t dispatchSource;
    dispatch_queue_t tqueue ;
    dispatch_queue_t socketqueue ;
    
}
- (void)viewDidLoad {
    
    [super viewDidLoad];
//    self.dispatchqueue = dispatch_queue_create("test", NULL);
    [self testCrypto];
//    [self testSodium];
    tqueue  = dispatch_queue_create("test.yarshure", DISPATCH_QUEUE_SERIAL);
    // Do any additional setup after loading the view, typically from a nib.
}
-(IBAction)tThread:(id)sender{
    last = [NSDate date];
    [NSThread detachNewThreadWithBlock:^{
        for (; ; ) {
            NSDate *n = [NSDate date];
            //NSLog(@"timer come %0.6f",[n timeIntervalSinceDate:last]);
            last = n;
            [NSThread sleepForTimeInterval:0.003];
        }
    }];
}
-(IBAction)testTimer:(id)sender{
    
    // Create a dispatch source that'll act as a timer on the concurrent queue
    // You'll need to store this somewhere so you can suspend and remove it later on
    NSLog(@"go");
    dispatchSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0,tqueue);
    last = [NSDate date];
    // Setup params for creation of a recurring timer
    double interval = 3.0;
    dispatch_time_t startTime = dispatch_time(DISPATCH_TIME_NOW, 0);
    uint64_t intervalTime = (int64_t)(interval * NSEC_PER_MSEC);
    dispatch_source_set_timer(dispatchSource, startTime, intervalTime, 0);
    
    // Attach the block you want to run on the timer fire
    dispatch_source_set_event_handler(dispatchSource, ^{
        // Your code here
        NSDate *n = [NSDate date];
        //NSLog(@"timer come %0.6f",[n timeIntervalSinceDate:last]);
        last = n;
    });
    
    
    dispatch_resume(dispatchSource);
    
    
    
    
}
-(void)testCrypto2
{
    
}
- (void)testSodium
{
    //sodium_init();
    //NSData *s = [@"0123456789ABCDEF0123456789ABCDEF" dataUsingEncoding:NSUTF8StringEncoding];
}
- (void)testCrypto{
    
    NSData *d= [[NSData alloc] initWithBase64EncodedString:@"zY3wjDNaIsMfxg0gQKZ5yPr4NWqYrngoMPeTmG5ZPlq6Xqf/2cQpbA==" options:0];
    NSData *s = [@"0123456789ABCDEF0123456789ABCDEF" dataUsingEncoding:NSUTF8StringEncoding];//
    NSLog(@"key %@",s);
//    Crypt *block = [[Crypt alloc] initWithKey:s crypto:@"aes"];  //blockWith(s.bytes, "aes");
//
//    NSData *cipher = [@"1234567890123456789012345678901234567890" dataUsingEncoding:NSUTF8StringEncoding];
    
//    for (int i = 1; i< 40 ; i++) {
//        Crypt *block = [[Crypt alloc] initWithKey:s crypto:@"aes"];  //blockWith(s.bytes, "aes");
//
//        NSData *cipher = [@"1234567890123456789012345678901234567890" dataUsingEncoding:NSUTF8StringEncoding];
//        NSMutableData *mcipher = [cipher subdataWithRange:NSMakeRange(0, i)].mutableCopy;
//        NSData *outData =[block encrypt:mcipher];
//        //block->encrypt((void *)mcipher.bytes, i, &outlen);
//        // NSData *outData = [NSData dataWithBytes:s.bytes  length:outlen];
//        //NSLog(@"enc result idx:%d %@",i,outData);
//        dump("enc result ", (char*)outData.bytes, i);
//
//    }
    
//    NSData *outData = [block encrypt:cipher] ;//encrypt(s.bytes, 32, &outlen);
//    //NSData *outData = [NSData dataWithBytes:outbuffer length:outlen];
//    //cd8df08c335a22c31fc60d2040a679c8faf8356a98ae782830f793986e593e5aba5ea7ffd9c4296c
//    NSLog(@"en %@",outData);
//    NSLog(@"org: %@",d);
//    if ([outData isEqualToData:d]){
//        NSLog(@"enc ok");
//    }
//    NSData *r =  [block decrypt:outData];
//    NSLog(@"result: %@",r);
//
    [self testB:d];
    
}
-(void)testB:(NSData *)d{
    NSData *s = [@"0123456789ABCDEF0123456789ABCDEF" dataUsingEncoding:NSUTF8StringEncoding];
    NSLog(@"org %@",s);
    BlockCrypt *block =  BlockCrypt::blockWith(s.bytes, "aes"); //blockWith(s.bytes, "aes");
    NSData *cipher = [@"1234567890123456789012345678901234567890" dataUsingEncoding:NSUTF8StringEncoding];
    
    size_t outlen=0;
    for (int i = 1; i< 40 ; i++) {
        
        
        NSMutableData *mcipher = [cipher subdataWithRange:NSMakeRange(0, i)].mutableCopy;
        
        block->encrypt((void *)mcipher.bytes, i, &outlen);
        // NSData *outData = [NSData dataWithBytes:s.bytes  length:outlen];
        //NSLog(@"enc result idx:%d %@",i,mcipher);
        
        dump("enc result ", (char*)mcipher.bytes, i);
        block->decrypt((void *)mcipher.bytes, i, &outlen);
        dump("dec result ", (char*)mcipher.bytes, i);
    }
   
//
//    block->decrypt((void*)mcipher.bytes, mcipher.length, &outlen);
//
//    NSLog(@"dec %@",mcipher);
//    free(block);

}
- (IBAction)go:(id)sender {
    //kcptest(, );
    const char *addr = [self.addr.text UTF8String];
    NSString *port= self.port.text;
    if (self.dispatchqueue == nil) {
        self.dispatchqueue = dispatch_queue_create("tun", nil);
    }
    if (self.tun == nil) {
        TunConfig *c = [[TunConfig alloc] init];
        c.dataShards = 2;
        c.parityShards = 2;
        c.iptos = 46;
        c.crypt = @"none";
        c.key = [@"" dataUsingEncoding:NSUTF8StringEncoding];
        self.tun = [[SFKcpTun alloc] initWithConfig:c ipaddr:self.addr.text port:port queue:self.dispatchqueue];
        [self.tun startWith:^(SFKcpTun * _Nonnull tun) {
            printf("connected");
        } recv:^(SFKcpTun * _Nonnull tun, NSData * _Nonnull d) {
            NSLog(@"recv:%@",d);
        } disConnect:^(SFKcpTun * _Nonnull tun) {
            printf("disConnect");
        }];
    }
    
}
- (IBAction)send:(id)sender {
    
    if ( self.tun == nil ){
        return;
    }
    [self sendtest];
    //self.t = [NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(sendtest) userInfo:nil repeats:true];
}
- (IBAction)stop:(id)sender {
    [self.t invalidate];
}
-(void)sendtest
{
    for (int i = 0; i < 1; i++) {
        NSString *msg = [NSString stringWithFormat:@"message %d",i];
        NSData *d = [msg dataUsingEncoding:NSUTF8StringEncoding];
        //char  *ptr = (char  *)BlockCrypt::ramdonBytes(40960);
        //NSData *d = [NSData dataWithBytes:(void*)ptr length:40960];
        //free(ptr);
        [self.tun input:d];
    }
}
-(IBAction)shutdown:(id)sender
{
    if (self.tun != nil){
        [self.tun shutdownUDPSession];
        self.tun = nil;
    }
}
-(void)didRecevied:(NSData*)data{
    NSLog(@"recv: %@",data);
}
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
