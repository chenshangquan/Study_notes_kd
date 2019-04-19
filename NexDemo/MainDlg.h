#pragma once
#include "dynamicwnd.h"
#include "transtatic.h"
#include "afxwin.h"


typedef enum _emCONNECT_STATUS
{
	CONNECT_SUCESS = 0,
	CONNECT_FAIL,
	CONNECTING,
	CONNECT_SCREEN_PROJECTING,      //Ͷ����
	CONNECT_BUSINESS_FAIL,          //ҵ��ҵ�
    CONNECT_OVER_RESOLUTION_LIMIT,  //�������Ʒֱ���Ͷ��
    CONNECT_NT30_MT_NONSUPPORT,     //�����ն˲�֧��Ͷ��
    CONNECT_NT30_MT_PAIR_HELP,      //MT�ͺ���԰���
}CONNECT_STATUS;


// CMainDlg �Ի���

class CMainDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMainDlg();

// �Ի�������
	enum { IDD = IDD_MAINFRAME };
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//��ʾ����״̬
	void ShowConnectStatus(NET_STATUS emNetStatus);
	//��ʾ���ӵ�ͼƬ
	void ShowConnectPicture(CONNECT_STATUS emConnectStatus);
	//��ʾ�����ؼ�
	void ShowUpgradeControl( bool bShow );
	//������������
	void HideUpgradeControlUI();
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	LRESULT OnNcHitTest(CPoint point);//������ڵ�����λ�ö���ʾ
	afx_msg void OnBnMin();
	afx_msg void OnBtnInfo();
	afx_msg void OnBtnClose();
    afx_msg void OnBtnConnect();
    afx_msg void OnBtnStart();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnUpgrade();
	afx_msg void OnNMClickSyslinkIgnore(NMHDR *pNMHDR, LRESULT *pResult);
	
	DECLARE_MESSAGE_MAP()

private:
	void InitUI();

private:
	Image   *m_pImgBK;

	CTransparentStatic	m_stTitle;
	CTransparentStatic	m_stTip;
	CTransparentStatic	m_stTipDes;
	CTransparentBtn	m_btnMin;
	CTransparentBtn	m_btnClose;
	CTransparentBtn	m_btnPicBk;
	CTransparentBtn	m_btnPicConnectFail;
    CTransparentBtn m_btnPicUSB;
    CTransparentBtn m_btnPicArrow;
    CTransparentBtn m_btnPicDevice;
	CTranStatic	m_staticPic;
	CTransparentGif m_stGifConnectSuccess;
	CTransparentGif m_stGifConnecting;
	//������ʾ���
	CTransparentBtn m_btnPicBkUpgrade;
	CTransparentStatic	m_stUpgradeTip1;
	CTransparentStatic	m_stUpgradeTip2;
	CTransparentSysLink m_linkIgnore;
    //���ӷ�����
    CTransparentBtn m_btnServerCnt;
    CTransparentBtn m_btnStart;
    CTransparentEdit m_editIP;

	bool m_bIsShowUpgrade;//�����ؼ��Ƿ��Ѿ���ʾ

    //IMIX IP��ַ
    u32     m_Ipv4Addr;
};
