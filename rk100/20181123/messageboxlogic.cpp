#include "stdafx.h"
#include "messageboxlogic.h"

APP_BEGIN_MSG_MAP(CMessageBoxLogic, CNotifyUIImpl)
    MSG_CREATEWINDOW(_T("MessageBoxLayout"), OnCreate)
    MSG_INIWINDOW(_T("MessageBoxLayout"), OnInit)
    MSG_WINDOWDESTORY(_T("MessageBoxLayout"), OnDestroy)

    MSG_CLICK(_T("closebtn"), OnCloseBtnClicked)
    MSG_CLICK(_T("MsgBoxclosebtn"), OnCloseBtnClicked)

    MSG_CLICK(_T("OkBtn"), OnOKBtnClicked)
    MSG_CLICK(_T("CancelBtn"), OnCancelBtnClicked)
    MSG_CLICK(_T("MsgBoxOkBtn"), OnOKBtnClicked)
    MSG_CLICK(_T("MsgBoxCancelBtn"), OnCancelBtnClicked)

    MSG_CLICK(_T("NetChangeBtn"), OnNetChangeBtnClicked)
    MSG_CLICK(_T("TopSpaceLayout"), OnSpaceLayoutClicked)
    MSG_CLICK(_T("BottomSpaceLayout"), OnSpaceLayoutClicked)
    MSG_CLICK(_T("LeftSpaceLayout"), OnSpaceLayoutClicked)
    MSG_CLICK(_T("RigntSpaceLayout"), OnSpaceLayoutClicked)
    MSG_CLICK(_T("BackUpsLayout"), OnSpaceLayoutClicked)

    USER_MSG(UI_RKC_RESTONE , OnRkcRestone)
APP_END_MSG_MAP()

static u8       m_byBoxType = 0;//0 ��Ϣ���� 1 ������������

static CString  m_strText = _T("");
static CString  m_strLeftBtnText = _T("");
static u8       m_byType = 0;

static CString  m_strBackUpTime = _T("");
static CString  m_strBackUpModel = _T("");
static CString  m_strBackUpIniPath = _T("");

static CString  m_strUpdateModel = _T("");
static CString  m_strUpdateVer = _T("");
static CString  m_strUpdateVerNew = _T("");

bool showMessageBox(LPCTSTR lpstrText, u8 byType, LPCTSTR lpstrLeftText)
{
    if (WINDOW_MGR_PTR->IsWindowMinsize(g_stcStrMainFrameDlg.c_str()))
    {
        return false;
    }
    m_byBoxType = 0;

    m_strText = lpstrText;
    m_strLeftBtnText = lpstrLeftText;
    m_byType = byType;
    if (m_byType == 2)
    {
        WINDOW_MGR_PTR->ShowWindow(g_stcStrBackGroundDlg.c_str());
        return WINDOW_MGR_PTR->ShowModal(g_stcStrMessageBoxDlg.c_str());
    }
    else
    {
        return WINDOW_MGR_PTR->ShowWindow(g_stcStrMessageBoxDlg.c_str());
    }
}

bool showBackUpBox(LPCTSTR lpstrTime, LPCTSTR lpstrModel, LPCTSTR lpstrIniPath)
{
    m_byBoxType = 1;

    m_strBackUpTime = lpstrTime;
    m_strBackUpModel = lpstrModel;
    m_strBackUpIniPath = lpstrIniPath;
    return WINDOW_MGR_PTR->ShowModal(g_stcStrMessageBoxDlg.c_str());
}

bool showUpDateBox(LPCTSTR lpstrModel, LPCTSTR lpstrVer, LPCTSTR lpstrVerN)
{
    m_byBoxType = 2;

    m_strUpdateModel = lpstrModel;
    m_strUpdateVer = lpstrVer;
    m_strUpdateVerNew = lpstrVerN;
    return WINDOW_MGR_PTR->ShowModal(g_stcStrMessageBoxDlg.c_str());
}

CMessageBoxLogic::CMessageBoxLogic()
{
    m_emOperation = Emue_None;
}

CMessageBoxLogic::~CMessageBoxLogic()
{
}

bool CMessageBoxLogic::OnCreate( TNotifyUI& msg )
{
    s32 nTop = 0;
    RECT rcParent;
    HWND hparent = GetParent(m_pm->GetPaintWindow());
    GetWindowRect(hparent,&rcParent);
#if 0
    SIZE szDlg = m_pm->GetInitSize();
    s32 nLeft = (rcParent.right - rcParent.left - szDlg.cx)/2;
    nTop = (rcParent.bottom - rcParent.top - szDlg.cy)/2;
    SetWindowPos( m_pm->GetPaintWindow(), NULL, nLeft, nTop, 0, 0, SWP_NOSIZE |SWP_NOACTIVATE );
#else
    SetWindowPos( m_pm->GetPaintWindow(), NULL, rcParent.left, rcParent.top, 0, 0, SWP_NOSIZE |SWP_NOACTIVATE );
#endif
    return false;
}


bool CMessageBoxLogic::OnInit( TNotifyUI& msg )
{
    REG_RCKTOOL_MSG_WND_OB(m_pm->GetPaintWindow());
    if (m_byBoxType == 1)//����������ʾ
    {
        CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("MessageBoxTab") );
        if (pControl)
        {
            pControl->SelectItem(1);
        }
        IRkcToolCommonOp::SetControlText( m_strBackUpModel, m_pm, _T("LabelModel") );
        IRkcToolCommonOp::SetControlText( m_strBackUpTime, m_pm, _T("LabelTime") );
        //ԭNet
        TRK100NetParam tRK100NetParam;
        memset(&tRK100NetParam, 0, sizeof(TRK100NetParam));
        CRkcComInterface->GetNetWorkConfig(tRK100NetParam);

        in_addr tAddrIP;
        tAddrIP.S_un.S_addr = tRK100NetParam.dwIP;
        IRkcToolCommonOp::SetControlText( (CA2T)inet_ntoa(tAddrIP), m_pm, _T("IPOldLab") );
        tAddrIP.S_un.S_addr = tRK100NetParam.dwSubMask;
        IRkcToolCommonOp::SetControlText( (CA2T)inet_ntoa(tAddrIP), m_pm, _T("SubOldLab") );
        tAddrIP.S_un.S_addr = tRK100NetParam.dwGateway;
        IRkcToolCommonOp::SetControlText( (CA2T)inet_ntoa(tAddrIP), m_pm, _T("GateOldLab") );
        //��Net
        TCHAR tchConfig[64] = {0};
        memset(tchConfig,0,sizeof(tchConfig));
        GetPrivateProfileString(_T("TRK100NetParam"), _T("dwIP"),_T(""), tchConfig, 32, m_strBackUpIniPath);
        IRkcToolCommonOp::SetControlText( tchConfig, m_pm, _T("IPNewLab") );
        memset(tchConfig,0,sizeof(tchConfig));
        GetPrivateProfileString(_T("TRK100NetParam"), _T("dwSubMask"),_T(""), tchConfig, 32, m_strBackUpIniPath);
        IRkcToolCommonOp::SetControlText( tchConfig, m_pm, _T("SubNewLab") );
        memset(tchConfig,0,sizeof(tchConfig));
        GetPrivateProfileString(_T("TRK100NetParam"), _T("dwGateway"),_T(""), tchConfig, 32, m_strBackUpIniPath);
        IRkcToolCommonOp::SetControlText( tchConfig, m_pm, _T("GateNewLab") );

        if (wcscmp(IRkcToolCommonOp::GetControlText(m_pm, _T("IPOldLab")).c_str() , IRkcToolCommonOp::GetControlText(m_pm, _T("IPNewLab")).c_str() ) != 0 ||
            wcscmp(IRkcToolCommonOp::GetControlText(m_pm, _T("SubOldLab")).c_str() , IRkcToolCommonOp::GetControlText(m_pm, _T("SubNewLab")).c_str() ) != 0 ||
            wcscmp(IRkcToolCommonOp::GetControlText(m_pm, _T("GateOldLab")).c_str() , IRkcToolCommonOp::GetControlText(m_pm, _T("GateNewLab")).c_str() ) != 0 )
        {
            IRkcToolCommonOp::ShowControl(true, m_pm , _T("NetChangeLab"));
            IRkcToolCommonOp::ShowControl(true, m_pm , _T("NetChangeBtn"));
        }
        else
        {
            IRkcToolCommonOp::ShowControl(false, m_pm , _T("NetChangeLab"));
            IRkcToolCommonOp::ShowControl(false, m_pm , _T("NetChangeBtn"));
        }
    }
    else if (m_byBoxType == 2)//�豸������ʾ
    {
        CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("MessageBoxTab") );
        if (pControl)
        {
            pControl->SelectItem(2);
        }
        IRkcToolCommonOp::SetControlText( m_strUpdateModel, m_pm, _T("LabelUPModel") );
        IRkcToolCommonOp::SetControlText( m_strUpdateVer, m_pm, _T("LabelUPVer") );
        IRkcToolCommonOp::SetControlText( m_strUpdateVerNew, m_pm, _T("LabelUPVerNew") );
    }
    else//��ͨ��Ϣ����
    {
        CTabLayoutUI *pControl = (CTabLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("MessageBoxTab") );
        if (pControl)
        {
            pControl->SelectItem(0);
        }
        switch (m_byType)
        {
        case 0:
            m_pm->DoCase(_T("caseTypeNoBtn"));
            break;
        case 1:
            m_pm->DoCase(_T("caseTypeOneBtn"));
            break;
        case 2:
            m_pm->DoCase(_T("caseTypeTwoBtn"));
            break;
        default:
            break;
        }
        IRkcToolCommonOp::SetControlText( m_strText, m_pm, _T("LabelTip") );
        IRkcToolCommonOp::SetControlText( m_strLeftBtnText, m_pm, _T("MsgBoxOkBtn") );
    }
    return true;
}

bool CMessageBoxLogic::OnDestroy( TNotifyUI& msg )
{
    UNREG_RCKTOOL_MSG_WND_OB(m_pm->GetPaintWindow());
    return true;
}

bool CMessageBoxLogic::OnCloseBtnClicked(TNotifyUI& msg)
{
    WINDOW_MGR_PTR->ShowWindow(g_stcStrBackGroundDlg.c_str(), false);  
    WINDOW_MGR_PTR->CloseWindow(g_stcStrMessageBoxDlg.c_str(),IDCANCEL);  
    return false;
}

bool CMessageBoxLogic::OnOKBtnClicked(TNotifyUI& msg)
{
    if (m_emOperation == Emue_Restore)
    {
        CRkcComInterface->ReBootRk();
    }
    WINDOW_MGR_PTR->CloseWindow(g_stcStrMessageBoxDlg.c_str(),IDOK);  
    return false;
}

bool CMessageBoxLogic::OnCancelBtnClicked(TNotifyUI& msg)
{
    WINDOW_MGR_PTR->CloseWindow(g_stcStrMessageBoxDlg.c_str(),IDCANCEL);  
    return false;
}

bool CMessageBoxLogic::OnNetChangeBtnClicked(TNotifyUI& msg)
{
    m_pm->DoCase(_T("caseShowNetDetail"));
    return true;
}

bool CMessageBoxLogic::OnSpaceLayoutClicked(TNotifyUI& msg)
{
    m_pm->DoCase(_T("caseCloseNetDetail"));
    return true;
}

bool CMessageBoxLogic::OnRkcRestone( WPARAM wparam, LPARAM lparam, bool& bHandle )
{
    bool bIsSuccess = (bool)wparam;
    EMRK100OptRtn emErr = (EMRK100OptRtn)lparam;
    if (bIsSuccess == false)
    {
        m_pm->DoCase(_T("caseTypeOneBtn"));
        IRkcToolCommonOp::SetControlText( _T("�ָ�����ʧ�ܣ����Ժ�����"), m_pm, _T("LabelTip") );
    }
    else
    {
        m_emOperation = Emue_Restore;
        m_pm->DoCase(_T("caseTypeOneBtn"));
        IRkcToolCommonOp::SetControlText( _T("�ָ������ɹ��������µ�¼"), m_pm, _T("LabelTip") );
    }
    return true;
}
