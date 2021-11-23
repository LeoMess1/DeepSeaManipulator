#ifndef THREAD_H_H
#define THREAD_H_H

#ifdef WIN32
#include <process.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#endif

class CThread
{
public:
    CThread();
    virtual ~CThread();

    int resume();  // 开始线程
    int suspend(); // 挂起线程
    int destroy(); // 杀死线程
    int isSuspended();
    virtual int run();   //线程的入口

#ifdef WIN32
    static unsigned int __stdcall start_address( void * obj);
#else
    static void* start_address( void * obj);
#endif
protected:
    static const int THREAD_TRUE  = 1;
    static const int THREAD_FALSE = 0;
private:
#ifdef WIN32
    HANDLE hThread_;        //线程句柄
    HANDLE hEventSuspend_;  //用于标识线程是工作,还是挂起状态
#else
    pthread_t       hThread_;
    pthread_mutex_t mutex_;
    pthread_cond_t  cond_;
#endif
    int suspended_;
};

#endif /*THREAD_H_H*/