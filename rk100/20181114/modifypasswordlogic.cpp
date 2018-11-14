#include "stdafx.h"
#include "modifypasswordlogic.h"
#include "tipboxlogic.h"

template<> CModifyPasswordLogic* Singleton<CModifyPasswordLogic>::ms_pSingleton  = NULL;

APP_BEGIN_MSG_MAP(CModifyPasswordLogic, CNotifyUIImpl)
    MSG_SETFOCUS(_T("InputCurPwd"), OnInputCurPassword)
    MSG_SETFOCUS(_T("InputNewPwd"), OnInputNewPassword)
    MSG_SETFOCUS(_T("InputCfmPwd"), OnInputCfmPassword)

    //MSG_KILLFOCUS(_T("InputCurPwd"), OnJudgeCurPassword)
    //MSG_KILLFOCUS(_T("InputCurPwd"), OnJudgeCurPassword)
    //MSG_KILLFOCUS(_T("InputNewPwd"), OnJudgeNewPassword)
    //MSG_KILLFOCUS(_T("InputCfmPwd"), OnJudgeCfmPassword)
    MSG_KILLFOCUS(_T("PasswordHelp"), OnHidePasswordHelp)

    MSG_TEXTCHANGED(_T("InputNewPwd"), OnInputNewPwdChange)

    MSG_CLICK(_T("CurPwdVisSwitchBtn"), OnCurPwdVisibleClicked)
    MSG_CLICK(_T("NewPwdVisSwitchBtn"), OnNewPwdVisibleClicked)
    MSG_CLICK(_T("CfmPwdVisSwitchBtn"), OnCfmPwdVisibleClicked)
    MSG_CLICK(_T("TextHelp"), OnShowPasswordHelp)
    MSG_CLICK(_T("SaveBtn"), OnSaveNewPwdClicked)

    USER_MSG(UI_RKC_MODIFY_PASSWORD , OnRkcPasswordModified)
APP_END_MSG_MAP()

CModifyPasswordLogic::CModifyPasswordLogic()
{
    bCurPwdIsJudge = false;
    bCurPwdIsRight = false;
    bNewPwdIsJudge = false;
    bNewPwdIsStandard = false;
}

CModifyPasswordLogic::~CModifyPasswordLogic()
{
}

bool CModifyPasswordLogic::OnInputCurPassword(TNotifyUI& msg)
{
    m_pm->DoCase(_T("caseInputCurPwd"));
    bCurPwdIsJudge = false;
    bCurPwdIsRight = false;
    bNewPwdIsJudge = false;
    bNewPwdIsStandard = false;

    return true;
}

bool CModifyPasswordLogic::OnJudgeCurPassword(TNotifyUI& msg)
{
    TRK100LoginInfo tLoginInfo;
    CRkcComInterface->GetLoginInfo(tLoginInfo);
    bCurPwdIsJudge = true;

    CString strGetInputCurPwd = (IRkcToolCommonOp::GetControlText( m_pm ,_T("InputCurPwd"))).c_str();

    //if (!strncmp(tLoginInfo.szPwd, (CT2A)strGetInputCurPwd, 64))
    if (!strncmp("Pwd_Change", (CT2A)strGetInputCurPwd, 64))
    {
        bCurPwdIsRight = true;
        return true;
    }

    return false;
}

bool CModifyPasswordLogic::OnInputNewPassword(TNotifyUI& msg)
{
    m_pm->DoCase(_T("caseInputNewPwd"));
    bCurPwdIsJudge = false;
    bCurPwdIsRight = false;
    bNewPwdIsJudge = false;
    bNewPwdIsStandard = false;

    //�жϵ�ǰ�������ȷ��
    if (!OnJudgeCurPassword(msg))
    {
        m_pm->DoCase(_T("caseIsPwdFalse"));
        OnInputNewPwdChange(msg);
        return false;
    }

    m_pm->DoCase(_T("caseIsPwdTrue"));
    OnInputNewPwdChange(msg);

    return true;
}

bool CModifyPasswordLogic::OnInputNewPwdChange(TNotifyUI& msg)
{
    bool bIsContainNum = false;
    bool bIsContainAlp = false;
    bool bIsContainSym = false;
    //bool bIsPointChar  = false;              // �Ƿ����ַ�"."
    //bool bIsUnderlineChar = false;           // �Ƿ����ַ�"_"
    bool bIsForbiddenChar = false;           // ��"."��"_"�������ַ�
    u32 dwSymbolCnt = 0;                     // �����ַ�����

    CString strGetInputNewPwd = (IRkcToolCommonOp::GetControlText( m_pm ,_T("InputNewPwd"))).c_str();
    if (strGetInputNewPwd != _T(""))
    {
        s32 nStrLength = strGetInputNewPwd.GetLength();
        for (s32 nIndex = 0; nIndex < nStrLength; nIndex++)
        {
            s32 nUnit = strGetInputNewPwd.GetAt(nIndex);
            if (nUnit > 127)
            {
                m_pm->DoCase(_T("caseNewPwdIsNonstandard"));
                return false;
            }

            if (isalpha(nUnit))
            {
                bIsContainAlp = true;
            }
            else if (!isalnum(nUnit))
            {
                bIsContainSym = true;

                if (nUnit == '.' || nUnit == '_')
                {
                    dwSymbolCnt++;
                }
                else
                {
                    bIsForbiddenChar = true;
                }
            }
            else
            {
                bIsContainNum = true;
            }
        }

        if (nStrLength < 8)
        {
            m_pm->DoCase(_T("caseInputNewPwd"));
            return false;
        }

        if (!bIsContainSym)
        {// �����н��������֡���ĸʱ������ǿ����
            if (bIsContainAlp && bIsContainNum)
            {
                m_pm->DoCase(_T("caseNewPwdIsWeak"));
                return true;
            }

            // ���벻���Ϲ淶
            m_pm->DoCase(_T("caseNewPwdIsNonstandard"));
            return false;
        }
        else if (!bIsForbiddenChar)
        {
            if (bIsContainAlp && bIsContainNum)
            {
                if (dwSymbolCnt == 1)
                {// �����а������֡���ĸ����һ�����ַ�ʱ������ǿ����
                    m_pm->DoCase(_T("caseNewPwdIsMedium"));
                    return true;
                }

                if (dwSymbolCnt > 1)
                {// �����а������֡���ĸ�Ͷ�������ַ�ʱ������ǿ��ǿ
                    m_pm->DoCase(_T("caseNewPwdIsStrong"));
                    return true;
                }

                // ���벻���Ϲ淶
                m_pm->DoCase(_T("caseNewPwdIsNonstandard"));
                return false;
            }

            // ���벻���Ϲ淶
            m_pm->DoCase(_T("caseNewPwdIsNonstandard"));
            return false;
        }
        else
        {// ���벻���Ϲ淶
            m_pm->DoCase(_T("caseNewPwdIsNonstandard"));
            return false;
        }
    }

    //m_pm->DoCase(_T("caseInputNewPwd"));
    return true;
}

bool CModifyPasswordLogic::OnJudgeNewPassword(TNotifyUI& msg)
{
    bool bIsContainNum = false;
    bool bIsContainAlp = false;
    bool bIsContainSym = false;

    bNewPwdIsJudge = true;

    // �ж������������淶
    CString strGetInputNewPwd = (IRkcToolCommonOp::GetControlText( m_pm ,_T("InputNewPwd"))).c_str();
    if (strGetInputNewPwd == _T(""))
    {
        return false;
    }

    s32 nStrLength = strGetInputNewPwd.GetLength();
    for (s32 nIndex = 0; nIndex < nStrLength; nIndex++)
    {
        s32 nUnit = strGetInputNewPwd.GetAt(nIndex);
        if (nUnit > 127)
        {
            return false;
        }

        if (isalpha(nUnit))
        {
            bIsContainAlp = true;
        }
        else if (!isalnum(nUnit))
        {
            if (nUnit == '.' || nUnit == '_')
            {
                bIsContainSym = true;
            }
        }
        else
        {
            bIsContainNum = true;
        }
    }

    if (nStrLength < 8)
    {
        return false;
    }

    if ((bIsContainNum + bIsContainSym + bIsContainAlp) < 2)
    {
        return false;
    }

    bNewPwdIsStandard = true;

    return true;
}

bool CModifyPasswordLogic::OnInputCfmPassword(TNotifyUI& msg)
{
    OnInputNewPwdChange(msg);

    m_pm->DoCase(_T("caseInputCfmNewPwd"));
    bNewPwdIsJudge = false;
    bNewPwdIsStandard = false;

    if (!bCurPwdIsJudge)
    {
        //�жϵ�ǰ�������ȷ��
        if (!OnJudgeCurPassword(msg))
        {
            m_pm->DoCase(_T("caseIsPwdFalse"));
        }
        else
        {
            m_pm->DoCase(_T("caseIsPwdTrue"));
        }
    }

    // �ж������������淶
    if (!OnJudgeNewPassword(msg))
    {
        m_pm->DoCase(_T("caseNewPwdIsNonstandard"));
    }

    return true;    
}

bool CModifyPasswordLogic::OnJudgeCfmPassword(TNotifyUI& msg)
{
    CString strGetInputNewPwd = (IRkcToolCommonOp::GetControlText( m_pm ,_T("InputNewPwd"))).c_str();
    CString strGetConfirmNewPwd = (IRkcToolCommonOp::GetControlText( m_pm ,_T("InputCfmPwd"))).c_str();

    if (strGetInputNewPwd != strGetConfirmNewPwd)
    {
        return false;
    }

    return true;
}

bool CModifyPasswordLogic::OnCurPwdVisibleClicked(TNotifyUI& msg)
{
    CEditUI *pControl = (CEditUI*)IRkcToolCommonOp::FindControl( m_pm, _T("InputCurPwd") );
    if (pControl)
    {
        if (pControl->IsPasswordMode())
        {
            m_pm->DoCase(_T("caseCurPwdIsVisible"));
            m_pm->SetFocus(pControl);
            return true;
        }

        m_pm->DoCase(_T("caseCurPwdIsInvisible"));
        m_pm->SetFocus(pControl);
    }

    return true;
}

bool CModifyPasswordLogic::OnNewPwdVisibleClicked(TNotifyUI& msg)
{
    CEditUI *pControl = (CEditUI*)IRkcToolCommonOp::FindControl( m_pm, _T("InputNewPwd") );
    if (pControl)
    {
        if (pControl->IsPasswordMode())
        {
            m_pm->DoCase(_T("caseNewPwdIsVisible"));
            m_pm->SetFocus(pControl);
            return true;
        }

        m_pm->DoCase(_T("caseNewPwdIsInvisible"));
        m_pm->SetFocus(pControl);
    }

    return true;
}

bool CModifyPasswordLogic::OnCfmPwdVisibleClicked(TNotifyUI& msg)
{
    CEditUI *pControl = (CEditUI*)IRkcToolCommonOp::FindControl( m_pm, _T("InputCfmPwd") );
    if (pControl)
    {
        if (pControl->IsPasswordMode())
        {
            m_pm->DoCase(_T("caseCfmPwdIsVisible"));
            m_pm->SetFocus(pControl);
            return true;
        }

        m_pm->DoCase(_T("caseCfmPwdIsInvisible"));
        m_pm->SetFocus(pControl);
    }

    return true;
}

bool CModifyPasswordLogic::OnShowPasswordHelp(TNotifyUI& msg)
{
    m_pm->DoCase(_T("caseShowHelpInfo"));

    CHorizontalLayoutUI *pControl = (CHorizontalLayoutUI*)IRkcToolCommonOp::FindControl( m_pm, _T("PasswordHelp") );
    if (pControl)
    {
        m_pm->SetFocus(pControl);
    }

    return true;
}

bool CModifyPasswordLogic::OnHidePasswordHelp(TNotifyUI& msg)
{
    m_pm->DoCase(_T("caseHideHelpInfo"));

    return true;
}

bool CModifyPasswordLogic::OnSaveNewPwdClicked(TNotifyUI& msg)
{
    if (!bNewPwdIsJudge)
    {
        // �ж������������淶
        if (!OnJudgeNewPassword(msg))
        {
            m_pm->DoCase(_T("caseNewPwdIsNonstandard"));
            return false;
        }
    }

    if (!bCurPwdIsRight || !bNewPwdIsStandard )
    {
        return false;
    }

    if (!OnJudgeCfmPassword(msg))
    {
        m_pm->DoCase(_T("caseConfirmNewPwdFail"));
        return false;
    }

    m_pm->DoCase(_T("caseConfirmNewPwdPass"));

    // ����������
    CString strPassWord = (IRkcToolCommonOp::GetControlText( m_pm ,_T("InputCfmPwd"))).c_str();
    if (!CRkcComInterface->SetLoginInfo(CT2A(strPassWord)))
    {
        CRkcComInterface->SendModifyPasswordReq();
    }

    return true;
}

bool CModifyPasswordLogic::OnRkcPasswordModified( WPARAM wparam, LPARAM lparam, bool& bHandle )
{
    bool bIsModified = (bool)wparam;
    EMRK100OptRtn emErr = (EMRK100OptRtn)lparam;
    if (bIsModified == false)
    {
        /*switch (emErr)
        {
        case RK100_OPT_ERR_USER:
            ShowTip(_T("�û�������"));
            break;
        case RK100_OPT_ERR_PASSWORD:
            ShowTip(_T("�������"));
            break;
        default:
            ShowTip(_T("��¼ʧ��"));
            break;
        }*/
    }
    else
    {
        ShowTipBox(_T("�����޸ĳɹ�"));        
    }
    return true;
}