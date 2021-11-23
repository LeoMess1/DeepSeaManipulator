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

    int resume();  // ��ʼ�߳�
    int suspend(); // �����߳�
    int destroy(); // ɱ���߳�
    int isSuspended();
    virtual int run();   //�̵߳����

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
    HANDLE hThread_;        //�߳̾��
    HANDLE hEventSuspend_;  //���ڱ�ʶ�߳��ǹ���,���ǹ���״̬
#else
    pthread_t       hThread_;
    pthread_mutex_t mutex_;
    pthread_cond_t  cond_;
#endif
    int suspended_;
};

#endif /*THREAD_H_H*/