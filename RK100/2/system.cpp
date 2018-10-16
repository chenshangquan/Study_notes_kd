#include "StdAfx.h"
#include "system.h"


template<> CSystem* Singleton<CSystem>::ms_pSingleton  = NULL;
const String CSystem::strMainWindow		= _T("rkctool");
const String CSystem::strSkin			= _T("skin");
const String CSystem::strDefaultSkin	= _T("skin\\");

const String g_stcStrMainFrameDlg = _T("rkctool");
const String g_stcStrLoginDlg = _T("rkclogin");
const String g_stcStrToolFrameDlg = _T("rktoolframer");
const String g_stcStrPasswordHelpDlg = _T("rkcPasswordhelp");

CSystem::CSystem(void)
{
}

CSystem::CSystem(String strFile) : m_strFile(strFile)
{
	bool bRet = IniSystem();
}


CSystem::~CSystem(void)
{

}


bool CSystem::IniSystem()
{
	// ��ʼ����Ϣ������
	if (IRkcToolMsgManager::GetSingletonPtr() == 0) new CRkcToolMsgManager();

	String strValue = _T("");
	bool bRet = DOCMNGR()->GetDoc(m_strFile, strSkin, strValue);
	CPaintManagerUI::SetResourcePath( bRet ? strValue.c_str() : strDefaultSkin.c_str() );

    //�Ƿ�����Ӧ�ֱ���
    bRet = DOCMNGR()->GetDoc(m_strFile, _T("Resolution"), strValue);
    SIZE sizeSrc;
    SIZE sizeDst;
    sizeSrc.cx = 1100;
    sizeSrc.cy = 720;

    LPTSTR pstr = NULL;
    sizeDst.cx = _tcstol( (LPCTSTR)strValue.c_str(), &pstr, 10);  
    sizeDst.cy = _tcstol(pstr + 1, &pstr, 10);

    if ( (sizeDst.cx != sizeSrc.cx) || (sizeDst.cy != sizeSrc.cy ) )
    {
        CPaintManagerUI::SetSelfAdaption(true);
        CPaintManagerUI::SetAdpResolution( sizeSrc, sizeDst );
    }


	IRkcToolCommonOp::RegLogics();

	return true;
}


bool CSystem::Launch()
{
	bool bRet;
	bRet = IWindowManager::GetSingletonPtr()->ShowWindowCenter(CSystem::strMainWindow.c_str());	
	if (bRet == true)
	{
		CPaintManagerUI::MessageLoop();
	}
	else
	{
		//PT_LOGEVENT("���������򴰿�ʧ��\n");
	}
	return false;
}