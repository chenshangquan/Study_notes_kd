#include "stdafx.h"
#include "msgrecver.h"
//////////////////////////////////////////////////////////////////////////
//  COspAppBase
//

CAppBase::CAppBase()
{
    m_dwNodeId = INVALID_NODE;
}

CAppBase::~CAppBase()
{
}

////////////////////////////////////////////////////////////////////////////////
// ȫ��OspӦ�ó������
zTemplate<CMsgHandler, INSTANCE_NUM, CAppBase> g_AirDisApp;

////////////////////////////////////////////////////////////////////////////////
// CMsgHandler

CMsgHandler* CMsgHandler::s_pMsgHandler = NULL;

CMsgHandler::CMsgHandler()
{
    s_pMsgHandler = this;
}

CMsgHandler::~CMsgHandler()
{
    s_pMsgHandler = NULL;
}

void CMsgHandler::InstanceEntry(CMessage *const pMsg)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pMsg->event;

    // ���ݲ�ͬ����Ϣ���ͽ��д���;
    switch (wCurEvent)
    {
    case EV_NVMPAPP_REGISTER_RSP:
        //ע��ظ�
        break;
    case EV_NVMPAPP_VIEWQKSHARE_Ntf:
        //Ͷ�����֪ͨ BOOL32
        break;
    case EV_NVMPAPP_IMIX_SOCKET_LISTEN_Ntf:
        //socket �������֪ͨ u32
        break;
    case EV_NVMPAPP_IMIX_SOCKET_ACCEPT_Ntf:
        //socket accept֪ͨ
        break;
    default:

        break;
    }

    return;
}
