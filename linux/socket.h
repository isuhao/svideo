#ifndef _SOCKET_H
#define _SOCKET_H


#ifdef WIN32    // Windows
    #include <winsock2.h>   // before include <windows.h>
    #pragma comment (lib,"ws2_32.lib")
    #include <windows.h>

    #define MSG_NOSIGNAL 0  // for send

#else /* linux */
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/select.h>
    #include <arpa/inet.h>
    typedef struct sockaddr SOCKADDR;
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef int SOCKET;
    #define INVALID_SOCKET  (-1)
    #define SOCKET_ERROR    (-1)

#endif

    ///////////////////////////////
    int socket_init();
    int socket_exit();
    SOCKET socket_create(int type);
    int socket_connect(SOCKET s, const char *ip, int port);
    int socket_send(SOCKET s, const char *buffer, int size, int timeout);
    int socket_send_data(SOCKET s, const char *buffer, int size);
    int socket_receive(SOCKET s, char *buffer, int size, int timeout);
    int socket_recieve_data(SOCKET s, char *buffer, int size);
    int socket_close(SOCKET s);



#endif /* _SOCKET_H */
