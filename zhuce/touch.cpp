
// touch.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "touch.h"
#include "touchDlg.h"
#include "msgdispatch.h"
#include "progress.h"
#include <imagehlp.h>

#pragma comment(lib, "DbgHelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const CString g_strIniFilePath = CLogo::GetModuleFullPath() + _T("temp\\NexTransmitter.ini");
CtouchDlg * g_dlg = NULL;
// CtouchApp

BEGIN_MESSAGE_MAP(CtouchApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CtouchApp ����

CtouchApp::CtouchApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	m_bInstallYHFont = false;
}


// Ψһ��һ�� CtouchApp ����

CtouchApp theApp;

//ץ���쳣��Ϣ
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)  
{     
	// ����Dump�ļ�
	HANDLE hDumpFile = CreateFileW(L"touchException.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// Dump��Ϣ
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;
	// д��Dump�ļ�����
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
	return EXCEPTION_EXECUTE_HANDLER;  
}

int CALLBACK EnumFontFamProc (ENUMLOGFONT* lpelf, NEWTEXTMETRIC* lpntm, int nFontType, LPARAM lParam)
{
	CtouchApp* pApp = (CtouchApp*) lParam;
	
	if ( 0 == lstrcmp(lpelf->elfLogFont.lfFaceName, _T("΢���ź�"))
		|| 0 == lstrcmp(lpelf->elfLogFont.lfFaceName, _T("Microsoft YaHei")))
	{
		pApp->m_bInstallYHFont = true;
	}

	return 1;
}

// CtouchApp ��ʼ��

BOOL CtouchApp::InitInstance()
{
    TCHAR strbRunOnce[6] = {0};
    GetPrivateProfileString(_T("FLAGS"), _T("RunOnce"), _T("false"), strbRunOnce, 6, g_strIniFilePath);
    WritePrivateProfileString(_T("FLAGS"), _T("RunOnce"), _T("false"), g_strIniFilePath);

    if ( !UdiskIn() && !lstrcmp(strbRunOnce, _T("true")) )
    {
        TerminateProcess(GetCurrentProcess(), 0);
        return FALSE;
    }

	//ֻ����һ������ʵ��
	if ( !OnlyStartOne() )
	{
		TerminateProcess(GetCurrentProcess(), 0);
		return FALSE;		
	}

	//�ж��Ƿ���Ȩ�޵��ÿ�����AVCollection.dll
	if (!IsAvAuthPassed())
	{
		::MessageBox(NULL, _T("��Ȩ�޵���AVCollection.dll!"), NULL, MB_OK|MB_ICONERROR );
		TerminateProcess(GetCurrentProcess(), 0);
		return FALSE;	
	}

	//////////////////////ע��������///////////////////////////
	// ��ȡ��������Ϣ��MFCĬ�ϵ����жԻ���Ĵ�������Ϊ #32770
	WNDCLASS wc;
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);
	// �ı䴰������
	wc.lpszClassName = _T("NexTransmitterClass");
	// ע���´�����
	AfxRegisterClass(&wc);


	//�ر�windows�Զ����Ŵ���
	CloseAutoPlayWindow();

	//����ϵͳ�������ر�ʱ����ر�˳��
	SetProcessShutdownParameters(0x100, SHUTDOWN_NORETRY);

	//���ý������ȼ�Ϊ���
	SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);

	//��ȡwinodws��������С��Ŵ����С�ı���������Ŀ����ֵ
	GetZoomScaleValue();

	//����Ĭ������
	SetDefaultFont();

	//ʹ��COM
	CoInitialize(NULL);

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup( &gdiplusToken, &gdiplusStartupInput, NULL );

	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	//ץ���쳣�ļ�
	::SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);  //cash����

	if( !IsOspInitd() )
	{

		BOOL bTelnet  = FALSE ;

#ifdef _DEBUG
		bTelnet = TRUE;
#endif

		OspInit( bTelnet);
		OspSetPrompt(DEF_TELNET_NAME);

#ifndef _DEBUG
		OspTelAuthor("admin", "KedaKdv123");
#endif
		
	}

	CMsgDispatch::Init();

	CtouchDlg dlg;
	m_pMainWnd = &dlg;
	g_dlg = &dlg;
	::SetProp( m_pMainWnd->GetSafeHwnd(), AfxGetApp()->m_pszAppName, (HANDLE)1 );
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	::RemoveProp( m_pMainWnd->GetSafeHwnd(), AfxGetApp()->m_pszAppName );
	return FALSE;
}

int CtouchApp::ExitInstance()
{
	if ( TRUE == IsOspInitd())
	{
		OspQuit();
	}

	::RemoveProp( m_pMainWnd->GetSafeHwnd(), AfxGetApp()->m_pszAppName );

	return CWinApp::ExitInstance();
}

bool CtouchApp::IsAvAuthPassed()
{
	bool bRst = false;

	char szIn[Av_CheckAuth_MAX_LEN] = {0x01, 0x04, 0x08, 0x03, 0x01, 0x04, 0x08, 0x03, 0xff, 0x01, 0x04, 0x48, 0x04, 0x48, 0x04, 0x48, 
		0x04, 0x48, 0xff, 0x01, 0x04, 0x48, 0x01, 0x04, 0x08, 0x03, 0xff, 0x01, 0x04, 0x48, 0x04, 0x00};

	char szOut[Av_CheckAuth_MAX_LEN+1] = {0};

	AvMd5Encode(szOut, szIn);

	bRst = AvCheckAuth(szOut, Av_CheckAuth_MAX_LEN);

	return bRst;

}

void CtouchApp::GetZoomScaleValue()
{
	float dbZoomScaleWidth = 1.0;
	float dbZoomScaleHeight = 1.0;


	HDC desktopDc = CreateDC(_T("display"), NULL, NULL, NULL);
	// Get native resolution
	if (desktopDc != NULL)
	{
		int horizontalDPI = GetDeviceCaps( desktopDc, LOGPIXELSX );
		dbZoomScaleWidth = horizontalDPI / 96.0f;

		int verticalDPI = GetDeviceCaps( desktopDc, LOGPIXELSY );
		dbZoomScaleHeight = verticalDPI / 96.0f;

		DeleteDC(desktopDc);
	}

	if (dbZoomScaleWidth > 1.0)
	{
		g_fScaleWidth = dbZoomScaleWidth;
	}
	
	if (dbZoomScaleHeight > 1.0)
	{
		g_fScaleHeight = dbZoomScaleHeight;
	}
}

void CtouchApp::CloseAutoPlayWindow()
{
	//�ж��Ƿ���win7ϵͳ�����Ǿͷ���
	OSVERSIONINFO osvi;    
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));    
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);    
	if (!GetVersionEx(&osvi))  
	{    
		return ;    
	} 
	if (osvi.dwMajorVersion != 6 || osvi.dwMinorVersion != 1)
	{
		return;
	}

	//win7ϵͳ�ر��Զ����ŵ���
	HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(),GW_CHILD);   
	while (::IsWindow(hWndPrevious))   
	{   
		CString strTmp;
		CWnd *pWnd = CWnd::FromHandle(hWndPrevious);
		if (pWnd)
		{
			pWnd->GetWindowText(strTmp);
			if (strTmp == _T("�Զ�����") || strTmp == _T("AutoPlay"))
			{
				HWND hChildWndPrevious = ::GetWindow(hWndPrevious,GW_CHILD); 
				while (::IsWindow(hChildWndPrevious))   
				{   
					CWnd *pChildWnd = CWnd::FromHandle(hChildWndPrevious);
					if (pChildWnd)
					{
						pChildWnd->GetWindowText(strTmp);
						if (strTmp.Find(_T(DEF_TELNET_NAME)) != -1)
						{
							SendMessage(hWndPrevious, WM_CLOSE, NULL, NULL);
							return;
						}
					}
					//   ����Ѱ����һ������   
					hChildWndPrevious  = ::GetWindow(hChildWndPrevious,GW_HWNDNEXT); 
				}   
			}
		}
		//   ����Ѱ����һ������   
		hWndPrevious  = ::GetWindow(hWndPrevious,GW_HWNDNEXT); 
	}   
}

void CtouchApp::SetDefaultFont()
{
	HDC hdc=::GetDC(NULL);
	int nSt = ::EnumFontFamilies (hdc, NULL, (FONTENUMPROC) EnumFontFamProc,
		(LPARAM)this);

	if ( !m_bInstallYHFont )
	{
		sprintf(g_achDefaultFont, "����");
	}
	else
	{
		LANGID langID = GetSystemDefaultLangID();
		if ( langID != 0x0804 )//������ϵͳ
		{
			sprintf(g_achDefaultFont, "Microsoft YaHei");
		}
	}
}

BOOL UdiskIn()
{
    SetErrorMode(SEM_FAILCRITICALERRORS);  //set error mode

    DWORD dwLen = GetLogicalDriveStrings(0, NULL);
    if( dwLen == 0) //error
    {
        return false;
    }

    LPTSTR lpDriveStrings = (LPTSTR)HeapAlloc(GetProcessHeap(), 0, dwLen*sizeof(TCHAR));
    GetLogicalDriveStrings(dwLen,lpDriveStrings);

    bool bFind = false;
    CString strPathName;

    LPTSTR pszName = lpDriveStrings;
    while ( *pszName )
    {
        if (DRIVE_CDROM == GetDriveType((pszName)))
        {
            CString strTmp = pszName;
            strPathName = strTmp + _T("NexTransmitter.exe");
            if ( PathFileExists(strPathName) )
            {
                bFind = true;
                break;
            }
        }
        pszName+=lstrlen((pszName))+1;
    }
    HeapFree(GetProcessHeap(), 0, lpDriveStrings); // ʹ����Ϻ��ͷŶ�

    return bFind;
}

BOOL OnlyStartOne()
{
	HANDLE  hSem = CreateSemaphore(NULL, 1, 1, AfxGetApp()->m_pszAppName);   

	//�ź������ڣ����������һ��ʵ������   
	if (GetLastError() == ERROR_ALREADY_EXISTS)   
	{  		
		//�ر��ź������   
		CloseHandle(hSem);  

		LPCWSTR className = _T("NexTransmitterClass");
		LPCWSTR windName = NULL;

		HWND hWnd = ::FindWindow( className, windName );
		if ( IsWindow( hWnd ) )
		{
			::ShowWindow( hWnd, SW_SHOW );
			::SetForegroundWindow(hWnd);   
		}
		
		// Ѱ����ǰʵ����������   
		/*HWND   hWndPrevious = ::GetWindow(::GetDesktopWindow(),GW_CHILD);   
		while (::IsWindow(hWndPrevious))   
		{   
			//   ��鴰���Ƿ���Ԥ��ı��?   
			//   �У���������Ѱ�ҵ�����   
			if (::GetProp(hWndPrevious, AfxGetApp()->m_pszAppName))   
			{    		 
				//BOOL re = ((CIPSetterDlg)(CWnd::FromHandle(hWndPrevious))).setDestroyFlagEx(FALSE);
				//(CWnd::FromHandle(hWndPrevious))->SendMessage(WM_DESTROY_FLAG,FALSE,TRUE); 

				//   ����������С������ָ����С   
				if (::IsIconic(hWndPrevious)) 				
					::ShowWindow(hWndPrevious,SW_RESTORE);   

				HWND hdl = GetForegroundWindow();
				if (hWndPrevious !=  hdl )
				{   					
					::ShowWindow(hWndPrevious,SW_RESTORE); 
					::SetForegroundWindow(hWndPrevious);
				}

				return  FALSE;   
			}   
			//   ����Ѱ����һ������   
			hWndPrevious  = ::GetWindow(hWndPrevious,GW_HWNDNEXT); 
		}  */
		//   ǰһʵ���Ѵ��ڣ����Ҳ���������   
		//   �˳���ʵ��   
		return   FALSE;   
	} 
	return TRUE;
}



API void help()
{
	 OspPrintf(TRUE,FALSE,"\nprt nLevel:��ӡ����ΪnLevel����Ϣ����Ϣ���� 0-4��\n \
	 0Ϊ�رմ�ӡ��Ĭ������prt��Ϊ�رմ�ӡ\n \
	 1Ϊ��ӡ������Ϣ\n \
	 2Ϊ��ӡ������Ƶ������Ϣ\n \
	 3Ϊ��ӡ������Ƶ������Ϣ\n \
	 4Ϊ��ӡ������Ϣ");
	 OspPrintf(TRUE,FALSE,"\nshowdev:��ʾhid�豸��Ϣ");
	 OspPrintf(TRUE,FALSE,"\nshowsize:��ʾDatalist��С");
	 OspPrintf(TRUE,FALSE,"\nshowver:��ʾ�汾��");
	 OspPrintf(TRUE,FALSE,"\nmdver:��ʾý�ؿ�汾��");
	 OspPrintf(TRUE,FALSE,"\ndatastatus:��ʾ�����Ƿ�����");
	 OspPrintf(TRUE,FALSE,"\ncpuadjust:�Ƿ�����CPU��̬����");
	 OspPrintf(TRUE,FALSE,"\nscreen byScreen:ѡ����Ļ,��1��ʼ\n");
}

API void prt( u8 byLevel )
{
	CPrint::Print( byLevel );
	OspPrintf(TRUE,FALSE,"\nbyLevel:%d", byLevel);
}

API void showdev()
{
	if (g_dlg)
	{
		THidDevice tHidDeviceTmp = g_dlg->GetHidDevie(HID_TYPE_VIDEO);
		if (!tHidDeviceTmp.hndHidDevice)
		{
			OspPrintf(TRUE,FALSE,"\nδ�ҵ�HID�豸");
			return;
		}

		for (int i=HID_TYPE_VIDEO; i<HID_TYPE_NUM; i++)
		{
			THidDevice tHidDevice = g_dlg->GetHidDevie((HID_TYPE)i);
			OspPrintf(TRUE,FALSE,"\nHID device info:hndHidDevice:%d,nOverlapped:%d,uGetReportTimeout:%d,uSetReportTimeout:%d,wInReportBufferLength:%d,wOutReportBufferLength:%d\n",
				tHidDevice.hndHidDevice,tHidDevice.nOverlapped,tHidDevice.uGetReportTimeout,tHidDevice.uSetReportTimeout,
				tHidDevice.wInReportBufferLength,tHidDevice.wOutReportBufferLength);
		
		}
	}
}

API void showsize()
{
	OspPrintf(TRUE,FALSE,"Video:DataList size:%d\n", g_dlg->GetVideoDataList()->Size() );
	OspPrintf(TRUE,FALSE,"Audio:DataList size:%d\n", g_dlg->GetAudioDataList()->Size() );
}

API void mdver()
{
	if (g_dlg)
	{
		char achMdVer[MAX_PATH] = {0};
		g_dlg->GetEncode().GetMeidaLibVersion(achMdVer);
		OspPrintf(TRUE,FALSE,"ý�ؿ�汾:%s\n", achMdVer );
	}
}

API void showver()
{
	s8 achVersion[MAX_VER_LEN] = {0};
	CAboutDlg::GetBuildVersion(achVersion);
	OspPrintf(TRUE,FALSE,"Current version:V%s\n", achVersion );
}

API void datastatus()
{
	if (g_dlg)
	{
		OspPrintf(TRUE,FALSE,"�Ƿ���������:%d\n", g_dlg->m_bDataBlock );
	}
}

API void cpuadjust()
{
	if (g_dlg)
	{
		BOOL32 bStatus = TRUE;
		bool bRst = g_dlg->GetEncode().GetCpuAdjustStatus(bStatus);
		if ( bRst )
		{
			OspPrintf(TRUE,FALSE,"����CPU��̬����(Ĭ��1,�͵���0):%d\n", bStatus );
		}
	}
}

API void screen( u32 byScreen )
{
	if (g_dlg)
	{
		g_dlg->GetEncode().SelectCaptureScreen( byScreen );
	}
}
