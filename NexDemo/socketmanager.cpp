﻿#include "stdafx.h"
//#include <afxsock.h>
//#include "windows.h"
#include "socketmanager.h"

//static UINT     g_nHeartBeatLostCount;                      //心跳包丢失个数
//static bool     g_bHeartBeatKeep;                           //
//#define         TIME_HEARTBEAT_INTERVAL        5000         //时间间隔

bool CSocketManager::m_bIsSocketOpen = false;    //Socket是否开启

CSocketManager* CSocketManager::m_pSocketManager = NULL;

/*
UINT ThreadSendDadaPack(LPVOID lpParam)
{
    while (CSocketManager::m_bIsSocketOpen)
    {
        SOCKETWORK->SendDataPack();
        Sleep(100);
    }
    return 0;
}*/
/*
UINT ThreadRevcDadaPack(LPVOID lpParam)
{
    while (CSocketManager::m_bIsSocketOpen)
    {
        SOCKETWORK->RecvDataPack();
    }
    return 0;
}*/

CSocketManager::CSocketManager()
{
    //g_nHeartBeatLostCount = 0;
    //g_bHeartBeatKeep = false;

    InitializeCriticalSection(&m_csMsgLock);

    memset(m_achIP, 0, 16 );
    m_dwPort = 0;
    //m_evWaitMsg = 0;

    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    WSAStartup(sockVersion, &data);
}

CSocketManager::~CSocketManager()
{
    DeleteCriticalSection(&m_csMsgLock);
    WSACleanup();
}

CSocketManager* CSocketManager::GetSocketManager()
{
    if (CSocketManager::m_pSocketManager == NULL)
    {
        CSocketManager::m_pSocketManager = new CSocketManager();
    }
    return CSocketManager::m_pSocketManager;
}

void CSocketManager::setSocketIP(char* pchbuf)
{
    if(pchbuf == NULL)
    {
        return;
    }
    memset(m_achIP, 0, MAX_IP_LENGTH);
    int nlength = (strlen(pchbuf) >= MAX_IP_LENGTH ? MAX_IP_LENGTH: strlen(pchbuf));
    memcpy(m_achIP, pchbuf, nlength);
    m_achIP[nlength] = '\0';
}

void CSocketManager::SetSocketPort(u32 dwPort)
{
    m_dwPort = dwPort;
}

void CSocketManager::OpenSocket()
{
    m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(m_sClient == INVALID_SOCKET)
    {
        PRINTMSG("Socket creation error.\r\n");
        return;
    }
    //设置发送接受超时
    UINT nNetTimeout = 3000;//超时时间 3s
    setsockopt(m_sClient, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout,sizeof(int));
    setsockopt(m_sClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));

    // 设置为非阻塞的socket  
    int iMode = 1;  
    ioctlsocket(m_sClient, FIONBIO, (u_long FAR*)&iMode);

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(m_dwPort);
    serAddr.sin_addr.S_un.S_addr = inet_addr(m_achIP);

    // 超时时间  
    struct timeval tm;  
    tm.tv_sec  = 5;  
    tm.tv_usec = 0;  

    if(connect(m_sClient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        fd_set set;  
        FD_ZERO(&set);  
        FD_SET(m_sClient, &set);  

        if (select(-1, NULL, &set, NULL, &tm) <= 0)  
        {  
            //连接失败 
            closesocket(m_sClient);
            PRINTMSG("Socket connect failed.\r\n");
            return;
        }  
        else  
        {  
            int error = -1;  
            int optLen = sizeof(int);  
            getsockopt(m_sClient, SOL_SOCKET, SO_ERROR, (char*)&error, &optLen);   
            // 之所以下面的程序不写成三目运算符的形式， 是为了更直观， 便于注释  
            if (0 != error)  
            {  
                //连接失败 
                closesocket(m_sClient);
                PRINTMSG("Socket connect failed.\r\n");
                return ;  
            }  
        }
    }

    // 设回为阻塞socket  
    iMode = 0;  
    ioctlsocket(m_sClient, FIONBIO, (u_long FAR*)&iMode); //设置为阻塞模式  

    m_bIsSocketOpen = true;
    return;
}

void CSocketManager::CloseSocket()
{
    closesocket(m_sClient);
    CSocketManager::m_bIsSocketOpen = false;
    //OspPost(MAKEIID(AID_RKC_APP,0), UI_RKC_DISCONNECTED);
    PRINTMSG( "关闭 Scoket...");
}

bool CSocketManager::IsSocket()
{
    return CSocketManager::m_bIsSocketOpen;
}

SOCKET CSocketManager::GetSocket()
{
    return m_sClient;
}

void CSocketManager::SendDataPack( u8* pchbuf, u32 dwDataLen)
{
    if ( IsSocket() )
    {
        int ret = send(m_sClient, (const char*)pchbuf, dwDataLen, 0);
        if (ret == -1)
        {
            //发送超时
            PRINTMSG("send data timeout.\r\n");
        }
    }
}
/*
void CSocketManager::SendDataPack()
{
    if (m_evWaitMsg != 0)
    {
        PrtRkcMsg( m_evWaitMsg, emEventTypeScoketSend, "等待回应中,拒绝发送...");
        return;
    }
    EnterCriticalSection(&m_csMsgLock);
    BOOL bIsMsgEmpty = m_RkcMsgQueue.IsEmpty();
    LeaveCriticalSection(&m_csMsgLock);
    if (!bIsMsgEmpty)
    {
        CRkMessage rkmsg;
        EnterCriticalSection(&m_csMsgLock);
        m_RkcMsgQueue.PopMsg(rkmsg);
        LeaveCriticalSection(&m_csMsgLock);
        TRK100MsgHead tRK100MsgHead = *(TRK100MsgHead*)(rkmsg.GetBody());
        //设置等待回复的消息为发送的消息+1 没有回复这不可继续发送消息
        m_evWaitMsg = ntohl(tRK100MsgHead.dwEvent) + 1;
        int ret = send(m_sClient, (const char*)rkmsg.GetBody(), rkmsg.GetBodyLen(), 0);
        if (ret == -1)
        {
            //发送超时
            PrtRkcMsg( tRK100MsgHead.dwEvent, emEventTypeScoketSend, "发送超时！");
        }
        //发送后等待接受消息
        while (m_evWaitMsg != 0)
        {
            RecvDataPack();
        }
    }
}*/
/*
void CSocketManager::RecvDataPack()
{
    char recData[RK_MAXLEN_MESSAGE] = {0};
    int ret = recv(m_sClient, recData, RK_MAXLEN_MESSAGE, 0);
    if(ret>0)
    {
        recData[ret] = 0x00;
        //消息放到OSP回调线程
        CRkMessage rkmsg;
        memset(&rkmsg,0,sizeof(CRkMessage));
        TRK100MsgHead tRK100MsgHead = *(TRK100MsgHead*)(recData);
        tRK100MsgHead.dwEvent = ntohl(tRK100MsgHead.dwEvent);

        //是等待的恢复消息 释放继续发送
        if (m_evWaitMsg == tRK100MsgHead.dwEvent)
        {
            m_evWaitMsg = 0;
        }
        if (RK100_EVT_SET_HEART_BEAT_ACK == tRK100MsgHead.dwEvent)//心跳包
        {
            g_nHeartBeatLostCount = 0;//未断链 重新计数
            PrtRkcMsg( RK100_EVT_SET_HEART_BEAT_ACK, emEventTypeScoketRecv, "收到心跳包...");
        }
        else
        {
            rkmsg.SetBody(recData, sizeof(TRK100MsgHead) + tRK100MsgHead.wMsgLen);
            OspPost(MAKEIID(AID_RKC_APP,0), tRK100MsgHead.dwEvent , rkmsg.GetBody() , sizeof(TRK100MsgHead) + tRK100MsgHead.wMsgLen);
        }
    }
    else if (ret == -1)
    {
        //接受超时
        PrtRkcMsg( m_evWaitMsg, emEventTypeScoketRecv, "接受超时！");

        if (m_evWaitMsg == RK100_EVT_LOGIN_ACK)//登录超时
        {
            TRK100MsgHead tRK100MsgHead;
            memset(&tRK100MsgHead, 0, sizeof(TRK100MsgHead));
            tRK100MsgHead.dwEvent = RK100_EVT_LOGIN_ACK;
            tRK100MsgHead.wOptRtn = RK100_OPT_ERR_UNKNOWN;
            OspPost(MAKEIID(AID_RKC_APP,0), tRK100MsgHead.dwEvent , &tRK100MsgHead , sizeof(TRK100MsgHead));
        }
        m_evWaitMsg = 0;
    }
}*/
/*
void CSocketManager::StartHeartBeat()
{
    g_nHeartBeatLostCount = 0;
    //启动心跳线程
    g_bHeartBeatKeep = true;
    AfxBeginThread(ThreadHeartBeat, NULL);
}
void CSocketManager::StopHeartBeat()
{
    g_bHeartBeatKeep = false;
    g_nHeartBeatLostCount = 0;
}*/