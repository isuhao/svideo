#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "my_utils.h"

#include "socket.h"
#include "h264_recv.h"

int ConnectCamera(const char* ip,unsigned short port,SOCKET *socket)
{
    int ret = 0;
    socket_init();
    *socket = socket_create(SOCK_STREAM);
    ret = socket_connect(*socket, ip, port);
    return ret;
}

int ReceiveVideo(const char* ip,void (*h264_recv_cb)(char *data,int size,void *ud),void *ud)
{
    SOCKET sockVideo = INVALID_SOCKET;
     char pbInfo1[1 * 1024 * 1024] = {0};
     char pbData1[4 * 1024 * 1024] = {0};
    int ret = 0;

	WORD wPort = 9901;
	INFO_HEADER header;
	INFO_HEADER cInfoResHeader;

	BLOCK_HEADER block;
	int iHeaderSize = sizeof(header);
	int iRecvLen = 0;

	int iResLen = sizeof(cInfoResHeader);
	cInfoResHeader.dwType = CAMERA_THROB_RESPONSE;
	cInfoResHeader.dwDataLen = 0;
	cInfoResHeader.dwInfoLen = 0;

    int cnt = 0;
	
    while (1)
    {
        //printf("ready to connect: %s\n", ip);
        // 连接
        if (sockVideo == INVALID_SOCKET || ret < 0)
        {
	    printf("connect...%s\n",ip);
            ret = ConnectCamera(ip,wPort,&sockVideo);
            if (ret < 0 || sockVideo == INVALID_SOCKET)
            {
                printf("connect fail\n");
                sleep(1);
                break;
            }
        }
        //printf("connect success.\n");
        // 读头部信息
        iRecvLen = socket_recieve_data(sockVideo,(char *)&header,iHeaderSize);
       if (iRecvLen != iHeaderSize)
        {
            printf("read head info failed... %d\n", iRecvLen);
            socket_close(sockVideo);
            sleep(1);
            break;
        }

        //回应心跳包
	if (header.dwType == CAMERA_THROB && header.dwInfoLen == 0 && header.dwDataLen == 0)
        {
            int iLen = socket_send_data(sockVideo, (char *)&cInfoResHeader, iResLen);
            if (iLen != iResLen)
            {
                socket_close(sockVideo);
 		break;
            }
            continue;
        }

	if (header.dwDataLen > 4 * 1024 * 1024)
	{
		sleep(2);
 		socket_close(sockVideo);
		break;
	}
	if (header.dwInfoLen > 4 * 1024 * 1024)
	{
		sleep(2);
 		socket_close(sockVideo);
		break;
	}

        // 信息部分
        if( header.dwInfoLen > 0 )
        {
            iRecvLen = socket_recieve_data(sockVideo, (char *)pbInfo1, header.dwInfoLen);
            if (iRecvLen != header.dwInfoLen)
            {
                socket_close(sockVideo);
                sleep(1);
                break;
            }
        }
        //printf("\r----- %5d %5d ----", cnt++, header.dwDataLen);
        //fflush(stdout);
        // 视频数据
        if( header.dwDataLen > 0 )
        {
            iRecvLen = socket_recieve_data(sockVideo, (char *)pbData1, header.dwDataLen);
            if (iRecvLen != header.dwDataLen)
            {
                socket_close(sockVideo);
                sleep(1);
                break;
            }
            // 视频流
            if (header.dwType == CAMERA_VIDEO)  // 保存、显示
            {
		h264_recv_cb(pbData1,iRecvLen,ud);
            }

        }
    }
    return 0;
}




