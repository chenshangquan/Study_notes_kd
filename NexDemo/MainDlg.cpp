// MainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "touch.h"
#include "MainDlg.h"
#include "afxdialogex.h"
#include "touchDlg.h"
#include "language.h"
#include "socketmanager.h"
#include "msgrecver.h"

extern CtouchDlg * g_dlg;
// CMainDlg �Ի���

IMPLEMENT_DYNAMIC(CMainDlg, CBaseDlg)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CMainDlg::IDD, pParent)
{
	m_dwBkResourceID = IDB_MAINBK;
	m_btnMin.SetImage( IDR_BTN_MIN_NORMAL, IDR_BTN_MIN_PRESS, IDR_BTN_MIN_HOVER, IDR_BTN_MIN_NORMAL);
	m_btnClose.SetImage( IDR_BTN_CLOSE_NORMAL, IDR_BTN_CLOSE_PRESS, IDR_BTN_CLOSE_HOVER, IDR_BTN_CLOSE_NORMAL);
	m_btnPicBk.SetImage( IDB_CONNECT_BK, IDB_CONNECT_BK, IDB_CONNECT_BK,IDB_CONNECT_BK);
	m_btnPicConnectFail.SetImage( IDB_CONNECT_FAIL, IDB_CONNECT_FAIL,IDB_CONNECT_FAIL,IDB_CONNECT_FAIL);
    m_btnPicUSB.SetImage(IDB_CONNECT_USB, IDB_CONNECT_USB, IDB_CONNECT_USB, IDB_CONNECT_USB);
    m_btnPicArrow.SetImage(IDB_CONNECT_ARROW, IDB_CONNECT_ARROW, IDB_CONNECT_ARROW, IDB_CONNECT_ARROW);
    m_btnPicDevice.SetImage(IDB_CONNECT_DEVICE, IDB_CONNECT_DEVICE, IDB_CONNECT_DEVICE, IDB_CONNECT_DEVICE);
	m_btnPicBkUpgrade.SetImage( IDB_UPGRADE_BK, IDB_UPGRADE_BK, IDB_UPGRADE_BK,IDB_UPGRADE_BK);
	m_pImgBK = CUtility::GetImage( IDB_MAINBK, _T("PNG") );
	m_staticPic.SetBkImage( m_pImgBK );

	m_stTitle.SetFont(11, g_achDefaultFont);
	m_stTitle.SetTextColor(DEFAULT_FONT_COLOR);

    //IP Edit
    m_editIP.SetFont(11, g_achDefaultFont);
    //m_editIP.SetTextColor(DEFAULT_FONT_COLOR, DEFAULT_FONT_COLOR);
    m_editIP.SetImage(0, 0, 0, 0, IDB_CONNECT_ARROW, IDB_CONNECT_ARROW);

    m_btnServerCnt.SetFont(10, g_achDefaultFont);
    m_btnServerCnt.SetTextColor(DEFAULT_FONT_COLOR, DEFAULT_FONT_COLOR, DEFAULT_FONT_COLOR, DEFAULT_FONT_COLOR);
    m_btnServerCnt.SetImage(IDB_BTN_NORMAL, IDB_BTN_PRESS, IDB_BTN_HOVER, IDB_BTN_NORMAL);

    m_btnStart.SetFont(10, g_achDefaultFont);
    m_btnStart.SetTextColor(DEFAULT_FONT_COLOR, DEFAULT_FONT_COLOR, DEFAULT_FONT_COLOR, DEFAULT_FONT_COLOR);
    m_btnStart.SetImage(IDB_BTN_NORMAL, IDB_BTN_PRESS, IDB_BTN_HOVER, IDB_BTN_NORMAL);
	
	m_stTip.SetFont(13, g_achDefaultFont);
	m_stTip.SetTextColor(DEFAULT_FONT_COLOR);
	m_stTip.SetTextAlign(CTransparentStatic::emAlignmentCenter);

	m_stTipDes.SetFont(10, g_achDefaultFont);
	m_stTipDes.SetTextColor(TIP_DES_FONT_COLOR);
	m_stTipDes.SetTextAlign(CTransparentStatic::emAlignmentCenter);

	m_stUpgradeTip1.SetFont(11, g_achDefaultFont);
	m_stUpgradeTip1.SetTextColor(DEFAULT_FONT_COLOR);
	
	m_stUpgradeTip2.SetFont(11, g_achDefaultFont);
	m_stUpgradeTip2.SetTextColor(DEFAULT_FONT_COLOR);
	
	m_linkIgnore.SetFont(10, g_achDefaultFont);
	m_linkIgnore.SetTextColor(UPGRADE_FONT_COLOR);

	m_bIsShowUpgrade = false;

    m_Ipv4Addr = 0;
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_stTitle);
	DDX_Control(pDX, IDC_STATIC_TIP, m_stTip);
	DDX_Control(pDX, IDC_STATIC_TIP_DES, m_stTipDes);
	DDX_Control(pDX, IDC_BTNMIN, m_btnMin);
	DDX_Control(pDX, IDC_BTNCLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BTN_PIC_BK, m_btnPicBk);
    DDX_Control(pDX, IDC_BTN_PIC_USB_BK, m_btnPicUSB);
    DDX_Control(pDX, IDC_BTN_PIC_ARROW_BK, m_btnPicArrow);
    DDX_Control(pDX, IDC_BTN_PIC_DEVICE_BK, m_btnPicDevice);
	DDX_Control(pDX, IDC_PIC_MAIN, m_staticPic);
	DDX_Control(pDX, IDC_BTN_PIC_FAIL, m_btnPicConnectFail);
	DDX_Control(pDX, IDC_PIC_CONNECT_SUCCESS, m_stGifConnectSuccess);
	DDX_Control(pDX, IDC_PIC_CONNECTING, m_stGifConnecting);
	DDX_Control(pDX, IDC_BTN_PIC_UPGRADE, m_btnPicBkUpgrade);
	DDX_Control(pDX, IDC_STATIC_UPGRADE_TIP1, m_stUpgradeTip1);
	DDX_Control(pDX, IDC_STATIC_UPGRADE_TIP2, m_stUpgradeTip2);
    DDX_Control(pDX, IDC_SYSLINK_IGNORE, m_linkIgnore);
    DDX_Control(pDX, IDC_EDIT_IP, m_editIP);
    DDX_Control(pDX, IDC_BTN_CNT, m_btnServerCnt);
    DDX_Control(pDX, IDC_BTN_START, m_btnStart);
}

BEGIN_MESSAGE_MAP(CMainDlg, CBaseDlg)
	ON_BN_CLICKED(IDC_BTNMIN, &CMainDlg::OnBnMin)
	ON_BN_CLICKED(IDC_BTNCLOSE, &CMainDlg::OnBtnClose)
    ON_BN_CLICKED(IDC_BTN_CNT, &CMainDlg::OnBtnConnect)
	ON_BN_CLICKED(IDC_BTN_START, &CMainDlg::OnBtnStart)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_IGNORE, &CMainDlg::OnNMClickSyslinkIgnore)
END_MESSAGE_MAP()

// CMainDlg ��Ϣ�������

BOOL CMainDlg::OnInitDialog()
{
	int nWidthTmp = MULX(500);
	int nHeighTmp = MULY(300);
	int width = (GetSystemMetrics ( SM_CXSCREEN )-nWidthTmp)/2; 
	int height= (GetSystemMetrics ( SM_CYSCREEN )-nHeighTmp)/2; 
	SetWindowPos( NULL, width, height, nWidthTmp, nHeighTmp, SWP_SHOWWINDOW/*SWP_NOMOVE*/ );

	CBaseDlg::OnInitDialog();
	InitUI();
	SetWindowText(_T("touch"));
	
	ShowConnectStatus(NET_STATUS_CONNECTING);

    //osp��ʼ��
    char szOspApp[] = "AirDisplay";
    const u32 dwPrior = 80;
    u32 dwLocalIP = 0;
    int nRet = g_AirDisApp.CreateApp(&szOspApp[0], AID_AIRDIS_APP, dwPrior, 300, 200);
    ASSERT(nRet == 0);

    //��ȡIP��ַ
    //m_editIP.GetWindowText()
    m_Ipv4Addr = inet_addr("127.0.0.1");

    u32 dwNodeNum = OspConnectTcpNode(m_Ipv4Addr, 60011, 10,
        3, 0, &dwLocalIP);
    if (INVALID_NODE == dwNodeNum)
    {
        PRINTMSG("Connection Result:INVALID NODE\r\n");
        return FALSE;
    }

	return TRUE;
}


BOOL CMainDlg::PreTranslateMessage( MSG* pMsg )
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if ( VK_RETURN == pMsg->wParam
			|| VK_SPACE == pMsg->wParam
			|| VK_ESCAPE == pMsg->wParam)
		{
			return TRUE;
		}
	}
	return CBaseDlg::PreTranslateMessage(pMsg);
}

void CMainDlg::InitUI()
{
	m_stTitle.SetWindowPos( NULL, MULX(16), MULY(15), 0, MULY(16), SWP_SHOWWINDOW/*SWP_NOSIZE*/ );

    m_editIP.SetWindowPos( NULL, MULX(50), MULY(15), MULX(180), MULY(18), SWP_SHOWWINDOW/*SWP_NOSIZE*/ );
    m_btnServerCnt.SetWindowPos( NULL, MULX(250), MULY(15), MULX(60), MULY(18), SWP_SHOWWINDOW/*SWP_NOSIZE*/ );
    m_btnStart.SetWindowPos( NULL, MULX(320), MULY(15), MULX(60), MULY(18), SWP_SHOWWINDOW/*SWP_NOSIZE*/ );

	m_btnMin.SetWindowPos( NULL, MULX(431), MULY(1), MULX(30), MULY(35), SWP_HIDEWINDOW/*SWP_NOSIZE*/ );
	m_btnClose.SetWindowPos( NULL, MULX(463),MULY(1), MULX(30), MULY(35), SWP_SHOWWINDOW/*SWP_NOSIZE*/ );

	m_stTip.SetWindowPos( NULL, 0, MULY(64), MULX(500), MULY(18), SWP_SHOWWINDOW/*SWP_NOSIZE*/ );

	int nTipDesTop = MULY(64 + 18 + 8);
	m_stTipDes.SetWindowPos( NULL, 0, nTipDesTop, MULX(500), MULY(16), SWP_SHOWWINDOW/*SWP_NOSIZE*/ );

	m_btnPicBk.SetWindowPos( NULL, MULX(45), MULY(115), MULX(410), MULY(130), SWP_HIDEWINDOW );
    m_btnPicUSB.SetWindowPos( NULL, MULX(109), MULY(148), MULX(82), MULY(78), SWP_HIDEWINDOW );
    m_btnPicArrow.SetWindowPos( NULL, MULX(197), MULY(183), MULX(20), MULY(8), SWP_HIDEWINDOW );
    m_btnPicDevice.SetWindowPos( NULL, MULX(223), MULY(148), MULX(168), MULY(78), SWP_HIDEWINDOW );
	m_stGifConnecting.SetWindowPos( NULL, MULX(215), MULY(187), MULX(70), MULY(6), SWP_HIDEWINDOW );
	m_btnPicConnectFail.SetWindowPos( NULL, MULX(215), MULY(179), MULX(70), MULY(22), SWP_HIDEWINDOW );
	m_stGifConnectSuccess.SetWindowPos( NULL, MULX(45), MULY(115), MULX(410), MULY(130), SWP_HIDEWINDOW );
	m_btnPicBkUpgrade.SetWindowPos( NULL, MULX(50), MULY(135), MULX(204), MULY(115), SWP_HIDEWINDOW );
	m_stUpgradeTip1.SetWindowPos( NULL, MULX(260), MULY(135), MULX(238), MULY(18), SWP_HIDEWINDOW );
	m_stUpgradeTip2.SetWindowPos( NULL, MULX(267), MULY(161), MULX(225), MULY(18), SWP_HIDEWINDOW );
	m_linkIgnore.SetWindowPos( NULL, MULX(295), MULY(201), MULX(200), MULY(18), SWP_HIDEWINDOW );

	m_stGifConnectSuccess.SetImage(IDR_GIF_CONNECT_SUCCESS);
	m_stGifConnectSuccess.SetTimerDelay(50);

	m_stGifConnecting.SetImage(IDR_GIF_CONNECTING);
}

LRESULT CMainDlg::OnNcHitTest(CPoint point)
{
	return HTCAPTION;
}

void CMainDlg::OnBnMin()
{
	g_dlg->OnMini();
}

void CMainDlg::OnBtnInfo()
{
	g_dlg->OnAbout();
}

void CMainDlg::OnBtnClose()
{
	g_dlg->OnHide();
}

void CMainDlg::OnBtnConnect()
{
    CString strImixIp(_T("127.0.0.1"));
    u16 dwSerPort = 6684;
    SOCKETWORK->setSocketIP((CW2A)strImixIp);
    SOCKETWORK->SetSocketPort(dwSerPort);

    //����socke
    //SOCKETWORK->OpenSocket();
    ShowConnectStatus(NET_STATUS_CONNECTED);

}

void CMainDlg::OnBtnStart()
{
    //Ͷ������
    BOOL32 bTPStart = TRUE;
    OspPost(MAKEIID(AID_AIRDIS_APP,0), EV_NVMPAPP_VIEWQKSHARE_Cmd, &bTPStart, sizeof(BOOL32));

    return;
}

void CMainDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	//RedrawUI( &dc );
}

void CMainDlg::ShowConnectStatus(NET_STATUS emNetStatus)
{
	if ( m_bIsShowUpgrade )
	{
		return;
	}

	switch (emNetStatus)
	{
	case NET_STATUS_CONNECTING:
		{
            if (g_dlg->m_emSendDateMode == em_To_HID)
            {
                m_stTip.SetWindowText(STRING_NET_STATUS_CONNECTING);
                m_stTipDes.SetWindowText(_T(""));
                ShowConnectPicture(CONNECTING);
            }
            else
            {
                //IP��ַ
                m_stTitle.SetWindowText(_T("IP:"));
                m_stTitle.SetWindowPos( NULL, MULX(16), MULY(15), MULX(30), MULY(16), SWP_SHOWWINDOW/*SWP_NOSIZE*/ );
                //m_editIP.SetWindowText(_T("127.0.0.1"));
                m_btnServerCnt.SetWindowText(_T("conenct"));

                m_stTip.SetWindowText(_T("�ȴ������������"));
                m_stTipDes.SetWindowText(_T(""));
                ShowConnectPicture(CONNECTING);
            }
            
		}
		break;
	case NET_STATUS_CONNECTED:
		{
            m_stTip.SetWindowText(STRING_NET_STATUS_CONNECTED);
            m_stTipDes.SetWindowText(STRING_DES_NET_STATUS_CONNECTED);
			ShowConnectPicture(CONNECT_SUCESS);
		}
		break;
	case NET_STATUS_DISCONNECTED:
		{
			m_stTip.SetWindowText(STRING_NET_STATUS_DISCONNECTED);
			m_stTipDes.SetWindowText(STRING_DES_NET_STATUS_DISCONNECTED);
			ShowConnectPicture(CONNECT_FAIL);
		}
		break;
	case NET_STATUS_NO_NETWORK:
		{
			m_stTip.SetWindowText(STRING_NET_STATUS_NO_NETWORK);
			m_stTipDes.SetWindowText(STRING_DES_NET_STATUS_NO_NETWORK);
			ShowConnectPicture(CONNECT_FAIL);
		}
		break;
	case NET_STATUS_NO_MATCH:
		{
            if (g_dlg->m_emQKPidType == em_NT30_Type)
            {
                m_stTip.SetWindowText(STRING_NET_STATUS_NO_MATCH);
                m_stTipDes.SetWindowText(STRING_DES_NET_STATUS_NO_MATCH);
                ShowConnectPicture(CONNECT_FAIL);
            }
            else
            {
                m_stTip.SetWindowText(STRING_MT_NET_STATUS_NO_MATCH);
                m_stTipDes.SetWindowText(STRING_DES_MT_NET_STATUS_NO_MATCH);
                ShowConnectPicture(CONNECT_NT30_MT_PAIR_HELP);
            }
			
		}
		break;
	case NET_STATUS_ENOUGHNUM:
		{
			m_stTip.SetWindowText(STRING_NET_STATUS_ENOUGHNUM);
			m_stTipDes.SetWindowText(STRING_DES_NET_STATUS_ENOUGHNUM);
			ShowConnectPicture(CONNECT_FAIL);
		}
		break;
	case NET_STATUS_RESETQUICKSHARE:
		{
			m_stTip.SetWindowText(STRING_NET_STATUS_RESETQUICKSHARE);
			m_stTipDes.SetWindowText(STRING_DES_NET_STATUS_RESETQUICKSHARE);
			ShowConnectPicture(CONNECT_FAIL);
		}
		break;
	case NET_STATUS_RESETWIFI:
		{
			m_stTip.SetWindowText(STRING_NET_STATUS_RESETWIFI);
			m_stTipDes.SetWindowText(STRING_DES_NET_STATUS_RESETWIFI);
			ShowConnectPicture(CONNECT_FAIL);
		}
		break;
    case NET_STATUS_FIND_SSID_FAIL:
        {
            m_stTip.SetWindowText(STRING_NET_STATUS_FIND_SSID_FAIL);
            m_stTipDes.SetWindowText(STRING_DES_NET_STATUS_FIND_SSID_FAIL);
            ShowConnectPicture(CONNECT_FAIL);
        }
        break;

	}
}

void CMainDlg::ShowConnectPicture(CONNECT_STATUS emConnectStatus)
{
	ShowUpgradeControl( false );

	switch (emConnectStatus)
	{
	case CONNECT_SUCESS:
		{
			m_stGifConnectSuccess.ShowGif();
			m_stGifConnectSuccess.ShowWindow(SW_SHOW);

			m_btnPicBk.ShowWindow(SW_HIDE);
			m_btnPicConnectFail.ShowWindow(SW_HIDE);
		    m_stGifConnecting.StopGif();
			m_stGifConnecting.ShowWindow(SW_HIDE);
		}
		break;
	case CONNECT_FAIL:
		{
			m_btnPicBk.ShowWindow(SW_SHOW);
			m_btnPicConnectFail.ShowWindow(SW_SHOW);

			m_stGifConnecting.StopGif();
			m_stGifConnecting.ShowWindow(SW_HIDE);
			m_stGifConnectSuccess.StopGif();
			m_stGifConnectSuccess.ShowWindow(SW_HIDE);
		}
		break;
	case CONNECTING:
		{
			m_btnPicBk.ShowWindow(SW_SHOW);
			m_stGifConnecting.ShowGif();
			m_stGifConnecting.ShowWindow(SW_SHOW);

			m_btnPicConnectFail.ShowWindow(SW_HIDE);
			m_stGifConnectSuccess.StopGif();
			m_stGifConnectSuccess.ShowWindow(SW_HIDE);
		}
		break;
	case CONNECT_BUSINESS_FAIL:
		{
			m_stTip.SetWindowText(STRING_CONNECT_BUSINESS_FAIL);
			m_stTipDes.SetWindowText(STRING_DES_CONNECT_BUSINESS_FAIL);
			
			m_btnPicBk.ShowWindow(SW_SHOW);
			m_btnPicConnectFail.ShowWindow(SW_SHOW);

			m_stGifConnecting.StopGif();
			m_stGifConnecting.ShowWindow(SW_HIDE);
			m_stGifConnectSuccess.StopGif();
			m_stGifConnectSuccess.ShowWindow(SW_HIDE);
		}
		break;
    case CONNECT_OVER_RESOLUTION_LIMIT:
        {
            m_stTip.SetWindowText(STRING_CONNECT_RESOLUTION_LIMITED);
            m_stTipDes.SetWindowText(STRING_DES_CONNECT_RESOLUTION_LIMITED);

            m_btnPicBk.ShowWindow(SW_SHOW);
            m_btnPicConnectFail.ShowWindow(SW_SHOW);

            m_stGifConnecting.StopGif();
            m_stGifConnecting.ShowWindow(SW_HIDE);
            m_stGifConnectSuccess.StopGif();
            m_stGifConnectSuccess.ShowWindow(SW_HIDE);
        }
        break;
    case CONNECT_NT30_MT_NONSUPPORT:
        {
            m_stTip.SetWindowText(STRING_CONNECT_NT30_MT_NONSUPPORT);
            m_stTipDes.SetWindowText(STRING_DES_CONNECT_NT30_MT_NONSUPPORT);

            m_btnPicBk.ShowWindow(SW_SHOW);
            m_btnPicConnectFail.ShowWindow(SW_SHOW);

            m_stGifConnecting.StopGif();
            m_stGifConnecting.ShowWindow(SW_HIDE);
            m_stGifConnectSuccess.StopGif();
            m_stGifConnectSuccess.ShowWindow(SW_HIDE);
        }
        break;
    case CONNECT_NT30_MT_PAIR_HELP:
        {
            m_btnPicUSB.ShowWindow(SW_SHOW);
            m_btnPicArrow.ShowWindow(SW_SHOW);
            m_btnPicDevice.ShowWindow(SW_SHOW);

            m_btnPicBk.ShowWindow(SW_HIDE);
            m_stGifConnecting.StopGif();
            m_stGifConnecting.ShowWindow(SW_HIDE);

            m_btnPicConnectFail.ShowWindow(SW_HIDE);
            m_stGifConnectSuccess.StopGif();
            m_stGifConnectSuccess.ShowWindow(SW_HIDE);
        }
        break;
	}
}

void CMainDlg::ShowUpgradeControl( bool bShow )
{
	m_bIsShowUpgrade = bShow;

	int nShow = SW_HIDE;
	if ( bShow )
	{
		nShow = SW_SHOW;
		m_stTip.SetWindowText(STRING_SHOW_UPGRADE);
		m_stTipDes.SetWindowText(_T(""));

		m_btnPicBk.ShowWindow(SW_HIDE);
		m_btnPicConnectFail.ShowWindow(SW_HIDE);
		m_stGifConnecting.StopGif();
		m_stGifConnecting.ShowWindow(SW_HIDE);
		m_stGifConnectSuccess.StopGif();
		m_stGifConnectSuccess.ShowWindow(SW_HIDE);
	}

	m_stUpgradeTip1.ShowWindow(nShow);
	m_stUpgradeTip1.SetWindowText(STRING_SHOW_UPGRADE_TIP1);
	m_stUpgradeTip2.ShowWindow(nShow);
	m_stUpgradeTip2.SetWindowText(STRING_SHOW_UPGRADE_TIP2);
	m_linkIgnore.ShowWindow(nShow);
    m_linkIgnore.SetWindowText(STRING_SHOW_LINK_IGNORE);
	m_btnPicBkUpgrade.ShowWindow(nShow);
}

void CMainDlg::OnNMClickSyslinkIgnore( NMHDR *pNMHDR, LRESULT *pResult )
{
	*pResult = 0;
	
	if ( g_dlg != NULL )
	{
		m_bIsShowUpgrade = false;
		ShowConnectStatus(g_dlg->m_bCurConnetStatus);
	}
}

void CMainDlg::HideUpgradeControlUI()
{
	if ( m_bIsShowUpgrade )
	{
		m_bIsShowUpgrade = false;
		ShowConnectStatus(g_dlg->m_bCurConnetStatus);
	}
}
