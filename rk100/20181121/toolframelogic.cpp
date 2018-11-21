#include "stdafx.h"
#include "searchequipmentlogic.h"
#include "ToolFrameLogic.h"
#include "messageboxlogic.h"
#include "networksetuplogic.h"
#include "radiosetuplogic.h"
#include "debugmodelogic.h"
#include "workingmodelogic.h"
#include "modifypasswordlogic.h"

#define    BOTTOM_STRING    "�豸IP:%s  �豸�ͺ�:%s  �汾��:%s  ����:%.3fA  ����:%.3fW/H"      //��������leb��ʾ

template<> CToolFrameLogic* Singleton<CToolFrameLogic>::ms_pSingleton  = NULL;

static UINT g_nMsgPowerTimerID;   //������Ϣʵʱ��ȡ��ʱ��
VOID  CALLBACK  CToolFrameTimerPoc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
    if ( idEvent == g_nMsgPowerTimerID )
    {
        CRkcComInterface->GetPowerConfig();
    }
}

APP_BEGIN_MSG_MAP(CToolFrameLogic, CNotifyUIImpl)
    MSG_CREATEWINDOW(_T("ToolFrameLayout"), OnCreate)
    MSG_INIWINDOW(_T("ToolFrameLayout"), OnInit)

    MSG_SELECTCHANGE(_T("NetworkSetupOpt"), OnTabNetworkSetup)
    MSG_SELECTCHANGE(_T("RadioSetupOpt"), OnTabRadioSetup)
    MSG_SELECTCHANGE(_T("WorkingModeOpt"), OnTabWorkingMode)
    MSG_SELECTCHANGE(_T("BackupUpgradeOpt"), OnTabBackupUpgrade)
    MSG_SELECTCHANGE(_T("DebugModeOpt"), OnTabDebugModel)

    USER_MSG(UI_RKC_POWER_FLESH , OnRkcPowerReflesh)
    USER_MSG(UI_RKC_DISCONNECTED , OnRkcDisconnected)

APP_END_MSG_MAP()

CToolFrameLogic::CToolFrameLogic()
{
    g_nMsgPowerTimerID = 0;
    m_bIsFirstLogin = false;
}

CToolFrameLogic::~CToolFrameLogic()
{
}

bool CToolFrameLogic::OnCreate( TNotifyUI& msg )
{
    s32 nTop = DEFAULT_TITLE_HEIGHT;
    if ( CPaintManagerUI::IsSelfAdaption() )
    {
        float fAdpX, fAdpY;
        CPaintManagerUI::GetAdpResolution( &fAdpX, &fAdpY );
        nTop = s32(nTop * fAdpY);
    }
    SetWindowPos( m_pm->GetPaintWindow(), NULL, 0, nTop, 0, 0, SWP_NOSIZE |SWP_NOACTIVATE );
    return false;
}

bool CToolFrameLogic::OnInit( TNotifyUI& msg )
{
    REG_RCKTOOL_MSG_WND_OB(m_pm->GetPaintWindow());
    CRkcComInterface->KillTftpProcess();
    NOTIFY_MSG(UI_RKC_POWER_FLESH);
    NOTIFY_MSG(UI_RKC_NETWORK_REFLESH);
    NOTIFY_MSG(UI_RKC_WORKMODE_REFLESH);
    NOTIFY_MSG(UI_RKC_DEBUGMODE_FLESH);
    if (m_bIsFirstLogin)
    {
        CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("ToolFrameSlideTab") );
        if (pControl)
        {
            pControl->SelectItem(emTabID_ModifyPassword);
        }
        m_pm->DoCase(_T("caseMenuItemSel"));
    }

    // ��ȡ����汾��
    s8 achVersion[MAX_VER_LEN] = {0};
    UIDATAMGR->GetBuildVersion(achVersion);
    IRkcToolCommonOp::SetControlText((CA2W)achVersion, m_pm, _T("BuildVersion"));

    return true;
}

void CToolFrameLogic::InitBottomLeb()
{
    TDevItem tLoginDevitem;
    CRkcComInterface->GetLoginDevItem(tLoginDevitem);
    u32 dwIP = htonl(tLoginDevitem.tDevInfo.tAddr.dwIpaddr);
    char achBottom[256] = {0};
    sprintf(achBottom, BOTTOM_STRING, inet_ntoa(*(in_addr*)&dwIP), tLoginDevitem.tDevID.achDevType, tLoginDevitem.tDevEx.achDevSoftVersion, 0.0, 0.0);
    IRkcToolCommonOp::SetControlText( CA2T(achBottom) , m_pm ,_T("BottomLeb"));
//     IRkcToolCommonOp::SetControlText( CA2T(inet_ntoa(*(in_addr*)&dwIP)) , m_pm ,_T("BottomIPLeb"));
//     IRkcToolCommonOp::SetControlText( CA2T(tLoginDevitem.tDevID.achDevType) , m_pm ,_T("BottomTypeLeb"));
//     IRkcToolCommonOp::SetControlText( CA2T(tLoginDevitem.tDevEx.achDevSoftVersion) , m_pm ,_T("BottomVerLeb"));

    //��¼�ɹ� ��ȡ������������
    CRkcComInterface->GetWorkModeConfig();
    CRkcComInterface->GetNetWorkConfig();
    CRkcComInterface->GetPowerConfig();
    CRkcComInterface->GetDebugConfig();
    g_nMsgPowerTimerID = SetTimer( NULL, NULL, 5000, CToolFrameTimerPoc );
}

void CToolFrameLogic::SetFirstLogin(bool bFirstLogin)
{
    m_bIsFirstLogin = bFirstLogin;
    if (m_bIsFirstLogin)
    {
        CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("ToolFrameSlideTab") );
        if (pControl)
        {
            pControl->SelectItem(emTabID_ModifyPassword);
        }
        m_pm->DoCase(_T("caseMenuItemSel"));
    }
}

bool CToolFrameLogic::IsFirstLogin()
{
    return m_bIsFirstLogin;
}

bool CToolFrameLogic::OnTabNetworkSetup(TNotifyUI& msg)
{
    CSearchEquipmentLogic::GetSingletonPtr()->SetTimerOutTimer(true);
    if (IsFirstLogin())//�����޸�ǿ�Ʋ����л�tab
    {
        m_pm->DoCase(_T("caseMenuItemSel"));
        return false;
    }
    CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("ToolFrameSlideTab") );
    if (pControl)
    {
        if (IsConfigChange())
        {
            pControl->SelectItem(emTabID_NetworkSetup);
        }
        else
        {
            COptionUI* pMsgSpender = (COptionUI*)msg.pSender;
            if (pMsgSpender)
            {
                pMsgSpender->SetCheckNoMsg(false);
            }
            switch(pControl->GetCurSel())
            {
            case emTabID_NetworkSetup:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("NetworkSetupOpt"));
                break;
            case emTabID_RadioSetup:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("RadioSetupOpt"));
                break;
            case emTabID_WorkingMode:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("WorkingModeOpt"));
                break;
            case emTabID_BackupUpgrade:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("BackupUpgradeOpt"));
                break;
            case emTabID_DebugMode:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("DebugModeOpt"));
                break;
            default:
                break;
            }
        }
    }
    return true;
}

bool CToolFrameLogic::OnTabRadioSetup(TNotifyUI& msg)
{
    CSearchEquipmentLogic::GetSingletonPtr()->SetTimerOutTimer(true);
    if (IsFirstLogin())//�����޸�ǿ�Ʋ����л�tab
    {
        m_pm->DoCase(_T("caseMenuItemSel"));
        return false;
    }
    CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("ToolFrameSlideTab") );
    if (pControl)
    {
        if (IsConfigChange())
        {
            pControl->SelectItem(emTabID_RadioSetup);
        }
        else
        {
            COptionUI* pMsgSpender = (COptionUI*)msg.pSender;
            if (pMsgSpender)
            {
                pMsgSpender->SetCheckNoMsg(false);
            }
            switch(pControl->GetCurSel())
            {
            case emTabID_NetworkSetup:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("NetworkSetupOpt"));
                break;
            case emTabID_RadioSetup:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("RadioSetupOpt"));
                break;
            case emTabID_WorkingMode:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("WorkingModeOpt"));
                break;
            case emTabID_BackupUpgrade:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("BackupUpgradeOpt"));
                break;
            case emTabID_DebugMode:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("DebugModeOpt"));
                break;
            default:
                break;
            }
        }
    }
    return true;
}

bool CToolFrameLogic::OnTabWorkingMode(TNotifyUI& msg)
{
    CSearchEquipmentLogic::GetSingletonPtr()->SetTimerOutTimer(true);
    if (IsFirstLogin())//�����޸�ǿ�Ʋ����л�tab
    {
        m_pm->DoCase(_T("caseMenuItemSel"));
        return false;
    }
    CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("ToolFrameSlideTab") );
    if (pControl)
    {
        if (IsConfigChange())
        {
            pControl->SelectItem(emTabID_WorkingMode);
        }
        else
        {
            COptionUI* pMsgSpender = (COptionUI*)msg.pSender;
            if (pMsgSpender)
            {
                pMsgSpender->SetCheckNoMsg(false);
            }
            switch(pControl->GetCurSel())
            {
            case emTabID_NetworkSetup:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("NetworkSetupOpt"));
                break;
            case emTabID_RadioSetup:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("RadioSetupOpt"));
                break;
            case emTabID_WorkingMode:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("WorkingModeOpt"));
                break;
            case emTabID_BackupUpgrade:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("BackupUpgradeOpt"));
                break;
            case emTabID_DebugMode:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("DebugModeOpt"));
                break;
            default:
                break;
            }
        }
    }
    return true;
}

bool CToolFrameLogic::OnTabBackupUpgrade(TNotifyUI& msg)
{
    CSearchEquipmentLogic::GetSingletonPtr()->SetTimerOutTimer(true);
    if (IsFirstLogin())//�����޸�ǿ�Ʋ����л�tab
    {
        m_pm->DoCase(_T("caseMenuItemSel"));
        return false;
    }
    CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("ToolFrameSlideTab") );
    if (pControl)
    {
        if (IsConfigChange())
        {
            pControl->SelectItem(emTabID_BackupUpgrade);
        }
        else
        {
            COptionUI* pMsgSpender = (COptionUI*)msg.pSender;
            if (pMsgSpender)
            {
                pMsgSpender->SetCheckNoMsg(false);
            }
            switch(pControl->GetCurSel())
            {
            case emTabID_NetworkSetup:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("NetworkSetupOpt"));
                break;
            case emTabID_RadioSetup:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("RadioSetupOpt"));
                break;
            case emTabID_WorkingMode:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("WorkingModeOpt"));
                break;
            case emTabID_BackupUpgrade:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("BackupUpgradeOpt"));
                break;
            case emTabID_DebugMode:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("DebugModeOpt"));
                break;
            default:
                break;
            }
        }
    }
    return true;
}

bool CToolFrameLogic::OnTabDebugModel(TNotifyUI& msg)
{
    CSearchEquipmentLogic::GetSingletonPtr()->SetTimerOutTimer(true);
    if (IsFirstLogin())//�����޸�ǿ�Ʋ����л�tab
    {
        m_pm->DoCase(_T("caseMenuItemSel"));
        return false;
    }
    CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("ToolFrameSlideTab") );
    if (pControl)
    {
        if (IsConfigChange())
        {
            pControl->SelectItem(emTabID_DebugMode);
        }
        else
        {
            COptionUI* pMsgSpender = (COptionUI*)msg.pSender;
            if (pMsgSpender)
            {
                pMsgSpender->SetCheckNoMsg(false);
            }
            switch(pControl->GetCurSel())
            {
            case emTabID_NetworkSetup:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("NetworkSetupOpt"));
                break;
            case emTabID_RadioSetup:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("RadioSetupOpt"));
                break;
            case emTabID_WorkingMode:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("WorkingModeOpt"));
                break;
            case emTabID_BackupUpgrade:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("BackupUpgradeOpt"));
                break;
            case emTabID_DebugMode:
                IRkcToolCommonOp::OptionSelectNoMsg(true,m_pm,_T("DebugModeOpt"));
                break;
            default:
                break;
            }
        }
    }
    return true;
}

bool CToolFrameLogic::IsConfigChange()
{
    CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("ToolFrameSlideTab") );
    if (pControl)
    {
        int nCurSel = pControl->GetCurSel();
        CVerticalLayoutUI* pVer = (CVerticalLayoutUI*)pControl->GetItemAt(nCurSel);
        if (pVer)
        {
            if (nCurSel == emTabID_NetworkSetup)
            {
                CButtonUI* pSaveButton = (CButtonUI*)IRkcToolCommonOp::FindControl(m_pm,_T("SaveNetWorkBut"),pVer);
                if (pSaveButton->IsEnabled())
                {
                    int nBoxRe = ShowMessageBox(_T("���������޸ģ��Ƿ񱣴�����"),2);
                    if (nBoxRe == IDOK)
                    {
                        return CNetworkSetupLogic::GetSingletonPtr()->OnSaveNetWorkButClicked();
                    }
                    else
                    {
                        CRkcComInterface->GetNetWorkConfig();
                        return true;
                    }
                }
            }
            else if (nCurSel == emTabID_RadioSetup)
            {
                CButtonUI* pSaveButton = (CButtonUI*)IRkcToolCommonOp::FindControl(m_pm,_T("SaveRadioBut"),pVer);
                if (pSaveButton->IsEnabled())
                {
                    int nBoxRe = ShowMessageBox(_T("���������޸ģ��Ƿ񱣴�����"),2);
                    if (nBoxRe == IDOK)
                    {
                        return CRadioSetupLogic::GetSingletonPtr()->OnSaveRadioButClicked();
                    }
                    else
                    {
                        CRkcComInterface->GetWorkModeConfig();
                        return true;
                    }
                }
            }
            else if (nCurSel == emTabID_WorkingMode)
            {
                CButtonUI* pSaveButton = (CButtonUI*)IRkcToolCommonOp::FindControl(m_pm,_T("SaveWorkModeBut"),pVer);
                if (pSaveButton->IsEnabled())
                {
                    int nBoxRe = ShowMessageBox(_T("���������޸ģ��Ƿ񱣴�����"),2);
                    if (nBoxRe == IDOK)
                    {
                        return CWorkingModeLogic::GetSingletonPtr()->OnSaveWorkModeButClicked();
                    }
                    else
                    {
                        CRkcComInterface->GetWorkModeConfig();
                        return true;
                    }
                }
            }
            else if (nCurSel == emTabID_DebugMode)
            {
                CButtonUI* pSaveButton = (CButtonUI*)IRkcToolCommonOp::FindControl(m_pm,_T("SaveDebugModeBut"),pVer);
                if (pSaveButton->IsEnabled())
                {
                    int nBoxRe = ShowMessageBox(_T("���������޸ģ��Ƿ񱣴�����"),2);
                    if (nBoxRe == IDOK)
                    {
                        return CDebugModeLogic::GetSingletonPtr()->OnSaveDebugModeButClicked();
                    }
                    else
                    {
                        CRkcComInterface->GetWorkModeConfig();
                        return true;
                    }
                }
            }
            else if (nCurSel == emTabID_ModifyPassword)
            {
                CButtonUI* pSaveButton = (CButtonUI*)IRkcToolCommonOp::FindControl(m_pm,_T("SaveBtn"),pVer);
                if (pSaveButton->IsEnabled())
                {
                    int nBoxRe = ShowMessageBox(_T("�Ƿ񱣴浱ǰ�޸�����"),2);
                    if (nBoxRe == IDOK)
                    {
                        return CModifyPasswordLogic::GetSingletonPtr()->OnSaveNewPwdClicked();
                    }
                    else
                    {
                        CModifyPasswordLogic::GetSingletonPtr()->OnResetAllInput();
                        return true;
                    }
                }
            }
        }
    }
    return true;
}

void CToolFrameLogic::OnTabModifyPassword()
{
    CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("ToolFrameSlideTab") );
    if (pControl)
    {
        pControl->SelectItem(emTabID_ModifyPassword);
    }

    m_pm->DoCase(_T("caseMenuItemSel"));
}

void CToolFrameLogic::OnTabAboutInfo()
{
    CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("ToolFrameSlideTab") );
    if (pControl)
    {
        pControl->SelectItem(emTabID_AboutInfo);
    }

    m_pm->DoCase(_T("caseMenuItemSel"));
}

bool CToolFrameLogic::OnRkcPowerReflesh( WPARAM wparam, LPARAM lparam, bool& bHandle )
{
    TRK100PowerInfo tRK100PowerInfo;
    memset(&tRK100PowerInfo, 0, sizeof(TRK100PowerInfo));
    CRkcComInterface->GetPowerConfig(tRK100PowerInfo);

    TDevItem tLoginDevitem;
    CRkcComInterface->GetLoginDevItem(tLoginDevitem);

    u32 dwIP = htonl(tLoginDevitem.tDevInfo.tAddr.dwIpaddr);
    char achBottom[256] = {0};
    sprintf(achBottom, BOTTOM_STRING, inet_ntoa(*(in_addr*)&dwIP), tLoginDevitem.tDevID.achDevType, tLoginDevitem.tDevEx.achDevSoftVersion, ((float)tRK100PowerInfo.dwCurrent)/1000, ((float)tRK100PowerInfo.dwPower)/1000);
    IRkcToolCommonOp::SetControlText( CA2T(achBottom) , m_pm ,_T("BottomLeb"));

    return true;
}

bool CToolFrameLogic::OnRkcDisconnected( WPARAM wparam, LPARAM lparam, bool& bHandle )
{
    m_bIsFirstLogin = false;
    KillTimer(NULL, g_nMsgPowerTimerID);
    g_nMsgPowerTimerID = 0;
    WINDOW_MGR_PTR->ShowWindow(g_stcStrToolFrameDlg.c_str(),false);
    m_pm->DoCase(_T("caseIsnotLogin"));
    return true;
}