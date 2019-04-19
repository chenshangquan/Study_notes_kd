
// touchDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "touch.h"
#include "touchDlg.h"
#include "afxdialogex.h"
#include "winioctl.h"
#include "MenuEx.h"

#include "dynamicwnd.h"
#include "AboutDlg.h"

#include "epvolume.h"
#include "Mmdeviceapi.h"
#include "PolicyConfig.h"
#include "Propidl.h"
#include "Functiondiscoverykeys_devpkey.h"

#include "lm.h"
#include "language.h"
#include "socketmanager.h"

#pragma comment(lib,"netapi32.lib")

extern CtouchDlg * g_dlg;

//��������
EMLangID g_emLanType;
//���뷽ʽ
bool g_bHwEncStatus = false;  //�Ƿ�ΪӲ����״̬
bool g_bHwEncSupport = false; //�Ƿ�֧��Ӳ��Ӳ��
//������ӦĬ�������仯
HWND g_hVolumeCtrlDlg = NULL;
GUID g_guidMyContext = GUID_NULL;
CAudioEndpointVolumeCallback EPVolEvents;
IAudioEndpointVolume* g_pEndptVol = NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define g_nAutoHideBannerTimerID    2   //timer��id�������Զ�����Ͷ����ť
#define TrayIconChangeTimerID       3   //����ͼ��ı�timer��ID
#define OpenVideoHidDevTimerID      4   //����Ƶ��HID�豸��timer��ID
#define OpenAudioHidDevTimerID      5   //����Ƶ��HID�豸��timer��ID
#define OpenCmdHidDevTimerID        6   //�������HID�豸��timer��ID
#define SendVersionTimerID          7   //���Ͱ汾��ҵ���timer��ID
#define SendWidthHeightFrameTimerID 8   //���ͷ�����Ϣ��timer��ID
#define SendPCDisconnetTimerID      9   //������PC���ߡ�˯�ߺ͹ػ��������й���Ϣ��timer��ID
#define SendPptPlayTimerID          10  //����ppt������Ϣ��timer��ID
#define SendPptStopTimerID          11  //����pptֹͣ������Ϣ��timer��ID
#define SendBusinessTimerID         12  //ȷ��ҵ���Ƿ�û�йҵ���timer��ID
#define BatteryPowerTimerID         13  //���ʼǱ���ص�����timer��ID
#define ThreadExitTimerID           14  //����Ƶ�����߳��˳���timer��ID

//��Ƶ����ص� *pFrameΪȡ��������
void CapScreenVidStart( void *pFrame, u32 dwContext )
{
	g_dlg->m_bDataBlock = true;
    static u32 dwVFrameId = 0;
    //��������Ƶ����
    //u8 *pVideobuf = new u8[RECVVIDEOBUF_LENGTH];
    u8 achVideobuf[RECVVIDEOBUF_LENGTH] = {0};

	FRAMEHDR stFrameHdr = *(PFRAMEHDR)pFrame;//��õ������ǽṹ��FRAMEHDR

	if (stFrameHdr.m_tVideoParam.m_bKeyFrame)
	{
		if ( !g_dlg->bFirstKeyFrame )
		{
			PRINTMSG_TIME( "\r\nCapScreenVidStarts�ص�:�յ���һ���ؼ�֡\r\n");
            dwVFrameId = 0;
		}
		g_dlg->bFirstKeyFrame = TRUE;		
	}
	if ( g_dlg->bFirstKeyFrame == FALSE )
	{
		return;
	}

    stFrameHdr.m_dwFrameID = ++dwVFrameId;
    //stFrameHdr.m_dwSSRC = g_sdwVideoSsrc;  //ͬ��Դ�����ڽ��ն�

    memset( achVideobuf, 0, RECVVIDEOBUF_LENGTH );
    memcpy( achVideobuf, &stFrameHdr, STRUCT_FRAMEHDR_LENGTH );

    memcpy( achVideobuf+STRUCT_FRAMEHDR_LENGTH, stFrameHdr.m_pData, stFrameHdr.m_dwDataSize);

    //����������IMIX��

    /*nWriteNum = writen(g_QkclientSock,pVideobuf,(STRUCT_FRAMEHDR_LENGTH+tVideoFramHdr.m_dwDataSize));
    if(nWriteNum == -1)
    {
        usleep(1000*10);
        NVLOG::NvLogHint( MDL_NVQUICKSHARE, "[CNvDeviceUsbCtrl]RevPCMsgHandle video writen error \n");
        break;
    }*/

    if (1) //��ʼͶ��
    {
        SOCKETWORK->SendDataPack(&achVideobuf[0], STRUCT_FRAMEHDR_LENGTH+stFrameHdr.m_dwDataSize);
    }

	g_dlg->m_bDataBlock = false;
}

//��Ƶ����ص�
void CapScreenAudStart( void *pFrame, u32 dwContext )
{	
	FRAMEHDR stFrameHdr = *(PFRAMEHDR)pFrame;//��õ������ǽṹ��FRAMEHDR

	static u32 s_dwSeqID = 0;

	u32 dwFrameFdrSize = sizeof(FRAMEHDR);
	u32 dwTotolSize = dwFrameFdrSize + stFrameHdr.m_dwDataSize;

	//��Ƶ����

}

UINT ThreadSendViedoData(LPVOID pParam)
{
	CtouchDlg *pThis = (CtouchDlg*)pParam;
	if (!pThis)
	{
		return -1;
	}

	pThis->m_bVideoThreadRun = true;
	while (!pThis->m_bAVExit)
	{
        if (!pThis->GetVideoDataList()->IsEmpty())
        {
			BOOL bClearBuffer = FALSE; //������������Ҫ��ջ��壬��������ؼ�֡
			if ( pThis->GetVideoDataList()->Size() > MAX_BUFFER_NUM )
			{
				bClearBuffer = TRUE;
			}

            BYTE achWritetBuffer[LENTH_OUT_BUFFER];
            memset(achWritetBuffer, 0x00, sizeof(achWritetBuffer));
            u16 wOffset = 0;
			
			if ( CPrint::IsPrintVideo() )
			{
				SYSTEMTIME st;
				GetLocalTime(&st);
				PRINTMSG( "\r\nVIDEO:TIME:%02d:%02d.%03d,HID_SendDataToDevice:\r\n", st.wMinute,st.wSecond,st.wMilliseconds );
			}

            while( !pThis->GetVideoDataList()->IsEmpty() )
            {
                TUSBData tData = pThis->GetVideoDataList()->GetDataFront();			
				//��ӡ������Ƶ��Ϣ
				if ( CPrint::IsPrintVideo() )
				{		
					PRINTMSG("m_dwSeqID:%d,m_wSliceID:%d,m_wSliceNum:%d,m_byDataSize:%d\r\n",
						tData.m_dwSeqID,tData.m_wSliceID,tData.m_wSliceNum,tData.m_byDataSize);
				}

                achWritetBuffer[0+wOffset] = SEND_TYPE_VIDEO;
                memcpy( achWritetBuffer+wOffset+1, (BYTE*)&tData, sizeof(TUSBData) );

				//�����������������һ֡���ݺ����buffer��֮������ؼ�֡
				if ( bClearBuffer && tData.m_wSliceID == (tData.m_wSliceNum-1) )
				{
					PRINTMSG( "Clear DataList, AskKeyFrm\r\n" );
					pThis->GetVideoDataList()->Clear();
					pThis->GetEncode().SetAskKeyFrm();
					break;
				}

                pThis->GetVideoDataList()->DeletDataFront();

                wOffset += PACKET_SIZE;
                if ( wOffset >= LENTH_OUT_BUFFER )
                {
                    break;
                }
            }

            DWORD dwRet = HID_SendDataToDevice(&pThis->GetHidDevie(HID_TYPE_VIDEO), achWritetBuffer, DATA_TYPE_VIDEO);
			if ( dwRet == 2 )
			{
				//������2�������豸�γ�δ��⵽  Ҳ������1167
				bool bRst = HID_IsDeviceConnected( &pThis->GetHidDevie(HID_TYPE_VIDEO) );
				if ( !bRst )
				{
					PRINTMSG( "SendDataToDevice Failed, Device is not connected\r\n" );
					pThis->UdiskOut();
				}
			}
        }
		else
		{
			Sleep(1);
		}
	}

	pThis->m_bVideoThreadRun = false;
	PRINTMSG( "\r\nThreadSendViedoData����\r\n");
	return 0;
}

UINT ThreadSendAudioData(LPVOID pParam)
{
	CtouchDlg *pThis = (CtouchDlg*)pParam;
	if (!pThis)
	{
		return -1;
	}

	pThis->m_bAudioThreadRun = true;
	while (!pThis->m_bAVExit)
	{
		if (!pThis->GetAudioDataList()->IsEmpty())
		{
			BYTE achWritetBuffer[LENTH_OUT_BUFFER];
			memset(achWritetBuffer, 0x00, sizeof(achWritetBuffer));
			u16 wOffset = 0;

			if ( CPrint::IsPrintAudio() )
			{
				SYSTEMTIME st;
				GetLocalTime(&st);
				PRINTMSG( "\r\nAUDIO:TIME:%02d:%02d.%03d,HID_SendDataToDevice:\r\n", st.wMinute,st.wSecond,st.wMilliseconds );
			}

			while (!pThis->GetAudioDataList()->IsEmpty())
			{
				TUSBData tData = pThis->GetAudioDataList()->GetDataFront();

				//��ӡ������Ƶ��Ϣ
				if ( CPrint::IsPrintAudio() )
				{		
					PRINTMSG("m_dwSeqID:%d,m_wSliceID:%d,m_wSliceNum:%d,m_byDataSize:%d\r\n",
						tData.m_dwSeqID,tData.m_wSliceID,tData.m_wSliceNum,tData.m_byDataSize);
				}

				achWritetBuffer[0+wOffset] = SEND_TYPE_AUDIO;
				memcpy( achWritetBuffer+wOffset+1, (BYTE*)&tData, sizeof(TUSBData) );

				pThis->GetAudioDataList()->DeletDataFront();

				wOffset += PACKET_SIZE;
				if ( wOffset >= LENTH_OUT_BUFFER )
				{
					break;
				}
			}

			bool bRst = HID_SendDataToDevice(&pThis->GetHidDevie(HID_TYPE_AUDIO), achWritetBuffer, DATA_TYPE_AUDIO);
		}
		else
		{
			Sleep(2);
		}
	}

	pThis->m_bAudioThreadRun = false;
	PRINTMSG( "\r\nThreadSendAudioData����\r\n");
	return 0;
}

UINT ThreadRead(LPVOID pParam)
{
	CtouchDlg *pThis = (CtouchDlg*)pParam;
	if (!pThis)
	{
		return -1;
	}

	BYTE recvDataBuf[LENTH_IN_BUFFER_CMD] = {0};

	while (!pThis->m_bReadExit)
	{
		HANDLE hidHandle = pThis->GetHidHandle(HID_TYPE_CMD);
		if (hidHandle)
		{
			memset(recvDataBuf, 0, LENTH_IN_BUFFER_CMD);
			bool bRst = HID_RecvData4Device(&pThis->GetHidDevie(HID_TYPE_CMD), recvDataBuf, LENTH_IN_BUFFER_CMD-1, DATA_TYPE_CMD);

			if (bRst)
			{
				pThis->SolveReadInfo(recvDataBuf);
			}
			else
			{
				Sleep(1);
				continue;
			}
		}
	}

	PRINTMSG( "\r\nThreadRead����\r\n");
	return 0;
}

UINT ThreadPpt(LPVOID pParam)
{
	CtouchDlg *pThis = (CtouchDlg*)pParam;
	if (!pThis)
	{
		return -1;
	}

	static bool bStatusChange = false;//�����ж�״̬�Ƿ�ı�

	bool bPlaying = false;
	while (!pThis->m_bPptExit)
	{
		//��Ͷ��״̬�ָ���־λ
		if (!pThis->m_bIsProjecting)
		{
			bStatusChange = false;
			Sleep(1000);
			continue;
		}

		bPlaying = CPpt::IsPptPlaying();
		if ( bPlaying && pThis->bFirstKeyFrame )
		{
			if ( !bStatusChange )
			{
				SendMessage(pThis->GetSafeHwnd(), WM_PPT_PLAY_MSG, (WPARAM)EM_PPT_PLAYING, 0);//PPT���ڲ���
				bStatusChange = true;
			}
		}
		else
		{
			if ( bStatusChange )
			{
				SendMessage(pThis->GetSafeHwnd(), WM_PPT_PLAY_MSG, (WPARAM)EM_PPT_NO_PLAY, 0);//PPTδ����
				bStatusChange = false;
			}	
		}

		Sleep(1000);
	}

	PRINTMSG( "\r\nThreadPpt����\r\n");
	return 0;
}

//�ɼ��ֱ��ʻص�
void VideoCapStdCB(u32 dwWidht, u32 dwHeight, u32 dwContext)
{
	TVideoEncParam tVideoEncParam;
	memset(&tVideoEncParam, 0, sizeof(TVideoEncParam));
	g_dlg->GetEncode().GetVideoEncParam(tVideoEncParam);
	PRINTMSG("VideoCapStdCB:dwWidht:%d,dwHeight:%d,m_wEncVideoWidth:%d,m_wEncVideoHeight:%d\r\n",
		dwWidht, dwHeight, tVideoEncParam.m_wEncVideoWidth,tVideoEncParam.m_wEncVideoHeight);

    if ( g_dlg->NeedCodeConsult() )
    {
        //��ʱ����֡����Ϣֱ���յ��ظ�
        KillTimer(g_dlg->GetSafeHwnd(), SendWidthHeightFrameTimerID);
        SetTimer(g_dlg->GetSafeHwnd(), SendWidthHeightFrameTimerID, 500, NULL);
        return;
    }

    //�ɼ��ֱ��������ֱ���һ��
    if (dwWidht == tVideoEncParam.m_wEncVideoWidth 
        && dwHeight == tVideoEncParam.m_wEncVideoHeight)
    {
        return;
    }

    g_dlg->m_bOverResLimit = false;
    g_dlg->m_bStretch = false;
    g_dlg->m_bCapOverEncode = false;

	//����  --�������֧������1080p
	if ( dwWidht > 1920 && dwHeight > 1080)
	{
        // �������Ʒֱ���Ͷ��
        if ( dwWidht > 3840 || dwHeight > 2160 )
        {
            g_dlg->m_bOverResLimit = true;
        }

        // ��������ֱ�����ɼ���һ�²���
        if ( float(dwWidht)/float(dwHeight) >= 1.5 )    // �ɼ��ֱ��ʱ������ڵ���1.5 --���ȱ�ȫ��
        {
            g_dlg->m_bStretch = true;
            g_dlg->GetEncode().SetVidDecZoomPolicy( EN_ZOOM_SCALE );
            PRINTMSG("���ȱ�ȫ��\r\n");
        }
        else
        {
            g_dlg->GetEncode().SetVidDecZoomPolicy( EN_ZOOM_FILLBLACK );
            PRINTMSG("�ȱȼӺڱ�\r\n");
        }

		dwWidht = 1920;
		dwHeight = 1080;
		g_dlg->m_bCapOverEncode = true;
	}

    if ( !g_dlg->NeedCodeConsult() )
    {
        tVideoEncParam.m_wEncVideoWidth = dwWidht;
        tVideoEncParam.m_wEncVideoHeight = dwHeight;
        g_dlg->GetEncode().SetVideoEncParam(tVideoEncParam);
    }

	//��ʱ����֡����Ϣֱ���յ��ظ�
	KillTimer(g_dlg->GetSafeHwnd(), SendWidthHeightFrameTimerID);
	SetTimer(g_dlg->GetSafeHwnd(), SendWidthHeightFrameTimerID, 500, NULL);
}

// CtouchDlg �Ի���

CtouchDlg::CtouchDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtouchDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pcMainDlg = NULL;
	m_pcBannerDlg = NULL;
	m_pcVerDlg = NULL;
	
	m_bMin = true;

	m_pcVideoThread = NULL;
	m_pcAudioThread = NULL;
	m_bAVExit = false;

	m_bIsProjecting = false;

	m_pcReadThread = NULL;
	m_bReadExit = false;

	m_bKeepWidthHeight = true;

	bFirstKeyFrame = FALSE;
	m_bHidOpen = false;
	m_bSetDefAudoDev = false;
	m_wstrSysAudioID = NULL;

	m_bVideoThreadRun = false;
	m_bAudioThreadRun = false;

	m_bDataBlock = false;

	m_bIsSleep = false;
	m_bIsRebootClose = false;

	m_bPptExit = false;

	m_bCapOverEncode = false;
	m_bStretch = false;
    m_bOverResLimit = false;

	m_bBusinessStaus = true;
	m_bNeedCodeConsult = FALSE;

    m_nStartPptCount = 0;
    m_nStopPptCount = 0;
    m_nThreadExitCount = 0;
    m_nSendBusinessCount = 0;

    m_emQKPidType = em_NT30_Type;
    m_emSendDateMode = em_To_HID;

	m_bCurConnetStatus = NET_STATUS_CONNECTING;
}

void CtouchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtouchDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_32771, &CtouchDlg::OnAbout)
	ON_COMMAND(ID_EXIT, &CtouchDlg::OnExit)
	ON_COMMAND(ID_SHOW, &CtouchDlg::OnShow)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CtouchDlg::OnBnClickedOk)
	ON_MESSAGE(WM_TRAY_NOTIFYICON, &CtouchDlg::OnNotifyIcon)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_COMMAND(ID_KEEP_WIDTH_HEIGHT, &CtouchDlg::OnClickMenuKeepWidthHeight)
	ON_COMMAND(ID_ABOUT, &CtouchDlg::OnClickMenuAbout)
	ON_MESSAGE(WM_OPENHIDDEV_SUCCESS, &CtouchDlg::OnOpenHidDevSuccess)
	ON_MESSAGE(WM_VOLUMECTRL_CHANGE, &CtouchDlg::OnVolumeCtrlChanged)
	ON_MESSAGE(WM_PCSLEEP_MSG, &CtouchDlg::OnPCSleepMsg)
	ON_MESSAGE(WM_PCSLEEPRESUME_MSG, &CtouchDlg::OnPCSleepResume)
	ON_MESSAGE(WM_PCREBOOTCLOSE_MSG, &CtouchDlg::OnPCRebootClose)
	ON_MESSAGE(WM_PPT_PLAY_MSG, &CtouchDlg::OnPptPlay)
	ON_MESSAGE(WM_POWER_STATUS_MSG, &CtouchDlg::OnPowerStatus)
END_MESSAGE_MAP()


// CtouchDlg ��Ϣ�������
/*
UINT ThreadAddVideoData(LPVOID pParam)
{
	while (1)
	{
		FRAMEHDR stFrameHdr;
		memset(&stFrameHdr, 0, sizeof(FRAMEHDR));
		u8 byData[10000];
		memset(byData, 1, sizeof(byData));
		stFrameHdr.m_pData = byData;
		stFrameHdr.m_dwDataSize = 10000;
		stFrameHdr.m_byFrameRate = 30;
		stFrameHdr.m_tVideoParam.m_bKeyFrame = 0;
		stFrameHdr.m_tVideoParam.m_wVideoWidth = 1600;
		stFrameHdr.m_tVideoParam.m_wVideoHeight = 900;
		stFrameHdr.m_tVideoParam.m_bHighProfile = 1;

		static u32 s_dwSeqID = 0;

		u32 dwFrameFdrSize = sizeof(FRAMEHDR);
		u32 dwTotolSize = dwFrameFdrSize + stFrameHdr.m_dwDataSize;

		if ( dwTotolSize > PC_SENDDATA_LENGTH )
		{
			u16 wTotolNum = 0;
			if (dwTotolSize % PC_SENDDATA_LENGTH != 0)
			{
				wTotolNum = dwTotolSize / PC_SENDDATA_LENGTH + 1;
			}
			else
			{
				wTotolNum = dwTotolSize / PC_SENDDATA_LENGTH;
			}

			int nOffset = 0; //Dataƫ����

			for ( int i = 0; i < wTotolNum; i++ )
			{
				TUSBData tUsbData;
				memset(&tUsbData, 0, sizeof(TUSBData));

				tUsbData.m_dwSeqID = s_dwSeqID++;
				tUsbData.m_wSliceID = i;
				tUsbData.m_wSliceNum = wTotolNum;

				if ( i == 0 )
				{
					tUsbData.m_byDataSize = PC_SENDDATA_LENGTH;
					memcpy( tUsbData.m_abyData, &stFrameHdr, sizeof(FRAMEHDR) );
					memcpy( tUsbData.m_abyData + sizeof(FRAMEHDR), stFrameHdr.m_pData, PC_SENDDATA_LENGTH - dwFrameFdrSize );
					nOffset =  PC_SENDDATA_LENGTH - dwFrameFdrSize;
				}
				else
				{
					u32 dwCuntSize = 0; //��ǰ�����ʹ�С
					if ( nOffset + PC_SENDDATA_LENGTH > stFrameHdr.m_dwDataSize )
					{
						dwCuntSize = stFrameHdr.m_dwDataSize - nOffset;
					}
					else
					{
						dwCuntSize = PC_SENDDATA_LENGTH;				
					}
					tUsbData.m_byDataSize = dwCuntSize;
					memcpy( tUsbData.m_abyData, stFrameHdr.m_pData + nOffset, dwCuntSize );

					nOffset += dwCuntSize;
				}

				g_dlg->GetVideoDataList()->AddDataTail(tUsbData);
			}

		}
		else
		{
			TUSBData tUsbData;;
			memset(&tUsbData, 0, sizeof(TUSBData));
			tUsbData.m_dwSeqID = s_dwSeqID++;
			tUsbData.m_wSliceID = 0;
			tUsbData.m_wSliceNum = 1;
			tUsbData.m_byDataSize = dwTotolSize;
			memcpy( tUsbData.m_abyData, &stFrameHdr, sizeof(FRAMEHDR) );
			memcpy( tUsbData.m_abyData + sizeof(FRAMEHDR), stFrameHdr.m_pData, stFrameHdr.m_dwDataSize );

			g_dlg->GetVideoDataList()->AddDataTail(tUsbData);
		}

		Sleep(1000);
	}
	
	return 0;
}

UINT ThreadAddAudioData(LPVOID pParam)
{
	while(1)
	{
		FRAMEHDR stFrameHdr;
		memset(&stFrameHdr, 0, sizeof(FRAMEHDR));
		u8 byData[300];
		memset(byData, 1, sizeof(byData));
		stFrameHdr.m_pData = byData;
		stFrameHdr.m_dwDataSize = 300;
		stFrameHdr.m_byFrameRate = 30;
		stFrameHdr.m_tAudioParam.m_bIncludeAudioLevel = 1;
		stFrameHdr.m_tAudioParam.m_bV = 1;
		stFrameHdr.m_tAudioParam.m_byAudioLevel = 3;
		stFrameHdr.m_tAudioParam.m_byAudioMode = 4;

		static u32 s_dwSeqID = 0;

		u32 dwFrameFdrSize = sizeof(FRAMEHDR);
		u32 dwTotolSize = dwFrameFdrSize + stFrameHdr.m_dwDataSize;

		if ( dwTotolSize > PC_SENDDATA_LENGTH )
		{
			u16 wTotolNum = 0;
			if (dwTotolSize % PC_SENDDATA_LENGTH != 0)
			{
				wTotolNum = dwTotolSize / PC_SENDDATA_LENGTH + 1;
			}
			else
			{
				wTotolNum = dwTotolSize / PC_SENDDATA_LENGTH;
			}

			int nOffset = 0; //Dataƫ����

			for ( int i = 0; i < wTotolNum; i++ )
			{
				TUSBData tUsbData;
				memset(&tUsbData, 0, sizeof(TUSBData));

				tUsbData.m_dwSeqID = s_dwSeqID++;
				tUsbData.m_wSliceID = i;
				tUsbData.m_wSliceNum = wTotolNum;

				if ( i == 0 )
				{
					tUsbData.m_byDataSize = PC_SENDDATA_LENGTH;
					memcpy( tUsbData.m_abyData, &stFrameHdr, sizeof(FRAMEHDR) );
					memcpy( tUsbData.m_abyData + sizeof(FRAMEHDR), stFrameHdr.m_pData, PC_SENDDATA_LENGTH - dwFrameFdrSize );
					nOffset =  PC_SENDDATA_LENGTH - dwFrameFdrSize;
				}
				else
				{
					u32 dwCuntSize = 0; //��ǰ�����ʹ�С
					if ( nOffset + PC_SENDDATA_LENGTH > stFrameHdr.m_dwDataSize )
					{
						dwCuntSize = stFrameHdr.m_dwDataSize - nOffset;
					}
					else
					{
						dwCuntSize = PC_SENDDATA_LENGTH;				
					}
					tUsbData.m_byDataSize = dwCuntSize;
					memcpy( tUsbData.m_abyData, stFrameHdr.m_pData + nOffset, dwCuntSize );

					nOffset += dwCuntSize;
				}


				g_dlg->GetAudioDataList()->AddDataTail(tUsbData);
			}
		}
		else
		{
			TUSBData tUsbData;;
			memset(&tUsbData, 0, sizeof(TUSBData));
			tUsbData.m_dwSeqID = s_dwSeqID++;
			tUsbData.m_wSliceID = 0;
			tUsbData.m_wSliceNum = 1;
			tUsbData.m_byDataSize = dwTotolSize;
			memcpy( tUsbData.m_abyData, &stFrameHdr, sizeof(FRAMEHDR) );
			memcpy( tUsbData.m_abyData + sizeof(FRAMEHDR), stFrameHdr.m_pData, stFrameHdr.m_dwDataSize );

			g_dlg->GetAudioDataList()->AddDataTail(tUsbData);
		}
		Sleep(500);
	}
	
	return 0;
}*/

BOOL CtouchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	//���ô��ڱ���
	SetWindowText(_T(DEF_TELNET_NAME));

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
/*
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_ntIcon.cbSize = sizeof(NOTIFYICONDATA);                            //�ýṹ������Ĵ�С
	m_ntIcon.hIcon = m_hIcon;  //ͼ�꣬ͨ����ԴID�õ�
	m_ntIcon.hWnd = this->m_hWnd;                                                 //��������ͼ��֪ͨ��Ϣ�Ĵ��ھ��
	TCHAR atip[128] = _T("NexPresence");                                                     //���������ʱ��ʾ����ʾ
	StrNCpy(m_ntIcon.szTip, atip, 128);  

	m_ntIcon.uCallbackMessage = WM_TRAY_NOTIFYICON;                //Ӧ�ó��������ϢID��
	m_ntIcon.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;               //ͼ������ԣ����ó�ԱuCallbackMessage��hIcon��szTip��Ч
	::Shell_NotifyIconW(NIM_ADD, &m_ntIcon);                                 //��ϵͳ֪ͨ�����������ͼ��

	//��ʼ�����ڱ仯������ͼ��
	m_hTrayIcon[0] = AfxGetApp()->LoadIcon(IDI_TRAYICON_DOWN);
    m_hTrayIcon[1] = AfxGetApp()->LoadIcon(IDI_TRAYICON_MIDDLE_DOWN);
	m_hTrayIcon[2] = AfxGetApp()->LoadIcon(IDI_TRAYICON_MIDDLE_UP);
	m_hTrayIcon[3] = AfxGetApp()->LoadIcon(IDI_TRAYICON_UP);
*/
    //����temp�ļ���
    m_strLogoPath = CLogo::GetModuleFullPath() + TP_TEMPFILE_PATH;
    if(!PathFileExists(m_strLogoPath))
    {
        CreateDirectory(m_strLogoPath, NULL);
    }

    //�汾��Ϣ
    s8 achVersion[MAX_VER_LEN] = {0};
    CAboutDlg::GetBuildVersion(achVersion);
    SYSTEMTIME st;
    GetLocalTime(&st);
    PRINTMSG("\r\nVersion:%s, Run Time:%d-%d-%d %d:%d:%d\r\n",
        achVersion, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	//��ȡϵͳ�û���
	if ( !GetSysUserName(m_strSysUserName) )
	{
		s8 achUserName[USERNAME_MAX_LENGTH] = {0};
		strncpy( achUserName, (CT2A)(LPCTSTR)m_strSysUserName, USERNAME_MAX_LENGTH );
		PRINTMSG("\r\n��ȡϵͳ�û���ʧ��!, m_strSysUserName:%s \r\n", achUserName);
		//::MessageBox(NULL, _T("��ȡϵͳ�û���ʧ��!"), NULL, MB_OK|MB_ICONERROR );
		//return FALSE;
	}

    // ��ȡ��Ӳ��״̬
    TKdvEncStatus tKdvEncStatus;
    memset(&tKdvEncStatus,0,sizeof(TKdvEncStatus));
    m_cEncoder.GetEncoderStatus(tKdvEncStatus);  //��ȡ������״̬
    if (tKdvEncStatus.m_emHwStatus == en_SupportedAndOpened)
    {
        //֧��Ӳ��Ӳ�⣬��ǰΪӲ����״̬
        g_bHwEncSupport = true;
        g_bHwEncStatus = true;
        PRINTMSG("--֧��Ӳ��Ӳ�⣬��ǰΪӲ����״̬\r\n");
    }
    else
    {
        //��֧��Ӳ��Ӳ�⣬��ǰΪ�����״̬
        g_bHwEncSupport = false;
        g_bHwEncStatus = false;
        PRINTMSG("--��֧��Ӳ��Ӳ�⣬��ǰΪ�����״̬\r\n");
    }

    //���ý�����������
    CString strIniFilePath = GetIniFilePath();
    if (!strIniFilePath.IsEmpty())
    {
        TCHAR strbEng[MAX_NAME_LEN] = {0};
        GetPrivateProfileString(_T("UILangInfo"), _T("Language"),
            _T("AUTO"), strbEng, MAX_NAME_LEN-1, strIniFilePath);
        if ( lstrcmp(strbEng, _T("AUTO")) == 0 )
        {
            // ��ȡϵͳ�������ͣ������ý�����������
            LANGID langID = GetSystemDefaultLangID();
            if (langID == 0x0804)
            {
                g_emLanType = enumLangIdCHN;
            }
            else
            {
                g_emLanType = enumLangIdENG;
            }
        }
        else if (lstrcmp(strbEng, _T("ENG")) == 0)
        {
            g_emLanType = enumLangIdENG;
        }
        else
        {
            g_emLanType = enumLangIdCHN;
        }
    }

    // ��ȡ�������䷽ʽ
    if (!strIniFilePath.IsEmpty())
    {
        TCHAR strbSendDataMode[MAX_NAME_LEN] = {0};
        GetPrivateProfileString(_T("SendDateMode"), _T("Mode"),
            _T("ToHID"), strbSendDataMode, MAX_NAME_LEN-1, strIniFilePath);
        if ( lstrcmp(strbSendDataMode, _T("ToHID")) == 0 )
        {
            m_emSendDateMode = em_To_HID;
            PRINTMSG("--����������Ͷ���� -->\r\n");
        }
        else if (lstrcmp(strbSendDataMode, _T("ToServer")) == 0)
        { 
            m_emSendDateMode = em_To_Server;
            PRINTMSG("--���������������� -->\r\n");
        }
        else
        {
            m_emSendDateMode = em_To_HID;
            PRINTMSG("--����������Ͷ���� -->\r\n");
        }
    }

	InitUI();

    if (m_emSendDateMode == em_To_HID)
    {
        //��Ƶ��hid�豸ͬ����
        if (!m_HidDevice[HID_TYPE_VIDEO].hndHidDevice)
        {
            m_bHidOpen = HID_OpenDevice(&m_HidDevice[HID_TYPE_VIDEO], HID_MODE_SYNC, HID_TYPE_VIDEO); //���豸����ʹ���ص����첽����ʽ;
            if (!m_bHidOpen)
            {
                SetTimer(OpenVideoHidDevTimerID, 500, NULL);
                return TRUE;
            }
        }

        //��Ƶ��hid�豸ͬ����
        if (!m_HidDevice[HID_TYPE_AUDIO].hndHidDevice)
        {
            m_bHidOpen = HID_OpenDevice(&m_HidDevice[HID_TYPE_AUDIO], HID_MODE_SYNC, HID_TYPE_AUDIO); //���豸����ʹ���ص����첽����ʽ;
            if (!m_bHidOpen)
            {
                SetTimer(OpenAudioHidDevTimerID, 500, NULL);
                return TRUE;
            }
        }

        //�����hid�첽��
        if (!m_HidDevice[HID_TYPE_CMD].hndHidDevice)
        {
            m_bHidOpen = HID_OpenDevice(&m_HidDevice[HID_TYPE_CMD], HID_MODE_ASYNC, HID_TYPE_CMD); //���豸����ʹ���ص����첽����ʽ;
            if (!m_bHidOpen)
            {
                SetTimer(OpenCmdHidDevTimerID, 500, NULL);
                return TRUE;
            }
        }
    }

	//���ص�hidͬ����
	//if (!m_HidDevice[HID_TYPE_RC].hndHidDevice)
	//{
	//	m_bHidOpen = HID_OpenDevice(&m_HidDevice[HID_TYPE_RC], HID_MODE_SYNC, HID_TYPE_RC); //���豸����ʹ���ص����첽����ʽ;
	//	if (!m_bHidOpen)
	//	{
	//		//�ر��Ѿ��򿪵�
	//		HID_CloseDevice(&m_HidDevice[HID_TYPE_AV]);
	//		HID_CloseDevice(&m_HidDevice[HID_TYPE_CMD]);

	//		CString strError;
	//		strError.Format(_T("��ȡ�豸���ʧ�ܣ�Hid:%d"), HID_TYPE_RC);
	//		MessageBox(strError, NULL, MB_OK|MB_ICONERROR );
	//		return TRUE;
	//	}
	//}

	OnOpenHidDevSuccess(0, 0);

	//ceshi
	//AfxBeginThread(ThreadAddVideoData, this);
	//AfxBeginThread(ThreadAddAudioData, this);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

CString CtouchDlg::GetIniFilePath()
{
    CString strIniFilePath = CLogo::GetModuleFullPath() + _T("\\touch.ini");
    if (!PathFileExists(strIniFilePath))
    {
        PRINTMSG("\r\n��ȡ�����ļ�ʧ��!\r\n");
        strIniFilePath.Empty();
    }

    return strIniFilePath;
}

BOOL CtouchDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if ( VK_RETURN == pMsg->wParam
			|| VK_SPACE == pMsg->wParam
			|| VK_ESCAPE == pMsg->wParam)
		{
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CtouchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
// 		CAboutDlg dlgAbout;
// 		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CtouchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialogEx::OnPaint();
		/*CPaintDC dc(this);

		CBrush cBrush;
		cBrush.CreateSolidBrush( RGB( 0, 0, 0 ) );

		CRect cRect;
		GetClientRect( &cRect );
		dc.FillRect( &cRect, &cBrush );

		cBrush.DeleteObject();*/
	}
}

void CtouchDlg::OnClose()
{
	m_bMin = true;
	::Shell_NotifyIconW(NIM_DELETE, &m_ntIcon);
	CDialogEx::OnClose();
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CtouchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtouchDlg::OnAbout()
{
	// TODO: �ڴ���������������
	if (m_pcVerDlg)
	{
		delete m_pcVerDlg;
		m_pcVerDlg = NULL;
	}
	
	if (m_pcVerDlg == NULL)
	{
		m_pcVerDlg = new CAboutDlg();
		m_pcVerDlg->Create( CAboutDlg::IDD, this );
		m_pcVerDlg->ShowWindow( SW_SHOW );
		m_pcVerDlg->UpdateWindow();
	}
}

void CtouchDlg::OnExit()
{
	m_bMin = true;
	::Shell_NotifyIconW(NIM_DELETE, &m_ntIcon);
	CDialogEx::OnCancel();
}

void CtouchDlg::OnShow()
{
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);
	m_pcMainDlg->ShowWindow(SW_SHOW);
}

void CtouchDlg::OnHide()
{
	m_pcMainDlg->ShowWindow( SW_HIDE );
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
}

void CtouchDlg::OnMini()
{
	OnHide();
}

void CtouchDlg::OnBannerShow()
{
	OnHide();
	if (m_pcBannerDlg)
	{
		m_pcBannerDlg->ShowWindow( SW_SHOW );
		SetBannerAutoHide();
	}
}

void CtouchDlg::SetBannerAutoHide()
{
	SetTimer(g_nAutoHideBannerTimerID,5000,NULL); //5s�Զ�����
}

void CtouchDlg::OnBannerClose()
{
	KillTimer(g_nAutoHideBannerTimerID);
	m_pcBannerDlg->StopAutoHide();
	m_pcBannerDlg->ShowWindow(SW_HIDE);
	
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	m_pcMainDlg->ShowWindow( SW_SHOW );
}

void CtouchDlg::OnTimer(UINT_PTR nIDEvent)   
{   
	// TODO: Add your message handler code here and/or call default   
	static int nCount = 0;//�������̸���

	switch (nIDEvent)   
	{   
	case g_nAutoHideBannerTimerID:   
		{
			KillTimer(g_nAutoHideBannerTimerID);
			m_pcBannerDlg->DoHide();
		}
		break;
	case TrayIconChangeTimerID:
		{
			if (m_hTrayIcon[nCount] != NULL)
			{
				m_ntIcon.hIcon = m_hTrayIcon[nCount];  //ͼ�꣬ͨ����ԴID�õ�
				::Shell_NotifyIconW(NIM_MODIFY, &m_ntIcon);      
				nCount++;
				if ( 4 == nCount )
				{
					nCount = 0;
				}
			}
		}
		break;
	case OpenVideoHidDevTimerID:
		{
			KillTimer(OpenVideoHidDevTimerID);
			//��Ƶ��hid�豸ͬ����
			if (!m_HidDevice[HID_TYPE_VIDEO].hndHidDevice)
			{
				m_bHidOpen = HID_OpenDevice(&m_HidDevice[HID_TYPE_VIDEO], HID_MODE_SYNC, HID_TYPE_VIDEO); //���豸����ʹ���ص����첽����ʽ;
				if (!m_bHidOpen)
				{
					SetTimer(OpenVideoHidDevTimerID, 500, NULL);
					break;
				}
			}

			//��Ƶ��hid�豸ͬ����
			if (!m_HidDevice[HID_TYPE_AUDIO].hndHidDevice)
			{
				m_bHidOpen = HID_OpenDevice(&m_HidDevice[HID_TYPE_AUDIO], HID_MODE_SYNC, HID_TYPE_AUDIO); //���豸����ʹ���ص����첽����ʽ;
				if (!m_bHidOpen)
				{
					SetTimer(OpenAudioHidDevTimerID, 500, NULL);
					break;
				}
			}

			//�����hid�첽��
			if (!m_HidDevice[HID_TYPE_CMD].hndHidDevice)
			{
				m_bHidOpen = HID_OpenDevice(&m_HidDevice[HID_TYPE_CMD], HID_MODE_ASYNC, HID_TYPE_CMD); //���豸����ʹ���ص����첽����ʽ;
				if (!m_bHidOpen)
				{
					SetTimer(OpenCmdHidDevTimerID, 500, NULL);
					break;
				}
			}

			SendMessage(WM_OPENHIDDEV_SUCCESS, 0, 0);
		}
		break;
	case OpenAudioHidDevTimerID:
		{
			KillTimer(OpenAudioHidDevTimerID);
			//��Ƶ��hid�豸ͬ����
			if (!m_HidDevice[HID_TYPE_AUDIO].hndHidDevice)
			{
				m_bHidOpen = HID_OpenDevice(&m_HidDevice[HID_TYPE_AUDIO], HID_MODE_SYNC, HID_TYPE_AUDIO); //���豸����ʹ���ص����첽����ʽ;
				if (!m_bHidOpen)
				{
					SetTimer(OpenAudioHidDevTimerID, 500, NULL);
					break;
				}
			}

			//�����hid�첽��
			if (!m_HidDevice[HID_TYPE_CMD].hndHidDevice)
			{
				m_bHidOpen = HID_OpenDevice(&m_HidDevice[HID_TYPE_CMD], HID_MODE_ASYNC, HID_TYPE_CMD); //���豸����ʹ���ص����첽����ʽ;
				if (!m_bHidOpen)
				{
					SetTimer(OpenCmdHidDevTimerID, 500, NULL);
					break;
				}
			}

			SendMessage(WM_OPENHIDDEV_SUCCESS, 0, 0);
		}
		break;
	case OpenCmdHidDevTimerID:
		{
			KillTimer(OpenCmdHidDevTimerID);
			//�����hid�첽��
			if (!m_HidDevice[HID_TYPE_CMD].hndHidDevice)
			{
				m_bHidOpen = HID_OpenDevice(&m_HidDevice[HID_TYPE_CMD], HID_MODE_ASYNC, HID_TYPE_CMD); //���豸����ʹ���ص����첽����ʽ;
				if (!m_bHidOpen)
				{
					SetTimer(OpenCmdHidDevTimerID, 500, NULL);
					break;
				}
			}

			SendMessage(WM_OPENHIDDEV_SUCCESS, 0, 0);
		}
		break;
	case SendVersionTimerID:
		{
			//SendCmdToHid(Ev_NV_TerminalType_PC_Cmd);
		}
		break;
	case SendWidthHeightFrameTimerID:
		{
			SendCmdToHid(Ev_NV_WidthHeightFrame_Cmd);
		}
		break;
	case SendPCDisconnetTimerID:
		{
			//1����û�յ��ظ���ֱ�ӶϿ�
			if (5 == m_nSendPCDisconnetCount)
			{
				PRINTMSG("\r\nû���յ�Ev_NV_PCDisconnet_Ntf\r\n");
				KillTimer(SendPCDisconnetTimerID);
				SolvePCNty();
				m_nSendPCDisconnetCount = 0;
			}
			else
			{
				SendCmdToHid(Ev_NV_PCDisconnet_Cmd);
				m_nSendPCDisconnetCount++;
			}
		}
		break;
	case SendPptPlayTimerID:
		{
			if ( 5 == m_nStartPptCount )
			{
				KillTimer(SendPptPlayTimerID);
				PRINTMSG("\r\nû���յ�PPT_STATUS_START_NTF\r\n");

				m_nStartPptCount = 0; 
			}
			else
			{
				SendPptCmdToHid( EM_PPT_PLAYING );
				m_nStartPptCount++;
			}
		}
		break;
	case SendPptStopTimerID:
		{
			if ( 5 == m_nStopPptCount )
			{
				KillTimer(SendPptStopTimerID);
				PRINTMSG("\r\nû���յ�PPT_STATUS_STOP_NTF\r\n");

				m_nStopPptCount = 0;
			}
			else
			{
				SendPptCmdToHid( EM_PPT_NO_PLAY );
				m_nStopPptCount++;
			}
		}
		break;
	case SendBusinessTimerID:
		{
			if ( 4 == m_nSendBusinessCount )
			{
				KillTimer(SendBusinessTimerID);
				PRINTMSG("\r\nû���յ�ҵ�����ҵ��ҵ�\r\n");

				StopProjectScreen(false);
				if ( m_pcMainDlg != NULL )
				{
					m_pcMainDlg->ShowConnectPicture( CONNECT_BUSINESS_FAIL );
				}

				m_nSendBusinessCount = 0;
				m_bBusinessStaus = true;
			}
			else
			{
				SendCmdToHid( Ev_NV_PCSendBusiness_Cmd );
				m_nSendBusinessCount++;
			}
		}
		break;
	case BatteryPowerTimerID:
		{
			if ( m_bIsProjecting )
			{
				CheckBatteryStatus();
			}		
		}
		break;
    case ThreadExitTimerID:
        {
            if ( 4 == m_nThreadExitCount )
            {
                KillTimer(ThreadExitTimerID);
                PRINTMSG_TIME("AVThread Exit Failed, m_bVideoThreadRun:%d, m_bAudioThreadRun:%d\r\n",
                    m_bVideoThreadRun, m_bAudioThreadRun);
                m_nThreadExitCount = 0;
            }
            else
            {
                if (m_bVideoThreadRun == false && m_bAudioThreadRun == false)
                {
                    KillTimer(ThreadExitTimerID);
                    //�������
                    GetVideoDataList()->Clear();
                    GetAudioDataList()->Clear();
                }
                else
                {
                    m_nThreadExitCount++;
                }
            }

        }
        break;
	default:   
		break;   
	}

	CDialogEx::OnTimer(nIDEvent);   
}  

void CtouchDlg::InitUI()
{
	SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOMOVE );
	m_pcMainDlg = new CMainDlg();
	m_pcMainDlg->Create( CMainDlg::IDD, this );
	m_pcMainDlg->ShowWindow( SW_SHOW );
	m_pcMainDlg->UpdateWindow();

	m_pcBannerDlg = new CBannerDlg();
	m_pcBannerDlg->Create( CBannerDlg::IDD, this );
}

void CtouchDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

LRESULT CtouchDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message)
	{
	case WM_DEVICECHANGE:
		{
			switch (wParam)
			{
				//case DBT_DEVICEARRIVAL:// Handle device arrival
				//	{
				//
				//	}
				//	break;

			case DBT_DEVICEREMOVECOMPLETE:// Handle device removal
				{
					if (!m_hDevNotiy)
					{
						break;
					}
					for (int i=HID_TYPE_VIDEO; i<HID_TYPE_NUM; i++)
					{
						if (m_HidDevice[i].hndHidDevice)
						{
							bool bRst = HID_IsDeviceConnected(&m_HidDevice[i]);
							if (!bRst)
							{
								PRINTMSG_TIME("\r\nHID�豸�γ�\r\n");
								UdiskOut();
								break;
							}
						}
					}
				}

				break;
			}
		}
		break;
	case WM_POWERBROADCAST:
		{
			switch(wParam)
			{
			case PBT_APMSUSPEND:         //˯�ߡ�����
				{
					SendMessage(WM_PCSLEEP_MSG, 0, 0);
				}
				break;
			case PBT_APMRESUMEAUTOMATIC: //˯�ߡ����߻ָ�
				{
					SendMessage(WM_PCSLEEPRESUME_MSG, 0, 0);
				}
				break;
			case PBT_APMPOWERSTATUSCHANGE://��Դ״̬
				{
					SendMessage(WM_POWER_STATUS_MSG, 0, 0);
				}
				break;
			}
		}
		break;
	case WM_QUERYENDSESSION:            //�ػ���������ע�� --˵��:DefWindowProc����Ӧ��������Ϣ 
		{
			SendMessage(WM_PCREBOOTCLOSE_MSG, 0, 0);
		}
		break;
    case WM_DISPLAYCHANGE:
        {
            //Ͷ��������δͶ��
            if (m_bHidOpen && !m_bIsProjecting)
            {
                CheckResolution();
                if ( !m_bOverResLimit )
                {
                    //��ʾ�����ӻ���
                    m_pcMainDlg->ShowConnectStatus(NET_STATUS_CONNECTED);
                }
            } 
        }
        break;
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}

void CtouchDlg::UdiskOut()
{	
	//��������������˳�
	SendMessage(WM_DESTROY, 0, 0);

	//��ֹ����
	TerminateProcess(GetCurrentProcess(), 0); 
}

LRESULT CtouchDlg::OnNotifyIcon(WPARAM wparam, LPARAM lparam)
{
	if(lparam == WM_LBUTTONDOWN)
	{
		if (!m_bIsProjecting)
		{
			OnShow();
		}
		//����ò��ֻ��д����������ܱ�֤�ָ����ں󣬸ô��ڴ��ڻ״̬������ǰ�棩
	}
	else if(lparam == WM_RBUTTONDOWN)
	{
		CMenuEx popMenu;
		popMenu.CreatePopupMenu();

		popMenu.SetItemHeight(30);
		popMenu.SetFontInfo(18, "΢���ź�");
		popMenu.InstallHook(theApp.m_hInstance);

		popMenu.AppendCheckItem(MF_STRING, ID_KEEP_WIDTH_HEIGHT, _T("���ֿ�߱�"), _T(""), IDI_MENU_CHECK_NORMAL, IDI_MENU_CHECK_HOVER);
		popMenu.AppendItem(MF_STRING, ID_ABOUT, _T("����"), _T(""), 0);
		
		if (m_bKeepWidthHeight)
		{
			popMenu.CheckMenuItem(ID_KEEP_WIDTH_HEIGHT, MF_CHECKED);
		}
		else
		{
			popMenu.CheckMenuItem(ID_KEEP_WIDTH_HEIGHT, MF_UNCHECKED);
		}
		
		
		CPoint pos;
		GetCursorPos(&pos);            //�����˵���λ�ã�����������ĵ�ǰλ��
		SetForegroundWindow();//����þ�Ŀ�������Ҽ��˵�ʧȥ����ʱ���Զ���ʧ
		//��ʾ�ò˵�����һ������������ֵ�ֱ��ʾ�������ұ���ʾ����Ӧ����һ�
		popMenu.TrackPopupMenu(TPM_RIGHTALIGN|TPM_RIGHTBUTTON, pos.x, pos.y, AfxGetMainWnd(), 0);

		popMenu.DestroyMenu();
		popMenu.UnInstallHook();
	}
	return 0;
}

void CtouchDlg::OnSize(UINT nType, int cx, int cy)
{
	if (nType == SIZE_MINIMIZED)
	{
		m_bMin = true;
	}
	else
	{
		m_bMin = false;
	}
}

void CtouchDlg::OnDestroy()
{
	//Ͷ���аε���ֹͣͶ��
	StopProjectScreen(false);
	
	if (!m_bReadExit && m_pcReadThread)
	{
		m_bReadExit = true;
		/*DWORD dwWaitStatus = WaitForSingleObject(m_pcReadThread->m_hThread, 1000);
		if ( WAIT_TIMEOUT == dwWaitStatus)
		{
		TerminateThread(m_pcReadThread->m_hThread, 0);
		}

		m_pcReadThread = NULL;*/
	}

	//�˳�ppt�߳�
	if (!m_bPptExit)
	{
		m_bPptExit = true;
	}
	//��ע��֪ͨ����
	if (m_hDevNotiy)
	{
		HID_UnRegisterDeviceNotification(&m_hDevNotiy);
		m_hDevNotiy = NULL;
	}
	
	for (int i=HID_TYPE_VIDEO; i<HID_TYPE_NUM; i++)
	{
		if (m_HidDevice[i].hndHidDevice)
		{
			HID_CloseDevice(&m_HidDevice[i]);
		}
	}

	m_bHidOpen = false;

	KillTimer( BatteryPowerTimerID );
	
	if (m_pcBannerDlg)
	{
		m_pcBannerDlg->DestroyWindow();
		m_pcBannerDlg = NULL;
	}

	if (m_pcVerDlg)
	{
		m_pcVerDlg->DestroyWindow();
		m_pcVerDlg = NULL;
	}

	if (m_pcMainDlg)
	{
		m_pcMainDlg->DestroyWindow();
		m_pcMainDlg = NULL;
	}

	//�ر�COM
	CoUninitialize();

	CDialogEx::OnDestroy();
}

//����ץȡ��ʼ��
void CtouchDlg::InitEncoderParam()
{
	InitAudioEncoderParam(AUDIO_MODE_BEST);
	
	m_cEncoder.SetDeskSharedVidCallback(&CapScreenVidStart, 0);
	m_cEncoder.SetDeskSharedAudCallback(&CapScreenAudStart, 0);
	m_cEncoder.SetVideoCapStdCB(&VideoCapStdCB, 0);
}

void CtouchDlg::InitVideoEncoderParam(u8 byVideoType)
{
	TVideoEncParam tVideoEncParam;
	memset(&tVideoEncParam, 0, sizeof(TVideoEncParam));

	//ý������ ����\include\KdvDef.h��
	switch(byVideoType)
	{
	case MEDIA_TYPE_H261:
		tVideoEncParam.m_byMinQuant = 1;
		tVideoEncParam.m_byMaxQuant = 31;
		tVideoEncParam.m_byMaxKeyFrameInterval = 30;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 25;
		tVideoEncParam.m_wEncVideoWidth = 352;
		tVideoEncParam.m_wEncVideoHeight = 288;
		tVideoEncParam.m_wBitRate = 768;
		break;
	case MEDIA_TYPE_H263:
		tVideoEncParam.m_byMinQuant = 1;
		tVideoEncParam.m_byMaxQuant = 31;
		tVideoEncParam.m_byMaxKeyFrameInterval = 300;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 25;
		tVideoEncParam.m_wEncVideoWidth = 352;
		tVideoEncParam.m_wEncVideoHeight = 288;
		tVideoEncParam.m_wBitRate = 768;
		break;
	case MEDIA_TYPE_H263PLUS:
		tVideoEncParam.m_byMinQuant = 1;
		tVideoEncParam.m_byMaxQuant = 31;
		tVideoEncParam.m_byMaxKeyFrameInterval = 300;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 5;
		tVideoEncParam.m_wEncVideoWidth = 1024;
		tVideoEncParam.m_wEncVideoHeight = 768;
		tVideoEncParam.m_wBitRate = 1024;
		break;
	case MEDIA_TYPE_H264:
		tVideoEncParam.m_byMinQuant = 10;
		tVideoEncParam.m_byMaxQuant = 45;
		tVideoEncParam.m_byMaxKeyFrameInterval = 3000;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 30;
		tVideoEncParam.m_wEncVideoWidth = 1920;
		tVideoEncParam.m_wEncVideoHeight = 1080;
        // ���������Ϊ2M��Ӳ����Ϊ����ͬ�Ȼ��ʣ���������ʣ��ݶ�Ϊ4M
        if (g_bHwEncStatus)
        {
            tVideoEncParam.m_wBitRate = /*1536*/4096;
            tVideoEncParam.m_wMaxBitRate = 10240;
            tVideoEncParam.m_wMinBitRate = 4096;
        }
        else
        {
            tVideoEncParam.m_wBitRate = /*1536*/2048;
            tVideoEncParam.m_wMaxBitRate = 2048;
            tVideoEncParam.m_wMinBitRate = 2048;
        }
		tVideoEncParam.m_byEncLevel = 1;//1-bp,3-HP
		break;
	case MEDIA_TYPE_H262:
		tVideoEncParam.m_byMinQuant = 5;
		tVideoEncParam.m_byMaxQuant = 31;
		tVideoEncParam.m_byMaxKeyFrameInterval = 75;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 25;
		tVideoEncParam.m_wEncVideoWidth = 352;
		tVideoEncParam.m_wEncVideoHeight = 288;
		tVideoEncParam.m_wBitRate = 768;
		break;
	case MEDIA_TYPE_MP4:
		tVideoEncParam.m_byMinQuant = 2;
		tVideoEncParam.m_byMaxQuant = 31;
		tVideoEncParam.m_byMaxKeyFrameInterval = 300;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 25;
		tVideoEncParam.m_wEncVideoWidth = 352;
		tVideoEncParam.m_wEncVideoHeight = 288;
		tVideoEncParam.m_wBitRate = 768;
		break;
	case MEDIA_TYPE_H265:
		tVideoEncParam.m_byMinQuant = 20;
		tVideoEncParam.m_byMaxQuant = 45;
		tVideoEncParam.m_byMaxKeyFrameInterval = 3000;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 25;
		tVideoEncParam.m_wEncVideoWidth = 1280;
		tVideoEncParam.m_wEncVideoHeight = 720;
		tVideoEncParam.m_wBitRate = 1536;
		break;
	}


	if (1/*!m_bKeepWidthHeight*/)
	{
		int nWidth = 0;
		int nHeight = 0;
		GetResolution(nWidth, nHeight);

		tVideoEncParam.m_wEncVideoWidth = nWidth;
		tVideoEncParam.m_wEncVideoHeight = nHeight;
	}

    m_cEncoder.SetVideoEncParam(tVideoEncParam);

	if ( m_bCapOverEncode )
	{
        // �ɼ��ֱ��ʱ������ڵ���1.5ʱ�����ȱ�ȫ������֮���ȱȼӺڱ�
        if (m_bStretch)
        {
            m_cEncoder.SetVidDecZoomPolicy( EN_ZOOM_SCALE );
            PRINTMSG("���ȱ�ȫ��\r\n");
        }
        else
        {
            m_cEncoder.SetVidDecZoomPolicy( EN_ZOOM_FILLBLACK );
            PRINTMSG("�ȱȼӺڱ�\r\n");
        }
	}

	//���ʼǱ�����״̬
	CheckBatteryStatus();
}

void CtouchDlg::InitAudioEncoderParam(u8 byAudioMode, u16 wAudioDuration)
{
	m_cEncoder.SetAudioEncParam(byAudioMode,wAudioDuration);
}

void CtouchDlg::OnStartScreenCatch()
{
	m_cEncoder.StartEnc();
}

void CtouchDlg::OnStopScreenCatch()
{
	m_cEncoder.StopEnc();
}

void CtouchDlg::OnClickMenuKeepWidthHeight()
{
	m_bKeepWidthHeight = !m_bKeepWidthHeight;

	//���ͱ��ֿ�߱�����
	SendCmdToHid(Ev_NV_KeepWidthHeight_Cmd);
}

void CtouchDlg::OnClickMenuAbout()
{
	OnAbout();
}

CDataListClass* CtouchDlg::GetVideoDataList()
{
	return &m_cVideoDataList;
}

CDataListClass* CtouchDlg::GetAudioDataList()
{
	return &m_cAudioDataList;
}

void CtouchDlg::StartAVThread()
{
	m_bAVExit = false;
	
	m_pcVideoThread = AfxBeginThread(ThreadSendViedoData, this, THREAD_PRIORITY_TIME_CRITICAL);
	m_pcAudioThread = AfxBeginThread(ThreadSendAudioData, this, THREAD_PRIORITY_TIME_CRITICAL);
}

void CtouchDlg::StopAVThread()
{
	if (!m_bAVExit && m_bVideoThreadRun && m_bAudioThreadRun)
	{
		m_bAVExit = true;

		//DWORD dwWaitStatus = WaitForSingleObject(m_pcVideoThread->m_hThread, 500);
		//if ( WAIT_TIMEOUT == dwWaitStatus)
		//{
		//	TerminateThread(m_pcVideoThread->m_hThread, 0);
		//}

		//dwWaitStatus = WaitForSingleObject(m_pcAudioThread->m_hThread, 500);
		//if ( WAIT_TIMEOUT == dwWaitStatus)
		//{
		//	TerminateThread(m_pcAudioThread->m_hThread, 0);
		//}

		//m_pcVideoThread = NULL;
		//m_pcAudioThread = NULL;
	}
}

void CtouchDlg::SolveReadInfo(BYTE* recvDataBuf)
{
	if (!recvDataBuf)
	{
		return;
	}

	BYTE byCmdType = recvDataBuf[0];
	switch (byCmdType)
	{
	case Ev_NV_StartProjecting_Ntf:
		{
			PRINTMSG_TIME("���߳�--�յ���ʼͶ������\r\n");
			StartProjectScreen();
			
			if ( m_pcMainDlg != NULL )
			{
				m_pcMainDlg->HideUpgradeControlUI();
			}
		}
		break;
	case Ev_NV_ImaxStopPic_Ntf:
		{
			PRINTMSG_TIME("���߳�--�յ�ֹͣͶ������\r\n");
			StopProjectScreen(false);
		}
		break;
	case Ev_NV_NetConnectStatus_Ntf:
		{
			//�յ��������ӵĻ���Ҫ���û�������Ϣ�����������ʾ�����ӻ���
			BYTE byNetStatus = recvDataBuf[1];
			PRINTMSG("���߳�--�յ�����״̬����,byNetStatus:%d\r\n", byNetStatus);
			if (byNetStatus != NET_STATUS_CONNECTED)
			{
				if (!m_bIsProjecting)
				{
					OnShow();
				}
				
				SolveNetStatusNty((NET_STATUS)byNetStatus);
			}

			m_bCurConnetStatus = (NET_STATUS)byNetStatus;
		}
		break;
	case Ev_NV_REQUSTPCNAME_Ntf:
		{
			//����PC�û���
			SendCmdToHid(Ev_NV_UserInfo_Cmd);
		}
		break;
	case Ev_NV_UserInfo_Nty:
		{
			PRINTMSG("���߳�--�յ��û����ɹ�����֪ͨ\r\n");
		
			//��ʱ����֡����Ϣֱ���յ��ظ�
			KillTimer(SendWidthHeightFrameTimerID);
			SetTimer(SendWidthHeightFrameTimerID, 100, NULL);
		}
		break;
	case Ev_NV_ImaxRequstKeyFrame_Ntf:
		{
			PRINTMSG("���߳�--�յ�����ؼ�֡����\r\n");
			GetEncode().SetAskKeyFrm();
		}
		break;
	case Ev_NV_RequestTerminalType_Ntf:
		{
			PRINTMSG("���߳�--�յ�����Զ����ͻظ�\r\n");
			KillTimer(SendVersionTimerID);
		}
		break;
	case Ev_NV_WidthHeightFrame_Ntf:
		{
			PRINTMSG("���߳�--�յ����ػظ���ϢEv_NV_WidthHeightFrame_Ntf\r\n");
			KillTimer(SendWidthHeightFrameTimerID);

            //�������Ʒֱ���Ͷ����Ͷ��ʧ��
            if (m_bOverResLimit)
            {
                if (m_bIsProjecting)
                {
                    if (m_pcMainDlg)
                    {
                        m_pcMainDlg->ShowConnectPicture(CONNECT_OVER_RESOLUTION_LIMIT);
                    }
                    StopProjectScreen(true);
                    PRINTMSG("�������Ʒֱ���Ͷ����Ͷ��ʧ��\r\n");
                }
                else
                {
                    //��ʾ�����ӻ���
                    SolveNetStatusNty(NET_STATUS_CONNECTED);
                }
            }
            else
            {
                //��ʾ�����ӻ���
                SolveNetStatusNty(NET_STATUS_CONNECTED);
            }
		}
		break;
	case Ev_NV_PCDisconnet_Ntf:
		{
			PRINTMSG("���߳�--�յ���ϢEv_NV_PCDisconnet_Ntf\r\n");
			m_nSendPCDisconnetCount = 0;
			
			KillTimer(SendPCDisconnetTimerID);
			SolvePCNty();
		}
		break;
	case Ev_NV_SendPCHeatMsg_Ntf:
		{
			static u32 nCount = 0;
			nCount++;
			//PRINTMSG_NOLOG("nCount:%d,���߳�--�յ�����Ev_NV_SendPCHeatMsg_Ntf\r\n",nCount);
			Sleep(100);
			//������������
			SendCmdToHid(Ev_NV_SendPCHeatMsg_Cmd);
		}
		break;
	case Ev_NV_BroadCastPPT_Ntf:
		{
			PPT_STATUS emPptStatus = (PPT_STATUS)recvDataBuf[1];
			SolvePptNty( emPptStatus );
		}
		break;
	case Ev_NV_PCSendBusiness_Ntf:
		{
			PRINTMSG("\r\n�յ�Ev_NV_PCSendBusiness_Ntf\r\n");
			KillTimer(SendBusinessTimerID);
			
			m_nSendBusinessCount = 0;
			m_bBusinessStaus = true;
		}
		break;
	case Ev_NV_UpgradeQuickShare_Ntf:
		{
			PRINTMSG("���߳�--�յ�������ϢEv_NV_UpgradeQuickShare_Ntf\r\n");
			if ( m_pcMainDlg != NULL )
			{
				m_pcMainDlg->ShowUpgradeControl( true );
			}
		}
		break;
	/*case Ev_NV_SendPCTerName_Ntf:
		{
			BYTE achName[32] = {0};
			memcpy( achName, &recvDataBuf[1], 30);
			//PRINTMSG("���߳�--�յ�����״̬����,byNetStatus:%d\r\n", byNetStatus);

		}
		break;*/
	case Ev_Nv_NeedCodeConsult_Ntf:
		{
			PRINTMSG("���߳�--�յ���ϢEv_Nv_NeedCodeConsult_Ntf\r\n");
			m_bNeedCodeConsult = TRUE;
		}
		break;
	case Ev_Nv_CodeFormat_Cmd:              /*֪ͨPC��������Ƶ��������*/
		{
			u8 byVideoFormat = recvDataBuf[1];
			u8 byAudioFormat = recvDataBuf[2];

			PRINTMSG("���߳�--�յ���ϢEv_Nv_CodeFormat_Cmd, VideoFormat:%d, AudioFormat:%d \r\n", byVideoFormat, byAudioFormat);

			SetConsultVideoParam(byVideoFormat);

			u8 byAudioMode = 0;
			MediaTypeToAudioMode( byAudioFormat, byAudioMode );
			InitAudioEncoderParam(byAudioMode);
		}
		break;
	case Ev_Nv_CodeResolution_Cmd:         /*֪ͨPC�������ֱ���*/
		{
			EmCodeResType emType = (EmCodeResType)recvDataBuf[1];

			PRINTMSG("���߳�--�յ���ϢEv_Nv_CodeResolution_Cmd, emType:%d\r\n", emType);
			switch( emType )
			{
			case em_RES_TYPE_1080P1920x1080:
				m_tVideoEncParam.m_wEncVideoWidth = 1920;
				m_tVideoEncParam.m_wEncVideoHeight = 1080;
				break;
			case em_RES_TYPE_WSXGA1440x900:
				m_tVideoEncParam.m_wEncVideoWidth = 1440;
				m_tVideoEncParam.m_wEncVideoHeight = 900;
				break;
			case em_RES_TYPE_SXGA1280x1024:
				m_tVideoEncParam.m_wEncVideoWidth = 1280;
				m_tVideoEncParam.m_wEncVideoHeight = 1024;
				break;
			case em_RES_TYPE_1280x960:
				m_tVideoEncParam.m_wEncVideoWidth = 1280;
				m_tVideoEncParam.m_wEncVideoHeight = 960;
				break;
			case em_RES_TYPE_WXGA1366x768:
				m_tVideoEncParam.m_wEncVideoWidth = 1366;
				m_tVideoEncParam.m_wEncVideoHeight = 768;
				break;
			case em_RES_TYPE_WXGA1360x768:
				m_tVideoEncParam.m_wEncVideoWidth = 1360;
				m_tVideoEncParam.m_wEncVideoHeight = 768;
				break;
			case em_RES_TYPE_WXGA1280x800:
				m_tVideoEncParam.m_wEncVideoWidth = 1280;
				m_tVideoEncParam.m_wEncVideoHeight = 800;
				break;
			case em_RES_TYPE_WXGA1280x768:
				m_tVideoEncParam.m_wEncVideoWidth = 1280;
				m_tVideoEncParam.m_wEncVideoHeight = 768;
				break;
			case em_RES_TYPE_720P1280x720:
				m_tVideoEncParam.m_wEncVideoWidth = 1280;
				m_tVideoEncParam.m_wEncVideoHeight = 720;
				break;
			case em_RES_TYPE_1280x600:
				m_tVideoEncParam.m_wEncVideoWidth = 1280;
				m_tVideoEncParam.m_wEncVideoHeight = 600;
				break;
			case em_RES_TYPE_XGA1024x768:
				m_tVideoEncParam.m_wEncVideoWidth = 1024;
				m_tVideoEncParam.m_wEncVideoHeight = 768;
				break;
			case em_RES_TYPE_SVGA800x600:
				m_tVideoEncParam.m_wEncVideoWidth = 800;
				m_tVideoEncParam.m_wEncVideoHeight = 600;
				break;
            case em_RES_TYPE_CIF352x288:
                m_tVideoEncParam.m_wEncVideoWidth = 352;
                m_tVideoEncParam.m_wEncVideoHeight = 288;
                break;
			}			
		}
		break;
	case Ev_Nv_CodeFrameRate_Cmd:          /*֪ͨPC�������֡��*/
		{
			u8 byFrameRate = recvDataBuf[1];

			PRINTMSG("���߳�--�յ���ϢEv_Nv_CodeFrameRate_Cmd, byFrameRate:%d\r\n", byFrameRate);
			m_tVideoEncParam.m_byFrameRate = byFrameRate;
		}
		break;
	case Ev_Nv_CodeRate_Cmd:               /*֪ͨPC���������*/
		{
			u8 byRateHigh = recvDataBuf[1];
			u8 byRateLow = recvDataBuf[2];
			u16 wBitRate = byRateHigh;
			wBitRate = wBitRate << 8;
			wBitRate = wBitRate | byRateLow;
			m_tVideoEncParam.m_wBitRate = wBitRate;

            // H264 BitRate < 3M ʱ�����ú㶨���ʿ���cbr
            if ( m_tVideoEncParam.m_byEncType == MEDIA_TYPE_H264 && wBitRate < 3072 )
            {
                m_tVideoEncParam.m_wMaxBitRate = wBitRate;
                m_tVideoEncParam.m_wMinBitRate = wBitRate;
            }
            else
            {
                m_tVideoEncParam.m_wMaxBitRate = 10240;
                m_tVideoEncParam.m_wMinBitRate = 2048;
            }

			PRINTMSG("���߳�--�յ���ϢEv_Nv_CodeRate_Cmd, wBitRate:%d\r\n", wBitRate);
		}
		break;
    case Ev_Nv_QKPidType_Cmd:              /*֪ͨPC���浱ǰͶ��������*/
        {
            m_emQKPidType = (EmQKPidType)recvDataBuf[1];
            PRINTMSG("���߳�--�յ���ϢEv_Nv_QKPidType_Cmd, m_emQKPidType:%d\r\n", m_emQKPidType);
        }
        break;
    case Ev_Nv_QKNotView_Cmd:
        {
            EmQkNotViewReason emQkNotViewReason = (EmQkNotViewReason)recvDataBuf[1];
            PRINTMSG("���߳�--�յ���ϢEv_Nv_QKNotView_Cmd, emQkNotViewReason:%d\r\n", emQkNotViewReason);

            //�����ն˻����в�֧��Ͷ����˫��
            if (emQkNotViewReason == em_MtEncryptConf_Reason)
            {
                PRINTMSG("�����ն˲�֧��Ͷ��\r\n");
                m_pcMainDlg->ShowConnectPicture(CONNECT_NT30_MT_NONSUPPORT);
            }
        }
        break;
	default:
		break;
	}
}

void CtouchDlg::SendCmdToHid( int nCmd )
{
	switch (nCmd)
	{
	case Ev_NV_StopProjecting_Cmd:
		{
			BYTE acBuf[LENTH_OUT_BUFFER_CMD] = {0};
			acBuf[0] = Ev_NV_StopProjecting_Cmd; //
			bool bRst = HID_SendData2Device(&m_HidDevice[HID_TYPE_CMD], acBuf, LENTH_OUT_BUFFER_CMD, DATA_TYPE_CMD);
			PRINTMSG("\r\n����ֹͣͶ������\r\n");
		}
		break;
	case Ev_NV_UserInfo_Cmd:
		{
			TUserInfo tUserInfo;

			CString strName;
			if ( m_strSysUserName.GetLength() >= USERNAME_MAX_LENGTH/sizeof(WCHAR) )
			{
				strName = m_strSysUserName.Left(USERNAME_MAX_LENGTH/sizeof(WCHAR) - 1);
			}
			else
			{
				strName = m_strSysUserName;
			}

			//strcpy_s(tUserInfo.abyUserName, USERNAME_MAX_LENGTH, CW2A(strName));
            lstrcpy((WCHAR *)tUserInfo.abyUserName, strName);  //����Unicode���뷢�ͣ�������ַ���ʾ��������

			BYTE acBuf[LENTH_OUT_BUFFER_CMD] = {0};
			acBuf[0] = Ev_NV_UserInfo_Cmd; //
			memcpy(&acBuf[1], &tUserInfo, sizeof(TUserInfo));
			bool bRst = HID_SendData2Device(&m_HidDevice[HID_TYPE_CMD], acBuf, LENTH_OUT_BUFFER_CMD, DATA_TYPE_CMD);
            PRINTMSG("\r\n�����û�������,achUserName:%s\r\n",CW2A(strName));
		}
		break;
	case Ev_NV_KeepWidthHeight_Cmd:
		{
			BYTE acBuf[LENTH_OUT_BUFFER_CMD] = {0};
			acBuf[0] = Ev_NV_KeepWidthHeight_Cmd; //
			acBuf[1] = m_bKeepWidthHeight ? 1:0; //
			bool bRst = HID_SendData2Device(&m_HidDevice[HID_TYPE_CMD], acBuf, LENTH_OUT_BUFFER_CMD, DATA_TYPE_CMD);
			PRINTMSG("\r\n���ͱ��ֿ�߱�����,m_bKeepWidthHeight:%d\r\n",m_bKeepWidthHeight);
		}
		break;
	case Ev_NV_WidthHeightFrame_Cmd:
		{
			int nWidth = 0;
			int nHeight = 0;
			GetResolution(nWidth, nHeight);

			TFrameInfo tFrameInfo;
			memset(&tFrameInfo, 0, sizeof(TFrameInfo));
            if ( m_bNeedCodeConsult )
            {
                tFrameInfo.m_dwPCWidth = m_tVideoEncParam.m_wEncVideoWidth;
                tFrameInfo.m_dwPCHeight = m_tVideoEncParam.m_wEncVideoHeight;
                tFrameInfo.m_dwFrame = m_tVideoEncParam.m_byFrameRate;
                tFrameInfo.m_dwBitRate = m_tVideoEncParam.m_wBitRate;
            }
            else
            {
                tFrameInfo.m_dwPCWidth = nWidth;
                tFrameInfo.m_dwPCHeight = nHeight;
                tFrameInfo.m_dwFrame = 30;
                tFrameInfo.m_dwBitRate = 2048;
            }

			BYTE acBuf[LENTH_OUT_BUFFER_CMD] = {0};
			acBuf[0] = Ev_NV_WidthHeightFrame_Cmd; 
			memcpy(&acBuf[1], &tFrameInfo, sizeof(TFrameInfo));
			
			bool bRst = HID_SendData2Device(&m_HidDevice[HID_TYPE_CMD], acBuf, LENTH_OUT_BUFFER_CMD, DATA_TYPE_CMD);
			PRINTMSG("\r\n���ͷ�����Ҫ��Ϣ,m_dwPCWidth:%d,m_dwPCHeight:%d,m_wFrame:%d,m_dwBitRate:%d\r\n",
				tFrameInfo.m_dwPCWidth, tFrameInfo.m_dwPCHeight, tFrameInfo.m_dwFrame, tFrameInfo.m_dwBitRate);
		}
		break;
	case Ev_NV_TerminalType_PC_Cmd:
		{
			BYTE acBuf[LENTH_OUT_BUFFER_CMD] = {0};
			acBuf[0] = Ev_NV_TerminalType_PC_Cmd; //

			SYSTEMTIME st;
			GetLocalTime(&st);
			TTimeInfo tTimeInfo;
			tTimeInfo.m_dwYear = st.wYear;
			tTimeInfo.m_dwMonth = st.wMonth;
			tTimeInfo.m_dwDay = st.wDay;
			tTimeInfo.m_dwHour = st.wHour;
			tTimeInfo.m_dwMinute = st.wMinute;
			tTimeInfo.m_dwSecond = st.wSecond;
			memcpy(&acBuf[1], &tTimeInfo, sizeof(TTimeInfo));

			bool bRst = HID_SendData2Device(&m_HidDevice[HID_TYPE_CMD], acBuf, LENTH_OUT_BUFFER_CMD, DATA_TYPE_CMD);
			PRINTMSG("\r\n����PC����-Ev_NV_TerminalType_PC_Cmd,%d-%d-%d,%d:%d:%d\r\n", tTimeInfo.m_dwYear, tTimeInfo.m_dwMonth, tTimeInfo.m_dwDay,
				tTimeInfo.m_dwHour, tTimeInfo.m_dwMinute, tTimeInfo.m_dwSecond);
		}
		break;
	case Ev_NV_PCDisconnet_Cmd:
		{
			BYTE acBuf[LENTH_OUT_BUFFER_CMD] = {0};
			acBuf[0] = Ev_NV_PCDisconnet_Cmd; //
			bool bRst = HID_SendData2Device(&m_HidDevice[HID_TYPE_CMD], acBuf, LENTH_OUT_BUFFER_CMD, DATA_TYPE_CMD);
			PRINTMSG("\r\n����Ev_NV_PCDisconnet_Cmd����\r\n");
		}
		break;
    case Ev_NV_SendPCHeatMsg_Cmd:
		{
			BYTE acBuf[LENTH_OUT_BUFFER_CMD] = {0};
			acBuf[0] = Ev_NV_SendPCHeatMsg_Cmd; //
			bool bRst = HID_SendData2Device(&m_HidDevice[HID_TYPE_CMD], acBuf, LENTH_OUT_BUFFER_CMD, DATA_TYPE_CMD);
			//PRINTMSG_NOLOG("\r\n����Ev_NV_SendPCHeatMsg_Cmd����\r\n");
		}
		break;
	case Ev_NV_PCSendBusiness_Cmd:
		{
			BYTE acBuf[LENTH_OUT_BUFFER_CMD] = {0};
			acBuf[0] = Ev_NV_PCSendBusiness_Cmd; //
			bool bRst = HID_SendData2Device(&m_HidDevice[HID_TYPE_CMD], acBuf, LENTH_OUT_BUFFER_CMD, DATA_TYPE_CMD);
			PRINTMSG("\r\n����Ev_NV_PCSendBusiness_Cmd����\r\n");
		}
		break;
	}
}

void CtouchDlg::StartProjectScreen()
{
	if (!m_bHidOpen)
	{
		return;
	}

	if ( m_bVideoThreadRun || m_bAudioThreadRun )
	{
		PRINTMSG_TIME("m_bVideoThreadRun:%d,m_bAudioThreadRun:%d\r\n", m_bVideoThreadRun, m_bAudioThreadRun);
		SendCmdToHid(Ev_NV_StopProjecting_Cmd);
		return;
	}

	if (!m_bIsProjecting)
	{
        //�������Ʒֱ��ʣ�3840*2160����Ͷ��ʧ��
        if (m_bOverResLimit)
        {
            PRINTMSG("�������Ʒֱ���Ͷ����Ͷ��ʧ��\r\n");
            m_pcMainDlg->ShowConnectPicture(CONNECT_OVER_RESOLUTION_LIMIT);
            Sleep(1000);  //��������
            SendCmdToHid(Ev_NV_StopProjecting_Cmd);
            return;
        }

		//����Ĭ����Ƶ�豸
		InitDefaultAudioDevice();
		//������������
		InitVolumeCtrl();
        //ֹͣͶ��ʱ�ñ�־λ�ᱻ��գ���Ҫ���³�ʼ��
        InitAudioEncoderParam(AUDIO_MODE_BEST);

		m_bIsProjecting = true;
		bFirstKeyFrame = FALSE;
		OnBannerShow();

		//StartAVThread();

		if ( m_bNeedCodeConsult )
		{
			m_cEncoder.SetVideoEncParam(m_tVideoEncParam);
		}
		else
		{
			InitVideoEncoderParam(MEDIA_TYPE_H264);//��Ƶ�ֱ��ʿ��ܻ���Ϊ�ı䣬��Ҫ���³�ʼ��
		}

		OnStartScreenCatch();
	}
	else
	{
		PRINTMSG_TIME("StartProjectScreen failed, m_bIsProjecting:%d\r\n", m_bIsProjecting);
	}
}

void CtouchDlg::StopProjectScreen(bool bNotifyHid)
{
	if (!m_bHidOpen)
	{
		return;
	}

	if (m_bIsProjecting)
	{
		//ȡ��Ĭ����Ƶ�豸
		UnInitDedaultAudioDevice();
		//ȡ����������
		UninitVolumeCtrl();

		m_bIsProjecting = false;
		bFirstKeyFrame = FALSE;
		OnBannerClose();

		OnStopScreenCatch();
		StopAVThread();
		
        if (m_bVideoThreadRun || m_bAudioThreadRun)
        {
            m_nThreadExitCount = 0;
            SetTimer(ThreadExitTimerID, 200, NULL);
        }
        else
        {
            //�������
            GetVideoDataList()->Clear();
            GetAudioDataList()->Clear();
        }

		//֪ͨusb-hid�豸ֹͣͶ����Ϣ
		if (bNotifyHid)
		{
			SendCmdToHid(Ev_NV_StopProjecting_Cmd);
	    }
	}
}

void CtouchDlg::SolveNetStatusNty( NET_STATUS emStatus )
{
	if (m_pcMainDlg)
	{
		if (!m_bIsProjecting)
		{
			m_pcMainDlg->ShowConnectStatus(emStatus);
		}
		else
		{
			if ( emStatus == NET_STATUS_DISCONNECTED
				|| emStatus == NET_STATUS_NO_NETWORK
				|| emStatus == NET_STATUS_NO_MATCH
				|| emStatus == NET_STATUS_CONNECTING
				|| emStatus == NET_STATUS_RESETQUICKSHARE
				|| emStatus == NET_STATUS_RESETWIFI
				|| emStatus == NET_STATUS_FIND_SSID_FAIL )
			{
				StopProjectScreen(true);
				m_pcMainDlg->ShowConnectStatus(emStatus);
			}
		}

	}
}

HANDLE CtouchDlg::GetHidHandle(HID_TYPE emType)
{
	return m_HidDevice[emType].hndHidDevice;
}

THidDevice & CtouchDlg::GetHidDevie(HID_TYPE emType)
{
	return m_HidDevice[emType];
}

void CtouchDlg::StartTrayIconChange()
{
	SetTimer(TrayIconChangeTimerID, 300, NULL);
}

void CtouchDlg::StopTrayIconChange()
{
	KillTimer(TrayIconChangeTimerID);
	
	//��ԭĬ��ͼ��
	m_ntIcon.hIcon = m_hIcon;  //ͼ�꣬ͨ����ԴID�õ�
	::Shell_NotifyIconW(NIM_MODIFY, &m_ntIcon);
}

CEncoder & CtouchDlg::GetEncode()
{
	return m_cEncoder;
}


// Execute the mouse command
void CtouchDlg::SetMouseMessage( WORD wMM,CPoint MousePosition,UINT nFlags,short zDelta )
{
    // Read the mouse packet
    //int mx = (int)((double)MousePosition.x * m_dxWidthScalar);
    //int my = (int)((double)MousePosition.y * m_dyHeightScalar);
    int mx = (int)((double)MousePosition.x);
    int my = (int)((double)MousePosition.y);

#if defined(_DEBUG)
    //DebugMsg("MM=%d (%d,%d)\n",wMM,mx,my);
#endif

    // The time stamp of the operation in milli-seconds
    DWORD dwTime = 0;

    // Extract the flags
    bool bCtrl = nFlags & 1 ? true : false;
    bool bShift = nFlags & 2 ? true : false;

    // Set the number of commands in the input
    int nInputs = 0;

    // Test for the virtual keys
    if (bCtrl)
        nInputs += 2;
    if (bShift)
        nInputs += 2;

    // For the loop termination
    int nHalf = nInputs / 2;

    // Test for the mouse message
    nInputs += 1;
    if (wMM == WM_LBUTTONDBLCLK || wMM == WM_MBUTTONDBLCLK || wMM == WM_RBUTTONDBLCLK)
        nInputs += 3;

    // Create the maximum number of user inputs
    std::auto_ptr<INPUT> UserInput(new INPUT[nInputs]);
    INPUT * pUserInput = UserInput.get();

    // Set the starting input
    int nStart = 0;

    // Set the virtual keydown(s) for the mouse message
    if (bCtrl)
    {
        INPUT & KeyBoardInput = *(pUserInput + (nStart++));
        SetMouseKB(KeyBoardInput,VK_CONTROL,true);
    }
    if (bShift)
    {
        INPUT & KeyBoardInput = *(pUserInput + (nStart++));
        SetMouseKB(KeyBoardInput,VK_SHIFT,true);
    }

    // Build up the mouse input
    int iInput = nStart;
    for (;iInput < (nInputs - nHalf);iInput++,dwTime += 100)
    {
        // Get the mouse input
        INPUT & MouseInput = pUserInput[iInput];

        // Set the input type
        MouseInput.type = INPUT_MOUSE;

        // Get the mouse input structure
        MOUSEINPUT & Mouse = MouseInput.mi;

        // Set the absolute coordinates
        Mouse.dx = mx;
        Mouse.dy = my;

        // Set the mouse data
        Mouse.mouseData = zDelta;

        // Set the flags
        Mouse.dwFlags = MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;

        // Set the button(s)
        if (wMM == WM_LBUTTONDBLCLK)
        {
            if (iInput == 0 || iInput == 2)
                Mouse.dwFlags |= MOUSEEVENTF_LEFTDOWN;
            else if (iInput == 1 || iInput == 3)
                Mouse.dwFlags |= MOUSEEVENTF_LEFTUP;
        }
        else if (wMM == WM_LBUTTONDOWN)
            Mouse.dwFlags |= MOUSEEVENTF_LEFTDOWN;
        else if (wMM == WM_LBUTTONUP)
            Mouse.dwFlags |= MOUSEEVENTF_LEFTUP;
        else if (wMM == WM_MBUTTONDBLCLK)
        {
            if (iInput == 0 || iInput == 2)
                Mouse.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
            else if (iInput == 1 || iInput == 3)
                Mouse.dwFlags |= MOUSEEVENTF_MIDDLEUP;
        }
        else if (wMM == WM_MBUTTONDOWN)
            Mouse.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
        else if (wMM == WM_MBUTTONUP)
            Mouse.dwFlags |= MOUSEEVENTF_MIDDLEUP;
        else if (wMM == WM_MOUSEWHEEL)
            Mouse.dwFlags |= MOUSEEVENTF_WHEEL;
        else if (wMM == WM_MOUSEMOVE)
            Mouse.dwFlags |= MOUSEEVENTF_MOVE;
        else if (wMM == WM_RBUTTONDBLCLK)
        {
            if (iInput == 0 || iInput == 2)
                Mouse.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
            else if (iInput == 1 || iInput == 3)
                Mouse.dwFlags |= MOUSEEVENTF_RIGHTUP;
        }
        else if (wMM == WM_RBUTTONDOWN)
            Mouse.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
        else if (wMM == WM_RBUTTONUP)
            Mouse.dwFlags |= MOUSEEVENTF_RIGHTUP;

        // Set the time
        Mouse.time = dwTime;
    }

    // Set the new starting place
    nStart = iInput;

    // Set the virtual keydown(s) for the mouse message
    if (bCtrl)
    {
        INPUT & KeyBoardInput = *(pUserInput + (nStart++));
        SetMouseKB(KeyBoardInput,VK_CONTROL,false);
    }
    if (bShift)
    {
        INPUT & KeyBoardInput = *(pUserInput + (nStart++));
        SetMouseKB(KeyBoardInput,VK_SHIFT,false);
    }

    // Send the user inputs for the mouse
    SendInput(nInputs,pUserInput,sizeof(INPUT));
}

// Set the virtual keycode for the mouse message
void CtouchDlg::SetMouseKB( INPUT & KeyBoardInput,WORD wVk,bool bDown )
{
    // Set the input type
    KeyBoardInput.type = INPUT_KEYBOARD;

    // Get the keyboard input structure
    KEYBDINPUT & KeyBoard = KeyBoardInput.ki;

    // Set the scan code
    KeyBoard.wScan = 0;

    // Set the ignored fields
    KeyBoard.dwExtraInfo = 0;
    KeyBoard.time = 0;

    // Test for the key being pressed or released
    KeyBoard.dwFlags = bDown ? 0 : KEYEVENTF_KEYUP;

    // Set the virtual key
    KeyBoard.wVk = wVk;
}

void CtouchDlg::SetLogo()
{
	FontStyle font = FontStyleBold;

	CString strPath = m_strLogoPath + LOGO_BMP_FILE;

	CString strName = m_strSysUserName;

	int nScreeWidth = 0;
	int nScreeHeight = 0;
	GetResolution(nScreeWidth, nScreeHeight);

	//logo�Ŀ�Ҫ�����ֱ���һ�£�����ý���Ǳ߲�������
	BOOL bSuccess;
	bSuccess = CLogo::MakeImgByChar( strName, _T("����"), font, LOGO_TEXT_COLOR, Color(255,255,255), 
		Color(255,255,255), LOGO_POINTSIZE, 2, strPath, nScreeWidth,LOGO_HEIGHT);
	if( !bSuccess )
	{
		MessageBox(_T("����̨��ʧ��!"), NULL, MB_OK|MB_ICONERROR );
		return;
	}

	TFullLogoParam tLogoPara;
	memset(&tLogoPara, 0, sizeof(TFullLogoParam));
	tLogoPara.dwLogoPathLen = wcslen(strPath)*2 +2;
	memcpy(tLogoPara.aLogoBmpPath, strPath, tLogoPara.dwLogoPathLen);

	tLogoPara.tLogoParam.dwLogoBgWidth = nScreeWidth;
	tLogoPara.tLogoParam.dwLogoBgHeight = nScreeHeight;

	// ֧��λ�õ���
	tLogoPara.tLogoParam.fXPosRate = float(LOGO_LEFT/nScreeWidth);
	tLogoPara.tLogoParam.fYPosRate = float(TEXT_TOP/nScreeHeight);

	tLogoPara.tLogoParam.tBackBGDColor.RColor = 255;
	tLogoPara.tLogoParam.tBackBGDColor.GColor = 255;
	tLogoPara.tLogoParam.tBackBGDColor.BColor = 255;
	tLogoPara.tLogoParam.tBackBGDColor.u8Transparency = 0;

	m_cEncoder.SetAddLogoIntoEncStream(&tLogoPara);

}

void CtouchDlg::GetResolution(int &nWidth, int &nHeight)
{
	int nWid = GetSystemMetrics(SM_CXSCREEN);
	int nHei = GetSystemMetrics(SM_CYSCREEN);
    //PRINTMSG("��ǰϵͳ�ֱ��ʣ�nWid = %d, nHei = %d\r\n", nWid, nHei);
	if ( 1366 == nWid && 768 == nHei )
	{
		nWid = 1376;
	}

    m_bStretch = false;
    m_bCapOverEncode = false;

	//����  --�������֧������
	if ( nWid > 1920 && nHei > 1080 )
	{
        // ��������ֱ�����ɼ���һ�²���
        if ( float(nWid)/float(nHei) >= 1.5 )    // �ɼ��ֱ��ʱ������ڵ���1.5 --���ȱ�ȫ��
        {
            m_bStretch = true;
        }

        nWid = 1920;
        nHei = 1080;
        m_bCapOverEncode = true;
	}

	nWidth = nWid;
	nHeight = nHei;
}

void CtouchDlg::CheckResolution()
{
    int nWid = GetSystemMetrics(SM_CXSCREEN);
    int nHei = GetSystemMetrics(SM_CYSCREEN);

    m_bOverResLimit = false;
    // �������Ʒֱ���Ͷ��
    if ( nWid > 3840 || nHei > 2160 )
    {
        PRINTMSG("�������Ʒֱ��ʣ�nWid = %d, nHei = %d\r\n", nWid, nHei);
        m_bOverResLimit = true;
    }
}

LRESULT CtouchDlg::OnOpenHidDevSuccess( WPARAM wParam, LPARAM lParam )
{
	if (m_bHidOpen)
	{
		//ע��֪ͨ����
		bool bRst = HID_RegisterDeviceNotification(this->GetSafeHwnd(), &m_hDevNotiy);
		PRINTMSG("HID_RegisterDeviceNotification:%d\r\n",bRst);
		for (int i=HID_TYPE_VIDEO; i<HID_TYPE_NUM; i++)
		{
			PRINTMSG("\r\n--HID�豸��Ϣ:HID_TYPE:%d,hndHidDevice:%d,nOverlapped:%d,oRead.hEvent:%x,oWrite.hEvent:%x,uGetReportTimeout:%d,uSetReportTimeout:%d,wInReportBufferLength:%d,wOutReportBufferLength:%d\r\n",
				i,m_HidDevice[i].hndHidDevice,m_HidDevice[i].nOverlapped,m_HidDevice[i].oRead.hEvent,m_HidDevice[i].oWrite.hEvent,
				m_HidDevice[i].uGetReportTimeout,m_HidDevice[i].uSetReportTimeout,m_HidDevice[i].wInReportBufferLength,
				m_HidDevice[i].wOutReportBufferLength);
		}

		//������ȡusb_hid�߳�
		PRINTMSG("\r\n������ȡ�߳�ThreadRead\r\n");
		m_pcReadThread = AfxBeginThread(ThreadRead, this);
	}

	//����ppt�߳�
	AfxBeginThread(ThreadPpt, this);

	//̨��
	//SetLogo();  //�������ⵥ�ţ�SDM-00134016��ȥ��̨��
	//������Ƶ���ص�
	InitEncoderParam();
	//�����Ƶ�豸
	CheckDedaultAudioDevice();
    //��鵱ǰϵͳ�ֱ���
    CheckResolution();

	//���ʼǱ���ص�����ʱ��
	SetTimer(BatteryPowerTimerID, 30000, NULL);

	//��ʱ���Ͱ汾��ҵ��ֱ���յ��ظ�
	SetTimer(SendVersionTimerID, 300, NULL);
	return 0;
}

HRESULT CtouchDlg::SetDefaultAudioPlaybackDevice( LPCWSTR devID )
{
	IPolicyConfigVista *pPolicyConfig;
	ERole reserved = eConsole;

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), 
		NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);
	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
		pPolicyConfig->Release();
	}
	return hr;
}

void CtouchDlg::InitDefaultAudioDevice()
{
	m_bSetDefAudoDev = false;
	m_wstrSysAudioID = NULL;
	BOOL bMute = FALSE ;//�Ƿ���

	IMMDeviceEnumerator *pEnum = NULL;
	// Create a multimedia device enumerator.
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
	if (SUCCEEDED(hr))
	{
		//�ж��Ƿ���Ĭ�ϵ���Ƶ�豸,�Ǿ��˳�
		IMMDevice  *pDefDevice = NULL;
		hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia,&pDefDevice);
		if (SUCCEEDED(hr))
		{
			IPropertyStore *pStore;
			hr = pDefDevice->OpenPropertyStore(STGM_READ, &pStore);
			if (SUCCEEDED(hr))
			{
				PROPVARIANT friendlyName;
				PropVariantInit(&friendlyName);
				hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
				if (SUCCEEDED(hr))
				{
					CString strTmp = friendlyName.pwszVal;
					if (strTmp.Find(DEF_AUDIO_NAME) != -1)
					{
						m_bSetDefAudoDev = true;
					}
					else
					{
						//����ʹ�õ���Ƶ�豸������ֹͣͶ����ָ�ԭ״̬
						hr = pDefDevice->GetId(&m_wstrSysAudioID);

						IAudioEndpointVolume* pEndptVol = NULL;
						hr = pDefDevice->Activate(__uuidof(IAudioEndpointVolume),
							CLSCTX_ALL, NULL, (void**)&pEndptVol);
						if (SUCCEEDED(hr))
						{
							hr = pEndptVol->GetMute( &bMute );
							SAFE_RELEASE(pEndptVol)
						}
					}
					PropVariantClear(&friendlyName);
				}
				pStore->Release();
			}
			pDefDevice->Release();
		}
		if (m_bSetDefAudoDev)
		{
			pEnum->Release();
			return;
		}

		IMMDeviceCollection *pDevices;
		// Enumerate the output devices.
		hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
		if (SUCCEEDED(hr))
		{
			UINT count;
			pDevices->GetCount(&count);
			if (SUCCEEDED(hr))
			{
				for (int i = 0; i < count; i++)
				{
					IMMDevice *pDevice;
					hr = pDevices->Item(i, &pDevice);
					if (SUCCEEDED(hr))
					{
						LPWSTR wstrID = NULL;
						hr = pDevice->GetId(&wstrID);
						if (SUCCEEDED(hr))
						{
							IPropertyStore *pStore;
							hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
							if (SUCCEEDED(hr))
							{
								PROPVARIANT friendlyName;
								PropVariantInit(&friendlyName);
								hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
								if (SUCCEEDED(hr))
								{
									// if no options, print the device
									// otherwise, find the selected device and set it to be default
									CString strTmp = friendlyName.pwszVal;
									if (strTmp.Find(DEF_AUDIO_NAME) != -1)
									{
										SetDefaultAudioPlaybackDevice(wstrID);
										m_bSetDefAudoDev = true;

										//���þ���״̬
										IAudioEndpointVolume* pEndptVol = NULL;
										hr = pDevice->Activate(__uuidof(IAudioEndpointVolume),
											CLSCTX_ALL, NULL, (void**)&pEndptVol);
										if (SUCCEEDED(hr))
										{
											hr = pEndptVol->SetMute( bMute, &g_guidMyContext );
											SAFE_RELEASE(pEndptVol)
										}
									}
									PropVariantClear(&friendlyName);
								}
								pStore->Release();
							}

							CoTaskMemFree(wstrID);
						}
						pDevice->Release();
					}

					if (m_bSetDefAudoDev)
					{
						break;
					}
				}
			}
			pDevices->Release();
		}
		pEnum->Release();
	}
}

void CtouchDlg::UnInitDedaultAudioDevice()
{
	if (!m_bSetDefAudoDev)
	{
		return;
	}

	m_bSetDefAudoDev = false;
	BOOL bMute = FALSE ;//�Ƿ���

	IMMDeviceEnumerator *pEnum = NULL;
	// Create a multimedia device enumerator.
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
	if (SUCCEEDED(hr))
	{
		//�ж��Ƿ���Ĭ�ϵ���Ƶ�豸,�Ǿ��˳�
		IMMDevice  *pDefDevice = NULL;
		hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia,&pDefDevice);
		if (SUCCEEDED(hr))
		{
			IPropertyStore *pStore;
			hr = pDefDevice->OpenPropertyStore(STGM_READ, &pStore);
			if (SUCCEEDED(hr))
			{
				PROPVARIANT friendlyName;
				PropVariantInit(&friendlyName);
				hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
				if (SUCCEEDED(hr))
				{
					CString strTmp = friendlyName.pwszVal;
					if (strTmp.Find(DEF_AUDIO_NAME) != -1)
					{
						m_bSetDefAudoDev = true;

						IAudioEndpointVolume* pEndptVol = NULL;
						hr = pDefDevice->Activate(__uuidof(IAudioEndpointVolume),
							CLSCTX_ALL, NULL, (void**)&pEndptVol);
						if (SUCCEEDED(hr))
						{
							hr = pEndptVol->GetMute( &bMute );
							SAFE_RELEASE(pEndptVol)
						}
					}
					PropVariantClear(&friendlyName);
				}
				pStore->Release();
			}
			pDefDevice->Release();
		}

		if (m_bSetDefAudoDev)
		{
			if (m_wstrSysAudioID != NULL)
			{
				//�ָ�ϵͳʹ�õ���Ƶ�豸
				SetDefaultAudioPlaybackDevice(m_wstrSysAudioID);
				m_bSetDefAudoDev = false;

				//ͬ������״̬
				IMMDevice *pSysDevice;
				hr = pEnum->GetDevice(m_wstrSysAudioID, &pSysDevice);
				if (SUCCEEDED(hr))
				{
					IAudioEndpointVolume* pEndptVol = NULL;
					hr = pSysDevice->Activate(__uuidof(IAudioEndpointVolume),
						CLSCTX_ALL, NULL, (void**)&pEndptVol);
					if (SUCCEEDED(hr))
					{
						hr = pEndptVol->SetMute( bMute, &g_guidMyContext );
						SAFE_RELEASE(pEndptVol)
					}
					pSysDevice->Release();
				}
			}
		}

		if ( m_wstrSysAudioID != NULL )
		{
			CoTaskMemFree(m_wstrSysAudioID);
			m_wstrSysAudioID = NULL;
		}	

		pEnum->Release();
	}
}

void CtouchDlg::InitVolumeCtrl()
{
	if (!m_bSetDefAudoDev)
	{
		return;
	}

	//���ûص�
	HRESULT hr = S_OK;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;

	// Get enumerator for audio endpoint devices.
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IMMDeviceEnumerator),
		(void**)&pEnumerator);
	HANDLE_ERROR(hr)

	// Get default audio-rendering device.
	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	HANDLE_ERROR(hr)

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_ALL, NULL, (void**)&g_pEndptVol);
	HANDLE_ERROR(hr)

	hr = g_pEndptVol->RegisterControlChangeNotify(
		(IAudioEndpointVolumeCallback*)&EPVolEvents);
	HANDLE_ERROR(hr)

	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)

	//���ô���
	g_hVolumeCtrlDlg = this->GetSafeHwnd();

	//��������
	int nVolume;
	float fVolume;
	hr = g_pEndptVol->GetMasterVolumeLevelScalar(&fVolume);
	nVolume = (int)(MAX_VOL*fVolume + 0.5);
	m_cEncoder.SetAudZoominVol((float)(nVolume*1.0/DIVIDED_NUM));

}

void CtouchDlg::UninitVolumeCtrl()
{
	if (!m_bSetDefAudoDev)
	{
		return;
	}

	if (g_pEndptVol != NULL)
	{
		g_pEndptVol->UnregisterControlChangeNotify(
			(IAudioEndpointVolumeCallback*)&EPVolEvents);
	}

	SAFE_RELEASE(g_pEndptVol)
}

LRESULT CtouchDlg::OnVolumeCtrlChanged( WPARAM wParam, LPARAM lParam )
{
	BOOL bMute = (BOOL)wParam;
	int nVolume = (int)lParam;
	PRINTMSG("\r\n�յ�WM_VOLUMECTRL_CHANGE:nVolume:%d, bMute:%d\r\n", nVolume, bMute);
	if (m_bIsProjecting)
	{
		if ( bMute )
		{
			m_cEncoder.SetAudZoominVol(0);
		}
		else
		{
			m_cEncoder.SetAudZoominVol((float)(nVolume*1.0/DIVIDED_NUM));
		}		
	}

	return 0;
}

LRESULT CtouchDlg::OnPCSleepMsg( WPARAM wParam, LPARAM lParam )
{
	if (m_bHidOpen)
	{
		PRINTMSG("\r\n�յ�ϵͳ���߻�˯����Ϣ\r\n");
		//Ͷ���аε���ֹͣͶ��
		StopProjectScreen(false);

		m_bIsSleep = true;
		m_nSendPCDisconnetCount = 0;

		SendCmdToHid(Ev_NV_PCDisconnet_Cmd);
		SetTimer(SendPCDisconnetTimerID, 200, NULL);

        m_pcMainDlg->ShowConnectStatus(NET_STATUS_CONNECTING);
	}

	return 0;
}

void CtouchDlg::SolvePCNty()
{
	//˯�ߡ����ߴ���
	if (m_bIsSleep)
	{
		PRINTMSG("\r\n˯�ߡ�����:SolvePCNty����\r\n");
	}

	//ϵͳ�������ػ���ע������
	if (m_bIsRebootClose)
	{
		PRINTMSG("\r\n�������ػ���ע��:SolvePCNty����\r\n");
		m_bIsRebootClose = false;
		UdiskOut();
	}
}

LRESULT CtouchDlg::OnPCSleepResume( WPARAM wParam, LPARAM lParam )
{
	//˯�ߡ����ߴ���
	if (m_bIsSleep)
	{
		m_bIsSleep = false;
		//��ʱ���Ͱ汾��ҵ��ֱ���յ��ظ�
		SetTimer(SendVersionTimerID, 300, NULL);
	}
	return 0;
}

LRESULT CtouchDlg::OnPCRebootClose( WPARAM wParam, LPARAM lParam )
{
	if (m_bHidOpen)
	{
		PRINTMSG("\r\n�յ�ϵͳ�������ػ���ע����Ϣ\r\n");
		//Ͷ���аε���ֹͣͶ��
		StopProjectScreen(false);

		m_bIsRebootClose = true;
		m_nSendPCDisconnetCount = 0;

		SendCmdToHid(Ev_NV_PCDisconnet_Cmd);
		SetTimer(SendPCDisconnetTimerID, 200, NULL);
	}

	return 0;
}

bool CtouchDlg::GetSysUserName( CString &strName )
{
	TCHAR achUserName[USERNAME_MAX_LENGTH];
	DWORD dwSize=USERNAME_MAX_LENGTH; 
	::GetUserName(achUserName, &dwSize);
	
	LPUSER_INFO_3 bufptr=NULL;
	NET_API_STATUS t3;
	t3 = NetUserGetInfo(0, achUserName, 3, (LPBYTE*)&bufptr);
	if ( t3 != NERR_Success || bufptr == NULL )
	{
		strName = achUserName;
		return false;
	}
	
	//ȫ����Ϊ�գ���ʹ���û�����ȫ����Ϊ�վ�ʹ���û���
	CString strFullName = bufptr->usri3_full_name;
	if (strFullName.IsEmpty())
	{
		strName = bufptr->usri3_name;
	}
	else
	{
		strName = bufptr->usri3_full_name;
	}
	
	NetApiBufferFree(bufptr);
	bufptr = NULL;

	return true;
}

void CtouchDlg::CheckDedaultAudioDevice()
{
	if (CoCreateGuid(&g_guidMyContext) < 0)
	{
		::MessageBox(NULL, _T("CoCreateGuidʧ��!"), NULL, MB_OK|MB_ICONERROR );
		return;
	}

	bool bSetDefAudoDev = false;

	IMMDeviceEnumerator *pEnum = NULL;
	// Create a multimedia device enumerator.
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
	if (SUCCEEDED(hr))
	{
		//�ж��Ƿ���Ĭ�ϵ���Ƶ�豸,���û���þͷ��أ�����Ѿ����ã���ȡ��
		IMMDevice  *pDefDevice = NULL;
		hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia,&pDefDevice);
		if (SUCCEEDED(hr))
		{
			IPropertyStore *pStore;
			hr = pDefDevice->OpenPropertyStore(STGM_READ, &pStore);
			if (SUCCEEDED(hr))
			{
				PROPVARIANT friendlyName;
				PropVariantInit(&friendlyName);
				hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
				if (SUCCEEDED(hr))
				{
					CString strTmp = friendlyName.pwszVal;
					if (strTmp.Find(DEF_AUDIO_NAME) != -1)
					{
						bSetDefAudoDev = true;
					}
					PropVariantClear(&friendlyName);
				}
				pStore->Release();
			}
			pDefDevice->Release();
		}

		if ( !bSetDefAudoDev )
		{
			pEnum->Release();
			return;
		}

		IMMDeviceCollection *pDevices;
		// Enumerate the output devices.
		hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
		if (SUCCEEDED(hr))
		{
			UINT count;
			pDevices->GetCount(&count);
			if (SUCCEEDED(hr))
			{
				int nLastID = 0;//��¼��NexTransmitter������һ����Ƶ�豸������
				for (int i = 0; i < count; i++)
				{
					IMMDevice *pDevice;
					hr = pDevices->Item(i, &pDevice);
					if (SUCCEEDED(hr))
					{
						LPWSTR wstrID = NULL;
						hr = pDevice->GetId(&wstrID);
						if (SUCCEEDED(hr))
						{
							IPropertyStore *pStore;
							hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
							if (SUCCEEDED(hr))
							{
								PROPVARIANT friendlyName;
								PropVariantInit(&friendlyName);
								hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
								if (SUCCEEDED(hr))
								{
									// if no options, print the device
									// otherwise, find the selected device and set it to be default
									CString strTmp = friendlyName.pwszVal;
									if (strTmp.Find(DEF_AUDIO_NAME) == -1)//����NexTransmitter��Ƶ�豸
									{
										nLastID = i;
									}
									PropVariantClear(&friendlyName);
								}
								pStore->Release();
							}

							CoTaskMemFree(wstrID);
						}
						pDevice->Release();
					}
				}

				//����ϵͳ�е����һ���豸ΪĬ���豸
				IMMDevice *pDevice;
				hr = pDevices->Item(nLastID, &pDevice);
				if (SUCCEEDED(hr))
				{
					LPWSTR wstrID = NULL;
					hr = pDevice->GetId(&wstrID);
					if (SUCCEEDED(hr))
					{
						//����Ĭ���豸
						SetDefaultAudioPlaybackDevice(wstrID);
						//����Ĭ��ͨ���豸
						IPolicyConfigVista *pPolicyConfig;
						ERole reserved = eCommunications;
						HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), 
							NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);
						if (SUCCEEDED(hr))
						{
							hr = pPolicyConfig->SetDefaultEndpoint(wstrID, eCommunications);
							pPolicyConfig->Release();
						}
						CoTaskMemFree(wstrID);
					}
					pDevice->Release();
				}
			}
			pDevices->Release();
		}
		pEnum->Release();
	}
}

LRESULT CtouchDlg::OnPptPlay( WPARAM wParam, LPARAM lParam )
{
	EnumPptStatus emStatus = (EnumPptStatus)wParam;
	
	if ( EM_PPT_PLAYING == emStatus )        //ppt���ڲ���
	{
		m_nStartPptCount = 0;

		//SendPptCmdToHid( EM_PPT_PLAYING );
		//SetTimer(SendPptPlayTimerID, 200, NULL);
	}
	else if ( EM_PPT_NO_PLAY == emStatus )  //pptδ����
	{
		m_nStopPptCount = 0;

		//SendPptCmdToHid( EM_PPT_NO_PLAY );
		//SetTimer(SendPptStopTimerID, 200, NULL);
	}

	return 0;
}

void CtouchDlg::SendPptCmdToHid( EnumPptStatus emStatus )
{
	if ( EM_PPT_PLAYING == emStatus )        //ppt���ڲ���
	{
		BYTE acBuf[LENTH_OUT_BUFFER_CMD] = {0};
		acBuf[0] = Ev_NV_BroadCastPPT_Cmd; 
		acBuf[1] = PPT_STATUS_START;
		bool bRst = HID_SendData2Device(&m_HidDevice[HID_TYPE_CMD], acBuf, LENTH_OUT_BUFFER_CMD, DATA_TYPE_CMD);
		PRINTMSG_TIME("\r\n����ppt���ڲ�������\r\n");
	}
	else if ( EM_PPT_NO_PLAY == emStatus )  //pptδ����
	{
		BYTE acBuf[LENTH_OUT_BUFFER_CMD] = {0};
		acBuf[0] = Ev_NV_BroadCastPPT_Cmd; 
		acBuf[1] = PPT_STATUS_STOP;
		bool bRst = HID_SendData2Device(&m_HidDevice[HID_TYPE_CMD], acBuf, LENTH_OUT_BUFFER_CMD, DATA_TYPE_CMD);
		PRINTMSG_TIME("\r\n����pptδ��������\r\n");
	}
}

void CtouchDlg::SolvePptNty( PPT_STATUS emPptStatus )
{
	switch ( emPptStatus )
	{
	case PPT_STATUS_START_NTF:
		{
			KillTimer(SendPptPlayTimerID);
			PRINTMSG("���߳�--�յ�ppt��ϢPPT_STATUS_START_NTF\r\n");
		}
		break;
	case PPT_STATUS_STOP_NTF:
		{
			KillTimer(SendPptStopTimerID);
			PRINTMSG("���߳�--�յ�ppt��ϢPPT_STATUS_STOP_NTF\r\n");
		}
		break;
	case PPT_STATUS_STOP:
		{
			CPpt::PPT_StopPlay();
		}
		break;
	case PPT_STATUS_NEXTPAGE:
		{
			CPpt::PPT_NextPage();
		}
		break;
	case PPT_STATUS_RPEVIOUSPAGE:
		{
			CPpt::PPT_PriviousPage();
		}
		break;
	}
}

LRESULT CtouchDlg::OnPowerStatus( WPARAM wParam, LPARAM lParam )
{
	if ( !m_bIsProjecting )
	{
		return 0;
	}
	PRINTMSG_NOLOG("Ͷ����-�յ����״̬֪ͨ\r\n");

	CheckBatteryStatus();

	return 0;
}

void CtouchDlg::CheckBatteryStatus()
{
	SYSTEM_POWER_STATUS ps;
	GetSystemPowerStatus(&ps);

	int nLife = 0;
	//�Ƿ�ʹ�ý�����Դ
	if ( ps.ACLineStatus != AC_LINE_ONLINE )
	{	
		if ( ps.BatteryFlag != BATTERY_FLAG_NO_BATTERY && ps.BatteryFlag != BATTERY_FLAG_UNKNOWN )
		{
			nLife = ps.BatteryLifePercent;
			if ( nLife <= 10 )
			{
				m_cEncoder.SetLowPowerMode();
				PRINTMSG_NOLOG("���õ͵���ģʽ\r\n");
			}
		}		
	}
	else
	{
		m_cEncoder.SetNormalPowerMode();
		PRINTMSG_NOLOG("������������ģʽ\r\n");
	}
}

void CtouchDlg::SetConsultVideoParam(u8 byVideoType)
{
	TVideoEncParam tVideoEncParam;
	memset(&tVideoEncParam, 0, sizeof(TVideoEncParam));

	switch(byVideoType)
	{
	case MEDIA_TYPE_H261:
		tVideoEncParam.m_byMinQuant = 1;
		tVideoEncParam.m_byMaxQuant = 31;
		tVideoEncParam.m_byMaxKeyFrameInterval = 30;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 25;
		tVideoEncParam.m_wEncVideoWidth = 352;
		tVideoEncParam.m_wEncVideoHeight = 288;
		tVideoEncParam.m_wBitRate = 768;
		break;
	case MEDIA_TYPE_H263:
		tVideoEncParam.m_byMinQuant = 1;
		tVideoEncParam.m_byMaxQuant = 31;
		tVideoEncParam.m_byMaxKeyFrameInterval = 300;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 25;
		tVideoEncParam.m_wEncVideoWidth = 352;
		tVideoEncParam.m_wEncVideoHeight = 288;
		tVideoEncParam.m_wBitRate = 768;
		break;
	case MEDIA_TYPE_H263PLUS:
		tVideoEncParam.m_byMinQuant = 1;
		tVideoEncParam.m_byMaxQuant = 31;
		tVideoEncParam.m_byMaxKeyFrameInterval = 300;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 5;
		tVideoEncParam.m_wEncVideoWidth = 1024;
		tVideoEncParam.m_wEncVideoHeight = 768;
		tVideoEncParam.m_wBitRate = 1024;
		break;
	case MEDIA_TYPE_H264:
		tVideoEncParam.m_byMinQuant = 10;
		tVideoEncParam.m_byMaxQuant = 45;
		tVideoEncParam.m_byMaxKeyFrameInterval = 3000;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 30;
		tVideoEncParam.m_wEncVideoWidth = 1920;
		tVideoEncParam.m_wEncVideoHeight = 1080;
        // ���������Ϊ2M��Ӳ����Ϊ����ͬ�Ȼ��ʣ���������ʣ��ݶ�Ϊ4M
        if (g_bHwEncStatus)
        {
            tVideoEncParam.m_wBitRate = /*1536*/4096;
            tVideoEncParam.m_wMaxBitRate = 10240;
            tVideoEncParam.m_wMinBitRate = 4096;
        }
        else
        {
            tVideoEncParam.m_wBitRate = /*1536*/2048;
            tVideoEncParam.m_wMaxBitRate = 2048;
            tVideoEncParam.m_wMinBitRate = 2048;
        }
		tVideoEncParam.m_byEncLevel = 1;//1-bp,3-HP
		break;
	case MEDIA_TYPE_H262:
		tVideoEncParam.m_byMinQuant = 5;
		tVideoEncParam.m_byMaxQuant = 31;
		tVideoEncParam.m_byMaxKeyFrameInterval = 75;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 25;
		tVideoEncParam.m_wEncVideoWidth = 352;
		tVideoEncParam.m_wEncVideoHeight = 288;
		tVideoEncParam.m_wBitRate = 768;
		break;
	case MEDIA_TYPE_MP4:
		tVideoEncParam.m_byMinQuant = 2;
		tVideoEncParam.m_byMaxQuant = 31;
		tVideoEncParam.m_byMaxKeyFrameInterval = 300;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 25;
		tVideoEncParam.m_wEncVideoWidth = 352;
		tVideoEncParam.m_wEncVideoHeight = 288;
		tVideoEncParam.m_wBitRate = 768;
		break;
	case MEDIA_TYPE_H265:
		tVideoEncParam.m_byMinQuant = 20;
		tVideoEncParam.m_byMaxQuant = 45;
		tVideoEncParam.m_byMaxKeyFrameInterval = 3000;
		tVideoEncParam.m_byEncType = byVideoType;
		tVideoEncParam.m_byFrameRate = 25;
		tVideoEncParam.m_wEncVideoWidth = 1280;
		tVideoEncParam.m_wEncVideoHeight = 720;
		tVideoEncParam.m_wBitRate = 1536;
		break;
	}

	int nWidth = 0;
	int nHeight = 0;
	GetResolution(nWidth, nHeight);

	tVideoEncParam.m_wEncVideoWidth = nWidth;
	tVideoEncParam.m_wEncVideoHeight = nHeight;

	m_tVideoEncParam = tVideoEncParam;
	//m_cEncoder.SetVideoEncParam(tVideoEncParam);

    if ( m_bCapOverEncode )
    {
        // �ɼ��ֱ��ʱ������ڵ���1.5ʱ�����ȱ�ȫ������֮���ȱȼӺڱ�
        if (m_bStretch)
        {
            m_cEncoder.SetVidDecZoomPolicy( EN_ZOOM_SCALE );
            PRINTMSG("���ȱ�ȫ��\r\n");
        }
        else
        {
            m_cEncoder.SetVidDecZoomPolicy( EN_ZOOM_FILLBLACK );
            PRINTMSG("�ȱȼӺڱ�\r\n");
        }
    }
}

void CtouchDlg::MediaTypeToAudioMode( u8 byMediatype, u8& byRatio )
{
	//һ��MediaType��Ӧ���AudioMode
	switch(byMediatype)
	{
	case MEDIA_TYPE_MP3:
		byRatio = AUDIO_MODE_BEST;
		break;
	case MEDIA_TYPE_PCMA:
		byRatio = AUDIO_MODE_PCMA;
		break;
	case MEDIA_TYPE_PCMU:
		byRatio = AUDIO_MODE_PCMU;
		break;
	case MEDIA_TYPE_G7231:
		if (byRatio != 8)
		{
			byRatio = AUDIO_MODE_G723_6;
		}
		break;
	case MEDIA_TYPE_G728:
		byRatio = AUDIO_MODE_G728;
		break;
	case MEDIA_TYPE_G722:
		byRatio = AUDIO_MODE_G722;
		break;
	case MEDIA_TYPE_G729:
		byRatio = AUDIO_MODE_G729;
		break;
	case MEDIA_TYPE_G7221C:
		if (byRatio != 17 && byRatio != 18) 
		{
			byRatio = AUDIO_MODE_G7221_24;
		}
		break;
	case MEDIA_TYPE_ADPCM:
		byRatio = AUDIO_MODE_ADPCM;
		break;
	case MEDIA_TYPE_AACLC:
		if (byRatio != 16)
		{
			byRatio = AUDIO_MODE_AACLC_32;
		}
		break;
	case MEDIA_TYPE_AACLD:
		if (byRatio != 20 &&
			byRatio != 25 &&
			byRatio != 26 &&
			byRatio != 27 &&
			byRatio != 28)
		{
			byRatio = AUDIO_MODE_AACLD_32_SINGLE;
		}
		break;
	case MEDIA_TYPE_G719:
		byRatio = AUDIO_MODE_G719;
		break;
	case MEDIA_TYPE_RED:
		byRatio = AUDIO_MODE_IBLC;
		break;
	case  MEDIA_TYPE_ISAC_32:
		byRatio = AUDIO_MODE_ISAC_32;
		break;
	case MEDIA_TYPE_OPUS:
		byRatio = AUDIO_MODE_OPUS;
		break;
	default:
		break;
	}
}

bool CtouchDlg::NeedCodeConsult()
{
    if (m_bNeedCodeConsult)
    {
        return true;
    }

    return false;
}