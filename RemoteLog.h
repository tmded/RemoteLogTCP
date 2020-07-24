#ifndef _REMOTE_LOG_H_
#define _REMOTE_LOG_H_

#include <execinfo.h>
#import <Foundation/Foundation.h>
#import <CFNetwork/CFNetwork.h>
#include <string.h>
// change this to match your destination (server) IP address
#define RLOG_IP_ADDRESS @"tomslaptop.local"
#define RLOG_PORT 11909

@interface Client : NSObject<NSStreamDelegate> {
    NSOutputStream *_outputStream;
}
@end


char* ipbuf = nil; 
BOOL resolved = NO;
dispatch_queue_t queue = nil;
int sd;
Client* socketClient = nil;



@implementation Client

+ (id)sharedManager {
    static Client *sharedMyManager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedMyManager = [[self alloc] init];
    });
    return sharedMyManager;
}


- (void)initNetworkCommunication {
    CFWriteStreamRef writeStream;
    CFStringRef bruh = (CFStringRef)CFBridgingRetain(RLOG_IP_ADDRESS);
    CFStreamCreatePairWithSocketToHost(kCFAllocatorDefault, bruh, RLOG_PORT, NULL, &writeStream);
    CFRelease(bruh);
    _outputStream = (__bridge NSOutputStream *)writeStream;

    [_outputStream setDelegate:self];

    [_outputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    NSLog(@"[remotelog] connected");
    [_outputStream open];
    [self send: [[NSProcessInfo processInfo] processName]];
}

// send data to server


- (void)send:(NSString *)str {
    NSString *NetString = [NSString stringWithFormat:@"%lu:%@,",(unsigned long)((NSString *)str).length, (NSString *)str];    
    const uint8_t * rawNetString = (const uint8_t *)[NetString UTF8String];
    int code = [_outputStream write:rawNetString maxLength: strlen((const char *)rawNetString)];
    if (code == -1){
        [_outputStream close];
        NSLog(@"[remote log] error: %@", [_outputStream streamError]);
        [self initNetworkCommunication];
        return ;
    }
    //[_outputStream close];
}
@end



//__attribute__((unused)) static 
void RLogv(NSString* message)
{
    #if DEBUG
        //dispatch_async(dispatch_get_main_queue(), ^{
        if (socketClient == nil){
            //socketClient = [[Client alloc] init];
            socketClient = [Client sharedManager];
            [socketClient initNetworkCommunication];
        }
        [socketClient send:[message copy]];
    #endif
    return;
}

//__attribute__((unused)) static 
void RLog(NSString* format, ...)
{
    //#if DEBUG

        va_list args;
        va_start(args, format);
        // we have to do this here as sometimes the objects won't exist later on in the async block
        NSString * message = [[NSString alloc] initWithFormat:format arguments:args];
        va_end(args);
        //dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        if (queue == nil){
            queue=dispatch_queue_create("com.remotelog.sendthread", DISPATCH_QUEUE_SERIAL); 
        }
        dispatch_async(queue, ^{
            RLogv(message);
        });
        //});
        
    //#endif
}
#endif
