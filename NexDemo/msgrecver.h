#pragma once
#include "osp.h"

#define INSTANCE_NUM	1	// ������Ϣ��ʵ������Ŀ

enum EMNvmpAppMsgSegs
{
    EV_NVMPAPP_REGISTER_REQ = 61012,        //ע������
    EV_NVMPAPP_REGISTER_RSP,                //ע��ظ�
    EV_NVMPAPP_VIEWQKSHARE_Cmd,             //Ͷ������  BOOL32
    EV_NVMPAPP_VIEWQKSHARE_Ntf,             //Ͷ�����֪ͨ BOOL32
    EV_NVMPAPP_IMIX_SOCKET_LISTEN_Ntf,      //socket �������֪ͨ u32
    EV_NVMPAPP_IMIX_SOCKET_ACCEPT_Ntf,      //socket accept֪ͨ
    EV_NVMPAPP_ASKKEYFRAME,                 //�ؼ�֡����
};

/************************************************************************/
/*                        OSPʵ������                                  */
/************************************************************************/
class CMsgHandler : public CInstance  
{
public:
    CMsgHandler();
    ~CMsgHandler();
    virtual void InstanceEntry(CMessage *const pMsg);

private:
    //������

public:
    static CMsgHandler* GetMsgHandler() { return s_pMsgHandler; }
private:
    static CMsgHandler *s_pMsgHandler;
};

class CAppBase
{
public:
    CAppBase();
    ~CAppBase();
    // ��ȡ��ǰ���ӽڵ�ı�ʶ(�̰߳�ȫ)
    u32 GetNodeId(){ return m_dwNodeId;}
    // ���õ�ǰ���ӽڵ�ı�ʶ(�̰߳�ȫ)
    void SetNodeId(u32 dwId) { m_dwNodeId = dwId; }
protected:
    u32                     m_dwNodeId;     // ��ǰ�������ӵĽڵ��ʶ
};

extern zTemplate<CMsgHandler, INSTANCE_NUM, CAppBase> g_AirDisApp;