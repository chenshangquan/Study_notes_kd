
// touchDlg.h : ͷ�ļ�
//

#pragma once
#include "MainDlg.h"
#include "BannerDlg.h"
#include "Encoder.h"
#include "AboutDlg.h"
#include "datalistclass.h"
#include "logo.h"
#include "ppt.h"

typedef enum em_Send_Data_Mode
{
    em_To_HID = 0x01,    //�����������豸
    em_To_Server,        //����������������
}EmSendDataMode;

// CtouchDlg �Ի���
class CtouchDlg : public CDialogEx
{
// ����
public:
	CtouchDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TOUCH_DIALOG };
	virtual BOOL PreTranslateMessage(MSG* pMsg);
// ʵ��
protected:
	// ���ɵ���Ϣӳ�亯��
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnNotifyIcon(WPARAM, LPARAM);
	//����Ĭ�ϵ���Ϣ������
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy( );
	
	afx_msg void OnExit();
	afx_msg void OnShow();
	
	afx_msg void OnBannerClose();
	afx_msg void OnBannerShow();
	afx_msg void OnClickMenuKeepWidthHeight();
	afx_msg void OnClickMenuAbout();
	afx_msg void OnBnClickedOk();
	void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnOpenHidDevSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnVolumeCtrlChanged( WPARAM wParam, LPARAM lParam );

	afx_msg LRESULT OnPCSleepMsg( WPARAM wParam, LPARAM lParam );   //PC���ߡ�˯����Ϣ����
	afx_msg LRESULT OnPCSleepResume( WPARAM wParam, LPARAM lParam );//PC���ߡ�˯�߻ָ���Ϣ����
	afx_msg LRESULT OnPCRebootClose( WPARAM wParam, LPARAM lParam );//PC�������ػ���Ϣ����

	afx_msg LRESULT OnPptPlay( WPARAM wParam, LPARAM lParam );      //ppt��Ϣ����
	afx_msg LRESULT OnPowerStatus( WPARAM wParam, LPARAM lParam );      //ppt��Ϣ����
	
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnMini();
	afx_msg void OnAbout();
	afx_msg void OnHide();
	void SetBannerAutoHide();
	
	void UdiskOut();

	void InitEncoderParam();
	void InitVideoEncoderParam(u8 byVideoType);
	void InitAudioEncoderParam(u8 byAudioMode, u16 wAudioDuration = 0);
	void OnStartScreenCatch();
	void OnStopScreenCatch();

	//��ȡ�豸���
	HANDLE GetHidHandle(HID_TYPE emType);
	THidDevice &GetHidDevie(HID_TYPE emType);

	CDataListClass* GetVideoDataList();
	CDataListClass* GetAudioDataList();

	void SendCmdToHid( int nCmd );//���������usb_hid�豸
	void SolveReadInfo(BYTE* readBuffer);//�����������Ϣ

	void StartAVThread();      //��ʼ����Ƶ�����߳�
	void StopAVThread();       //ֹͣ����Ƶ�����߳�

	void StartProjectScreen(); //��ʼͶ��
	void StopProjectScreen(bool bNotifyHid);  //ֹͣͶ��

	void SolveNetStatusNty(NET_STATUS emStatus);

	void StartTrayIconChange();//��ʼ����ͼ��仯
	void StopTrayIconChange(); //ֹͣ����ͼ��仯

	CEncoder &GetEncode();//��ȡ�������

	void SetLogo();    //����̨��
	void GetResolution(int &nWidth, int &nHeight);//��ȡҪ���б���ķֱ���
    void CheckResolution();  //��⵱ǰϵͳ�ֱ����Ƿ񳬳����Ʒֱ��ʣ�3840*2160��

	//Ĭ����Ƶ�豸����
	HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID);
	void InitDefaultAudioDevice();
	void UnInitDedaultAudioDevice();
	void CheckDedaultAudioDevice();

	//��������
	void InitVolumeCtrl();
	void UninitVolumeCtrl();

	void SolvePCNty();      //����ϵͳ˯�ߡ�����,�������رյ�֪ͨ

	bool GetSysUserName( CString &strName );  //��ȡ����ϵͳ�û���

    CString GetIniFilePath();  //ini�ļ�·��

	//ppt����
	void SendPptCmdToHid( EnumPptStatus emStatus );//����ppt��Ϣ��hid
	void SolvePptNty( PPT_STATUS emPptStatus );

	//���ʼǱ�����״̬
	void CheckBatteryStatus();

	//���ñ���Э�̲���
	void SetConsultVideoParam(u8 byVideoType);
    //�Ƿ����Э��
    bool NeedCodeConsult();

private:
	void InitUI();

    // ģ�������Ϣ
    void SetMouseMessage( WORD wMM,CPoint MousePosition,UINT nFlags,short zDelta );
    void SetMouseKB( INPUT & KeyBoardInput,WORD wVk,bool bDown );

	//��Ƶ����ת��Ƶģʽ
	void MediaTypeToAudioMode( u8 byMediatype, u8& byRatio );

public:
	CMainDlg  *m_pcMainDlg;
	CBannerDlg  *m_pcBannerDlg;
	CAboutDlg *m_pcVerDlg;

	bool m_bIsProjecting;      //�Ƿ�����Ͷ��
	bool m_bAVExit;            //�Ƿ��˳�����Ƶ�߳�
	bool m_bReadExit;          //�Ƿ��˳����߳�
	bool m_bPptExit;           //�Ƿ��˳�ppt�߳�

	bool m_bKeepWidthHeight;  //�Ƿ񱣳ֿ�߱�
	BOOL bFirstKeyFrame;      //�Ƿ��ǹؼ�֡
	HDEVNOTIFY m_hDevNotiy;   //�豸״̬�ı�֪ͨ
	bool m_bHidOpen;          //hid�豸�Ƿ��Ѿ���
	bool m_bSetDefAudoDev;    //�Ƿ�������Ĭ����Ƶ�豸
	LPWSTR m_wstrSysAudioID;  //ϵͳδ����Ͷ����ǰʹ�õ���Ƶ�豸

	CString m_strLogoPath;    //̨��·��

	bool m_bVideoThreadRun;   //�ж���Ƶ�߳��Ƿ�����
	bool m_bAudioThreadRun;   //�ж���Ƶ�߳��Ƿ�����

	bool m_bDataBlock;        //�Ƿ���������

	bool m_bIsSleep;          //�Ƿ��յ�ϵͳ˯�ߡ�������Ϣ
	bool m_bIsRebootClose;    //�Ƿ��յ�ϵͳ�������ر���Ϣ
	int m_nSendPCDisconnetCount;//����PC�Ͽ���Ϣ����

	CString m_strSysUserName; //ϵͳ�û���

	int m_nStartPptCount;      //����ppt�����������
	int m_nStopPptCount;       //����pptδ�����������
    int m_nSendBusinessCount;  //����ҵ���������
    int m_nThreadExitCount;    //����Ƶ�����߳��˳�����

	bool m_bCapOverEncode;   //�жϲɼ��ֱ����Ƿ���ڱ���ֱ���
	bool m_bStretch;         //�ж��Ƿ񲻵ȱ����죬���ɼ��ֱ��ʱ������ڵ���1.5ʱ
    bool m_bOverResLimit;    //�ж��Ƿ񳬳����Ʒֱ���

	bool m_bBusinessStaus;   //�Ƿ���Ҫ�ж�ҵ��״̬

	NET_STATUS m_bCurConnetStatus;    //��ǰ����״̬
    EmQKPidType m_emQKPidType;        //��ǰͶ��������
    EmSendDataMode m_emSendDateMode;  //����������ʽ

protected:
	HICON m_hIcon;
	NOTIFYICONDATA m_ntIcon;
	HICON m_hTrayIcon[4];//���ڱ仯��icon;

private:
	bool m_bMin;
	CEncoder m_cEncoder;
	TVideoEncParam  m_tVideoEncParam;  //�ն�Э�̵ı������
	BOOL     m_bNeedCodeConsult;       //�Ƿ�Э�̱������

	THidDevice m_HidDevice[HID_TYPE_NUM];
	CDataListClass m_cVideoDataList;
	CDataListClass m_cAudioDataList;
	CWinThread * m_pcVideoThread;
	CWinThread * m_pcAudioThread;
	CWinThread * m_pcReadThread;//��ȡ�����߳�
};