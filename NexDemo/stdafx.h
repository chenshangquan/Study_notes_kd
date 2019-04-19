
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��


#define WM_TRAY_NOTIFYICON      (WM_USER+1001)
#define WM_OPENHIDDEV_SUCCESS   (WM_USER+1002)
#define WM_VOLUMECTRL_CHANGE    (WM_USER+1003)
#define WM_PCSLEEP_MSG          (WM_USER+1004)
#define WM_PCSLEEPRESUME_MSG    (WM_USER+1005)
#define WM_PCREBOOTCLOSE_MSG    (WM_USER+1006)
#define WM_PPT_PLAY_MSG         (WM_USER+1007)
#define WM_POWER_STATUS_MSG     (WM_USER+1008)


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#include "kdvtype.h"

#include "TransparentBaseDlg.h"
#include "TransparentBtn.h"
#include "TransparentEdit.h"
#include "transparentprogressctrl.h"
#include "uicommon.h"

#include "CommInterfaceDll.h"
#pragma comment(lib, "CommInterfaceDll.lib")

#define _WIN32_WINNT 0x0600

//telnetĬ����ʾ����
#define  DEF_TELNET_NAME "NexTransmitter"

//����Ĭ����Ƶ�����豸
#define  DEF_AUDIO_NAME _T("NexTransmitter")

#define TOUCH_VERSION_ID	   "6.0.0.1.0."
#define MAX_VER_LEN            38

#define MAX_NAME_LEN           32  //�������ֳ���
#define AID_AIRDIS_APP            62

//ȫ�ֶ���
extern s8 g_achDefaultFont[MAX_PATH];
#define DEFAULT_FONT_COLOR Color(76,76,76)
#define TIP_DES_FONT_COLOR Color(163,163,163)
#define UPGRADE_FONT_COLOR Color(0,127,192)

//winodws��������С��Ŵ����С�ı���������Ŀ����ֵ
extern float g_fScaleWidth; //ˮƽ����
extern float g_fScaleHeight;//��ֱ����
#define MULX(x) (int)((x) * g_fScaleWidth)
#define MULY(y) (int)((y) * g_fScaleHeight)

#define HID_MODE_SYNC  0  //hid�򿪷�ʽ��ͬ��
#define HID_MODE_ASYNC 1  //hid�򿪷�ʽ���첽

typedef enum _EnumPptStatus
{
	EM_PPT_NO_PLAY  = 0,         // pptδ����״̬
	EM_PPT_PLAYING		         // ppt����״̬	
}EnumPptStatus;

enum EMLangID
{
    enumLangIdCHN   = (u8)0,    //��������
    enumLangIdENG   = (u8)1     //English
};

enum EMUILangID
{
    enumUILangAUTO   = (u8)0,    //�Զ�ʶ��
    enumUILangCHN    = (u8)1,    //���Ľ���
    enumUILangENG    = (u8)2     //Ӣ�Ľ���
};
