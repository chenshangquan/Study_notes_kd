/* �ͻ��˺ͷ����֮��ͨ�ŵ���Ϣͷ���ṹ���� */
#define u32 unsigned int
#define u16 unsigned short int
#define s32 int
#define u8 unsigned char

typedef struct
{
    u32 dwHandle;             /* ��Ϣ����,͸�� */
    u32 dwRsvd;               /* ���� */
    u32 dwSerial;             /* ��ˮ�� һ������/Ӧ����,��ֵ���ֲ��� */
    u16 wExtLen;              /* ��չͷ�ֽ���,������ֽ� */
    u16 wReserved1;           /* ���� */

    u32 dwProtocolVer;        /* Э��汾��, �ɸ��������ж���,���磺1.1.1.1��0x01010101 */
    u32 dwEvent;              /* �¼�,��ʾ��ͬ�Ĳ�����,��EMRK100EvtId���� */
    s32 nArgv;                /* �豸���������ص�˽�д������ͻ��˴��ݵĲ������� */
    u16 wOptRtn;              /* �¼�������ֵ�����: EMRK100OptRtn */
    u16 wMsgLen;              /* ������Ϣ�峤�� */
} TRK100MsgHead;