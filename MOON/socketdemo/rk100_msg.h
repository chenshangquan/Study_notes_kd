/*******************************************************************************
 * ģ����  : COMMON_MSG
 * �ļ���  : rk100_msg.h
 * ����ļ�:
 * �ļ�����: RK100 TCPͨ��Э�鹫����Ϣ�����ݽṹ����
 * ����      : Ԭ����
 * �汾      : 1.0
 * -----------------------------------------------------------------------------
 * �޸ļ�¼:
 * ��  ��        �汾       �޸���                          �޸�����
 * 2018/09/26  1.0      yuanzengxing@kedacom.com     ����
*******************************************************************************/
#ifndef __RK100_MSG_H
#define __RK100_MSG_H

#define SN_LENGTH                        36    //SN����
#define ENCRYPT_LENGTH                   36    //��Կ����
#define RK100_LISTEN_PORT                8000

/* ��Ϣ�¼�ID���� */
enum EMRK100EvtId
{
    /* �û���¼, ��Ϣ��:TRK100LoginInfo
     * Ӧ����Ϣ��:NULL */
    RK100_EVT_LOGIN                     =2500,
    RK100_EVT_LOGIN_ACK                 =2501,

    /* ��������, ��Ϣ��:TRK100LoginInfo
     * Ӧ����Ϣ��:NULL */
    RK100_EVT_SET_PASSWORD              =2502,
    RK100_EVT_SET_PASSWORD_ACK          =2503,

    /* ��ȡ�弶��Ϣ, ��Ϣ��:NULL
     * Ӧ����Ϣ��:TRK100BrdInfo */
    RK100_EVT_GET_BRD_INFO              =2504,
    RK100_EVT_GET_BRD_INFO_ACK          =2505,

    /* ���ð弶��Ϣ, ��Ϣ��:TRK100BrdInfo
     * Ӧ����Ϣ��:NULL */
    RK100_EVT_SET_BRD_INFO              =2506,
    RK100_EVT_SET_BRD_INFO_ACK          =2507,

    /* �����豸, ��Ϣ��:NULL
     * Ӧ����Ϣ��:NULL */
    RK100_EVT_REBOOT                    =2508,
    RK100_EVT_REBOOT_ACK                =2509,

    /* ���𵥰�TFTP����, ��Ϣ��:TRK100TftpFwUpdate
     * Ӧ����Ϣ��:TRK100UpdateHead
     * ����������������ʵ��ʧ�ܺ���, ������ע��ȴ�ʱ����ܽϳ� */
    RK100_EVT_TFTP_UPDATE_FIRST         =2510,
    RK100_EVT_TFTP_UPDATE_FIRST_ACK     =2511,

    /* ���𵥰�TFTP����, ��Ϣ��:TRK100TftpFwUpdate
     * Ӧ����Ϣ��:NULL
     * ����������������ʵ��ʧ�ܺ���, ������ע��ȴ�ʱ����ܽϳ� */
    RK100_EVT_TFTP_UPDATE_SECOND        =2512,
    RK100_EVT_TFTP_UPDATE_SECOND_ACK    =2513,

    /* ��ȡ�������, ��Ϣ��:NULL
     * Ӧ����Ϣ��:TRK100NetParam */
    RK100_EVT_GET_NETPARAM              =2514,
    RK100_EVT_GET_NETPARAM_ACK          =2515,

    /* �����������, ��Ϣ��:TRK100NetParam
     * Ӧ����Ϣ��:NULL */
    RK100_EVT_SET_NETPARAM              =2516,
    RK100_EVT_SET_NETPARAM_ACK          =2517,

    /* �ָ���������, ��Ϣ��:TRK100LoginInfo
     * Ӧ����Ϣ��:NULL */
    RK100_EVT_RESTORE                   =2518,
    RK100_EVT_RESTORE_ACK               =2519,

    /* ��ȡ����ģʽ, ��Ϣ��:NULL
     * Ӧ����Ϣ��:TRK100WorkMode */
    RK100_EVT_GET_WORK_MODE             =2520,
    RK100_EVT_GET_WORK_MODE_ACK         =2521,

    /* ���ù���ģʽ, ��Ϣ��:TRK100WorkMode
     * Ӧ����Ϣ��:NULL */
    RK100_EVT_SET_WORK_MODE             =2522,
    RK100_EVT_SET_WORK_MODE_ACK         =2523,

    /* ��������, ��Ϣ��:NULL
     * Ӧ����Ϣ��:NULL */
    RK100_EVT_SET_HEART_BEAT            =2524,
    RK100_EVT_SET_HEART_BEAT_ACK        =2525,

    /* ��ȡ����, ��Ϣ��:NULL
     * Ӧ����Ϣ��:TRK100PowerInfo */
    RK100_EVT_GET_POWER                 =2526,
    RK100_EVT_GET_POWER_ACK             =2527,

    /* ������ */
    RK100_EVT_LOGIN_OTHER               =2528,

    /* ������������ʵ��ʧ�ܺ���,������ע��ȴ�ʱ����ܽϳ� */
    RK100_EVT_TFTP_UPDATE_FIRST_NOTIFY  =2529,
    RK100_EVT_TFTP_UPDATE_SECOND_NOTIFY =2530,

    /* ���õ���ģʽ, ��Ϣ��:TRK100DebugMode
     * Ӧ����Ϣ��:NULL */
    RK100_EVT_SET_DEBUG_MODE            =2531,
    RK100_EVT_SET_DEBUG_MODE_ACK        =2532,

    /* ��ȡ����ģʽ, ��Ϣ��:NULL
     * Ӧ����Ϣ��:TRK100DebugMode */
    RK100_EVT_GET_DEBUG_MODE            =2533,
    RK100_EVT_GET_DEBUG_MODE_ACK        =2534,

    //��������
   /*=========================lpc1837����sony��о-xzy add========================*/
	/*һ���۽�,��Ϣ�壺CamFocusAutoManualMode
	 *Ӧ����Ϣ�壺NULL*/
    RK100_EVT_SET_CAM_FOCUS             =2535,
    RK100_EVT_SET_CAM_FOCUS_ACK         =2536,
	/*����ZOOMֵ���ڣ���Ϣ�壺CamZoomVal
	 *Ӧ����Ϣ�壺NULL*/
	RK100_EVT_SET_CAM_ZOOM_VAL            =2537,
	RK100_EVT_SET_CAM_ZOOM_VAL_ACK        =2538,

    /*������Ȧ���ڣ���Ϣ�壺ApertureAutoMode
		 *Ӧ����Ϣ�壺NULL*/
	RK100_EVT_SET_CAM_IRIS            =2539,
	RK100_EVT_SET_CAM_IRIS_ACK        =2540,

	/*�ġ��ع����ã���Ϣ�壺ExposAutoMode
	 *Ӧ����Ϣ�壺NULL*/
	RK100_EVT_SET_CAM_EXPOSURE            =2541,
	RK100_EVT_SET_CAM_EXPOSURE_ACK        =2542,

    /*�塢��ƽ����ڣ���Ϣ�壺CamWBMode
		 *Ӧ����Ϣ�壺CamWBMode_Ack*/
	RK100_EVT_SET_CAM_WB                  =2543,
	RK100_EVT_SET_CAM_WB_ACK              =2544,

	 /*����ͼ���������,��Ϣ�壺CamImagParam
	  *Ӧ����Ϣ�壺NULL*/
	RK100_EVT_SET_CAM_ImagParam           =2545,
	RK100_EVT_SET_CAM_ImagParam_ACK       =2546,
	
	/*�ߡ�������ڣ���Ϣ�壺NULL
	 *Ӧ����Ϣ�壺NULL*/		
	RK100_EVT_SET_CAM_D2NR                  =2547,
	RK100_EVT_SET_CAM_D2NR_ACK              =2548,
	
	/*�ˡ�Ԥ��λ����Ϣ�壺NULL
	 *Ӧ����Ϣ�壺NULL*/
	RK100_EVT_SET_CAM_Preset_PowOnRecall  =2549,
	RK100_EVT_SET_CAM_Preset_PowOnRecall_ACK  =2550,

	/*�š���ȡ��ƽ��ֵ����Ϣ�壺NULL
	 *Ӧ����Ϣ�壺CamWBMode_Ack*/
	RK100_EVT_GET_CAM_WB                =2551,
	RK100_EVT_GET_CAM_WB_ACK            =2552,

    /*�㡢�����ʽ����Ϣ�壺TPOutputFmt        //����
	 *Ӧ����Ϣ�壺NULL*/ 
    RK100_EVT_SET_CAM_OUTPUT_FORMAT   =2553,
    RK100_EVT_SET_CAM_OUTPUT_FORMAT_ACK   =2554,

    /*ʮ��Ԥ��λ�б���Ϣ�壺NULL
	 *Ӧ����Ϣ�壺--- */
	RK100_EVT_SET_CAM_Preset_List            =2555,
	RK100_EVT_SET_CAM_Preset_List_ACK         =2556,

	/*ʮһ������Ԥ��λ����Ϣ�壺NULL
	 *Ӧ����Ϣ�壺--- */
	RK100_EVT_SET_CAM_Preset_Save             =2557,
	RK100_EVT_SET_CAM_Preset_Save_ACK         =2558,

	/*��.һ����������*/
	RK100_EVT_SET_CAM_SHUTTER                =2559,
	RK100_EVT_SET_CAM_SHUTTER_ACK            =2560,

	/*��.������������*/
	RK100_EVT_SET_CAM_GAIN                =2561,
	RK100_EVT_SET_CAM_GAIN_ACK            =2562,

    /*��.һ��D3������ڣ���Ϣ�壺NULL
	 *Ӧ����Ϣ�壺NULL*/		
	RK100_EVT_SET_CAM_D3NR                  =2563,
	RK100_EVT_SET_CAM_D3NR_ACK              =2564,


    /*��оͬ������*/
    RK100_EVT_SET_CAM_ID                  = 2567,
    RK100_EVT_SET_CAM_ID_ACK                  = 2568,

};

/* �¼�������ֵ���� */
enum EMRK100OptRtn
{
    RK100_OPT_RTN_OK                   = 0x0000, /* �����ɹ�*/

    RK100_OPT_ERR_EVT_ID               = 0x8000, /* ��֧�ֵ��¼�ID */
    RK100_OPT_ERR_USER                 = 0x8001, /* �û������� */
    RK100_OPT_ERR_PASSWORD             = 0x8002, /* ������� */
    RK100_OPT_ERR_NO_LOGIN             = 0x8003, /* �û�δ��¼ */
    RK100_OPT_ERR_PARAM                = 0x8004, /* �������� */
    RK100_OPT_ERR_LEN                  = 0x8005, /* ���ȴ��� */
    RK100_OPT_ERR_NO_MEMORY            = 0x8006, /* û���ڴ� */
    RK100_OPT_ERR_CHKSUM               = 0x8007, /* У��ʹ��� */
    RK100_OPT_ERR_FUNC_FAILE           = 0x8008, /* �ײ㺯������ʧ�� */
    RK100_OPT_ERR_BUSY                 = 0x8009, /* �豸��æ */
    RK100_OPT_ERR_FIRST_LOGIN          = 0x800A, /* �״ε�¼ */
    RK100_OPT_ERR_LOCKED               = 0x800B, /* �豸���� */
    RK100_OPT_ERR_UNKNOWN              = 0xFFFF, /* δ֪���� */
};

/* �豸����״̬���� */
enum EMRK100DevStat
{
    RK100_DEV_STAT_RUN_OK              = 0x0000, /* �豸�������� */
    RK100_DEV_STAT_UPDATE              = 0x0001, /* �豸��������״̬ */
    RK100_DEV_STAT_IN_BOOT             = 0x8001, /* �豸����BOOTģʽ */
    RK100_DEV_STAT_WAINING             = 0x8002, /* �豸�и澯 */
    RK100_DEV_STAT_UNKNOWN             = 0xFFFF, /* δ֪״̬ */
};

/* ��¼У�� */
typedef struct
{
    char szUsr[64];
    char szPwd[64];
} TRK100LoginInfo;

/* ������Ϣ */
typedef struct
{
    u32 dwCurrent;            /* ����Ĺ���mW */
    u32 dwPower;              /* ����ĵ���mA */
} TRK100PowerInfo;

/* ������Ϣ���� */
typedef struct
{
    u8   abyMac[6];           /* MAC��ַ */
    u16  wHid;                /* hid */
    u16  wPid;                /* pid */
    char szSoftVer[32];       /* ����ҵ���������ʱ��, ��: "Jan 9 2017 15:56:12" */
    char szSn[SN_LENGTH];     /* ���к�,ֻ�� */
    u32  dwExt;               /* ��չ���� */
} TRK100BrdInfo;

/* ͨ��TFTP�������� */
typedef struct
{
    u32  dwTftpSvrIp;         /* TFTP ��������ַ,������ */
    u32  dwFileSize;          /* �ļ���С */
    char szFwTypeName[64];    /* �̼��������� */
    char szFileFullName[260]; /* �����ļ��� */
    u8   byChkSumValid;       /* �汾����У�����Ч��־����0ʱ�����������Ч */
    u8   byChkSum;            /* �汾����У��ͣ��ļ��ֽ��ۼӵ�ֵ */
    u8   byRsved;             /* ���� */
} TRK100TftpFwUpdate;

 /* ������� */
typedef struct {
    u8  byDhcpEn;             /* 1��ʾʹ�ö�̬IP��ַ,���²����ڻ�ȡ����ַ��Ϣ��ֵ */
    u8  byRsved[1];           /* Ԥ���ֶ� */
    u16 MqttPort;             /* mqtt�˿� */
    u32 dwIP;                 /* IP��ַ */
    u32 dwSubMask;            /* �������� */
    u32 dwGateway;            /* ���� */
    u32 dwMqttIP;             /* mqtt��ַ */
} TRK100NetParam;

/* ����ģʽ */
typedef struct
{
    u8 AcFlag;                /* AC�����ı�־ 1:���� 0: ������ */
    u8 AcDelay;               /* AC�ϵ��Ժ����ʱ0~180s */
    u8 Uart0Flag;             /* uart0 ������־ 1: ���� 0: ������ */
    u8 Uart0SndDat[32];       /* uart0 ���͵����� */
    u8 Uart0RevDat[32];       /* uart0 ���յ����� */
    u8 Uart0SndLen;           /* uart0 ���͵����ݳ��� */
    u8 Uart0RevLen;           /* uart0 ���յ����ݳ��� */
    u8 Uart0Delay;            /* uart0 ������ʱ */
    u8 Uart1Flag;             /* uart1 ������־ 1: ���� 0: ������ */
    u8 Uart1SndDat[32];       /* uart1 ���͵����� */
    u8 Uart1RevDat[32];       /* uart1 ���յ����� */
    u8 Uart1SndLen;           /* uart1 ���͵����ݳ��� */
    u8 Uart1RevLen;           /* uart1 ���յ����ݳ��� */
    u8 Uart1Delay;            /* uart1 ������ʱ */
    u8 Uart2Flag;             /* uart2 ������־ 1: ���� 0: ������ */
    u8 Uart2SndDat[32];       /* uart2 ���͵����� */
    u8 Uart2RevDat[32];       /* uart2 ���յ����� */
    u8 Uart2SndLen;           /* uart2 ���͵����ݳ��� */
    u8 Uart2RevLen;           /* uart2 ���յ����ݳ��� */
    u8 Uart2Delay;            /* uart2 ������ʱ */
    u8 Ir0Flag;               /* ���ⷢ��0 ������־ 1: ���� 0: ������ */
    u8 Ir0Index;              /* ���ⷢ��0�������±� CalcDate[Ir0Index] */
    u8 Ir0Delay;              /* ���ⷢ��0��ʱ */
    u8 Ir1Flag;               /* ���ⷢ��1 ������־ 1: ���� 0: ������ */
    u8 Ir1Index;              /* ���ⷢ��1�������±� CalcDate[Ir1Index] */
    u8 Ir1Delay;              /* ���ⷢ��1��ʱ */
    u8 RfFlag;                /* RFģ�鿪����־ 1: ���� 0: ������ */
    u8 RfID[5][5];            /* RFģ��ID�� RfID[0][0]:0x0 ���� 0x1:���� RfID[0][1]..RfID[0][4]:RF���ص�ID */
} TFuncMod;

/* ����ģʽ�ṹ�� */
typedef struct
{
    TFuncMod FuncMod[4];      /* 4������ģʽ 0:���� 1:�ػ� 2:���� 3:���� */
} TRK100WorkMode;

typedef struct
{
    u32 Magic;
    u8  FileCrc;
    u8  Version[64];
    u8  TftpCount;
} TRK100UpdateHead;

typedef struct
{
    u8  DebugSW;              /* 1:�������� 0:�رմ��� */
} TRK100DebugMode;
#endif


/*=======================xzy add=======================*/
//��������60/30fps  �ع�--����
typedef struct 
{
	u8 SixtyFpsModeFlag;
	u8 ThirtyFpsModeFlag;
	u8 Shutter_30Sp ;         // 1/30
	u8 Shutter_60Sp ;         // 1/60
	u8 Shutter_90Sp ;         // 1/90
	u8 Shutter_100Sp;         // 1/100(Ĭ��)
	u8 Shutter_125Sp;         // 1/125
	u8 Shutter_180Sp;         // 1/180
	u8 Shutter_250Sp;         // 1/250
	u8 Shutter_350Sp;         // 1/350
	u8 Shutter_500Sp;         // 1/500
	u8 Shutter_725Sp;         // 1/725
	u8 Shutter_1000Sp;        // 1/1000
	u8 Shutter_1500Sp;        // 1/1500
	u8 Shutter_2000Sp;        // 1/2000
	u8 Shutter_3000Sp;        // 1/3000
	u8 Shutter_4000Sp;        // 1/4000
	u8 Shutter_6000Sp;        // 1/6000
	u8 Shutter_10000Sp;        // 1/10000
}TPSOrThShutter;//xzy 

//��������50/25fps  �ع�--����
typedef struct  
{
	u8 FiftyFpsModeFlag;
	u8 TenwFifFpsModeFlag;
	u8 Shutter_25Spd ;         // 1/25
	u8 Shutter_50Spd ;         // 1/50
	u8 Shutter_60Spd ;         // 1/60
	u8 Shutter_100Spd;         // 1/100(Ĭ��)
	u8 Shutter_120Spd;         // 1/120
	u8 Shutter_150Spd;         // 1/150
	u8 Shutter_215Spd;         // 1/215
	u8 Shutter_300Spd;         // 1/300
	u8 Shutter_425Spd;         // 1/425
	u8 Shutter_600Spd;         // 1/600
	u8 Shutter_1000Spd;        // 1/1000
	u8 Shutter_1250Spd;        // 1/1250
	u8 Shutter_1750Spd;        // 1/1750
	u8 Shutter_2500Spd;        // 1/2500
	u8 Shutter_3500Spd;        // 1/3500
	u8 Shutter_6000Spd;        // 1/6000
	u8 Shutter_10000Spd;        // 1/10000
}TPFOrTwShutter;//xzy


typedef struct
{
    u8 AutoModeFlag;      /* 1:�����Զ�ģʽ 0:�رտ����Զ�ģʽ�������ֶ�ģʽ */
	u8 ManualModeFlag;
    u8 focusFarFlag;
    u8 focusNearFlag;
    u8 FarNearStopFlag;
} TCamFocusAutoManualMode;

typedef struct
{
    u32 InputVal;  //��������ľ�ȷZoomֵ
    u8 InputPreciseValFlag;   /* ���뾫ȷֵ��1:�����Զ�ģʽ 0:�رտ����Զ�ģʽ�������ֶ�ģʽ */
	u8 ZoomUpFlag;  //Zoom+
	u8 ZoomDownFlag;  //Zoom-
	u8 ZoomUpDownStopFlag;
} TCamZoomVal; 


typedef struct
{
	u8 IrisAutoFlag;
	u8 IrisBackCompFlag;  //���ⲹ��
    u8 IrisManuFlag;  
	u8 optIrisF4_4Flag;
	u8 optIrisF4_0Flag;
	u8 optIrisF3_7Flag;
	u8 optIrisF3_4Flag;
	u8 optIrisF3_1Flag;
    u8 optIrisF2_8Flag;
} TIrisAutoManuMode;    //��Ȧ

typedef struct
{
    u8 ExposAutoModeFlag;    /* 1:�����Զ�ģʽ 0:�رտ����Զ�ģʽ�������ֶ�ģʽ */
} TExposAutoMode;  

typedef struct
{
     /* 1:�����Զ�ģʽ 0:�رտ����Զ�ģʽ�������ֶ�ģʽ */
	  //u8 ExposManuModeFlag;  
      TPSOrThShutter SixtyOrThirtyMode;  //
      TPFOrTwShutter FiftyOrTwentyMode;  //
} TShutterMode;  

typedef struct
{
    u8 CAMGainCmdUpFlag;
    u8 CAMGainCmdDownFlag;
    //u8 CAMGainEnterPrecisValFlag;
    u8 GainInputVal;
} TGainMode;

typedef struct
{
    u8 CamWBAutoModeFlag; 
	u8 CamWBManuModeFlag;
   	u8 RGainVal; //
    u8 BGainVal;  //
} TCamWBMode; 

typedef struct
{
   u8 BrightVal; //����
   u8 ColorHueVal;//ɫ��
   u8 ColorGainVal;//���Ͷ�
   u8 Gamma_opt_1_flag;// /* 1:�����Զ�ģʽ 0:�رտ����Զ�ģʽ�������ֶ�ģʽ */
   u8 Gamma_opt_2_flag;//
   u8 Gamma_opt_3_flag;//
   
} TCamImagParam;  

typedef struct
{
    u8 D2NROnFlag;
    u8 D2NROffFlag;
    u8 D2NR_level_1_Flag;
    u8 D2NR_level_2_Flag;
    u8 D2NR_level_3_Flag;
    u8 D2NR_level_4_Flag;
    u8 D2NR_level_5_Flag;
} TCamD2NRMode; //

typedef struct
{
    u8 D3NROnFlag;
    u8 D3NROffFlag;
    u8 D3NR_level_1_Flag;
    u8 D3NR_level_2_Flag;
    u8 D3NR_level_3_Flag;
    u8 D3NR_level_4_Flag;
    u8 D3NR_level_5_Flag;
} TCamD3NRMode;  //

typedef struct
{
    u8 RVal;
	  u8 BVal;
}TCamWBMode_Ack;

typedef struct
{
    u8 FMT4K_30fps_flag;
    u8 FMT4K_25fps_flag;
    u8 FMT1080_60fps_flag;
    u8 FMT1080_50fps_flag;
    u8 FMT1080_30fps_flag;
    u8 FMT1080_25fps_flag;
    u8 FMT720_60fps_flag;
    u8 FMT720_50fps_flag;
}TPOutputFmt;//�����ʽ������

typedef struct
{
    u8 PresetLaststate;
    u8 PresetNumber1;
    u8 PresetNumber2;
    u8 PresetNumber3;
    u8 PresetNumber4;
    u8 PresetNumber5;
    u8 PresetNumber6;
    u8 PresetNumber7;
    u8 PresetNumber8;
}TCamPresetNumberList;


typedef struct
{
    u8 PresetSaveFlge;
    u8 PresetRecallFlag;
    TCamPresetNumberList SelctPresetNumberFlag;  //
    u8 PresetDelFlag;
    u8 PresetModifyFlag;
    u8 PresetNumber;  //Ԥ��λ���
}TCamPresetSave;

typedef struct
{
    u8 CamNum1Flag;  //��о1
    u8 CamNum2Flag;
    u8 CamNum3Flag;  
    u8 SyncFlag;  //ͬ��
}TCamID;

typedef struct tagTTPMoonCamInfo
{
    //u8                 byIndex;//�����ʶ����о1��2��3
    TCamID       TCamIDIndex;//�����ʶ����о1��2��3
    TPOutputFmt       OutputFmt;
    u32                dwZoomPos;         //ʵ�����õ���о����Ұ��zoom��ֵ-xzy
    //	BOOL               bIsUsed; //Ԥ��λ���Ƿ�����
    //	BOOL               bIsPreUsed;//�����Ƿ����Ԥ��λ1
    
    TCamFocusAutoManualMode       FocusMode;//�۽�ģʽ-xzy
    //	u32                dwFocusValue;//�۽�ֵ
    TIrisAutoManuMode       IrisMode;//��Ȧģʽ-xzy
    TExposAutoMode       ExpMode;//�ع�ģʽ-xzy 
    TShutterMode    ShutterMode; //����
    TGainMode        GainMode;//����
    TCamWBMode         WBMode;//��ƽ��ģʽ���ֶ����Զ�-xzy
    TCamImagParam    CamImagParam;
    TCamD2NRMode     CamD2NRMode;
    TCamD3NRMode     CamD3NRMode;
}TTPMoonCamInfo;