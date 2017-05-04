#include "socket.h"

#ifdef WIN32
static WSADATA wsaData;
#endif

#ifdef WIN32
#define mysleep(x) Sleep(x)
#else
#define mysleep(x) usleep(x*1000)
#endif
#define debug

int socket_init()
{
#ifdef WIN32
    int err;
    WORD wVersionRequested;

    wVersionRequested = MAKEWORD(1,1);
    err = WSAStartup(wVersionRequested, &wsaData);
    if(err != 0)
    {
        WSACleanup();
    }
#if 0
    if( LOBYTE(wsaData.wVersion) != 1 ||
        HIBYTE(wsaData.wVersion) != 1 )
    {
        WSACleanup();
    }
#endif
#else

#endif
    return 0;
}

int socket_exit()
{
#ifdef WIN32
    if( LOBYTE(wsaData.wVersion) != 1 ||
        HIBYTE(wsaData.wVersion) != 1 )
    {
        WSACleanup();
    }
#endif
    return 0;
}

SOCKET socket_create(int type)
{
    return socket(AF_INET, type, 0);
}

int socket_close(SOCKET s)
{
    if ( s != 0 )
    {
#ifdef WIN32
        closesocket(s);
#else
        close(s);
#endif
        s = 0;  // 设为0
        return 0;
    }
    else
        return -1;
}

int socket_connect(SOCKET s, const char *ip, int port)
{
    SOCKADDR_IN addr;
    int err = 0;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    err = connect(s, (SOCKADDR*)&addr, sizeof(SOCKADDR));
    if (err < 0)
    {
        socket_close(s);
        return -1;
    }
    return 0;
}

#define SELECT_TIMEOUT -3
#define SELECT_ERROR   -2
#define SEND_ERROR     -1
int socket_send(SOCKET s, const char *buffer, int size, int timeout)
{
    fd_set set;

    struct timeval tv = {timeout, 0};
    int ret;

    FD_ZERO(&set);
    FD_SET(s, &set);
    /* (int)(s+1): make M$ compiler happy */
    ret = select((int)(s + 1), NULL, &set, NULL, &tv);
    if (ret == 0)
    {
        return SELECT_TIMEOUT;
    }
    else if (ret == -1)
    {
        return SELECT_ERROR;
    }

    ret=send(s, buffer, (int)size, MSG_NOSIGNAL);
    if (ret == SOCKET_ERROR)
    {
        return SEND_ERROR;
    }
    return ret;
}

#define SEND_TIMEOUT -1

int socket_send_data(SOCKET s, const char *buffer, int size)
{
#define SENDSIZE 2*1024*1024
    int nBytesSent = 0;
    int nBytesThisTime;
    int nBytesOneTime = SENDSIZE;   // 每次最多发SENDSIZE
    const char *tmp = buffer;
    int iRetry = 0;
    int nLeft;
    do{
        if (size < SENDSIZE)
        {
            nBytesOneTime = (int)size - nBytesSent;
        }
        nBytesThisTime = socket_send(s, tmp, nBytesOneTime, 10); // 一次发nBytesOneTime
        
        if (SELECT_ERROR == nBytesThisTime || SEND_ERROR == nBytesThisTime)
        {
            return SEND_ERROR;
        }
        if (SELECT_TIMEOUT == nBytesThisTime)  // 超时重试
        {
            iRetry++;
            if (5 == iRetry)
            {
                return SEND_TIMEOUT;
            }
            continue;
        }

        nBytesSent += nBytesThisTime;
        tmp += nBytesThisTime;
        
        nLeft = size - nBytesSent;
        if (nLeft < nBytesOneTime)
        {
            nBytesOneTime = nLeft;
        }
    } while (nBytesSent < size);

    return nBytesSent;
}

#define RECV_ERROR -1
int socket_receive(SOCKET s, char *buffer, int size, int timeout)
{
    fd_set set;
    struct timeval tv = {timeout, 0};
    int ret;
    FD_ZERO(&set);
    FD_SET(s, &set);
    /* (int)(s+1): make M$ compiler happy */
    ret = select((int)(s + 1), &set, NULL, NULL, &tv);
    if ( ret == 0)
    {
        return SELECT_TIMEOUT;
    }
    else if (ret == -1)
    {
        return SELECT_ERROR;
    }

    ret = recv(s, buffer, (int)size, 0);
    if (ret == SOCKET_ERROR)
    {
        return RECV_ERROR;
    }

    return ret;
}

// 接收指定大小的数据，buffer由调用者分配
int socket_recieve_data(SOCKET s, char *buffer, int size)
{
    int nBytesRecieved = 0;
    int nBytesThisTime = 0;
    char *tmp = buffer;
    int nLeft = size;
    do{
        nBytesThisTime = socket_receive(s, tmp, nLeft, 10);
        if (nBytesThisTime <= 0)
        {
            break;
        }
        tmp += nBytesThisTime;
        nBytesRecieved += nBytesThisTime;
        nLeft = size - nBytesRecieved;
        mysleep(10);
    } while (nBytesRecieved < size);

    return nBytesRecieved;
}
