/** @defgroup ����������߼���Ԫ 
 *  @version V1.0.0
 *  @author  ylp
 *  @date    2018.10.9
 */
#if !defined(AFX_TOOLFRAME_H_)
#define AFX_TOOLFRAME_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum EmConfCtrlTabID
{
    emTabID_NetworkSetup = 0,
    emTabID_RadioSetup,
    emTabID_WorkingMode,
    emTabID_BackupUpgrade,
    emTabID_ModifyPassword,
    emTabID_AboutInfo
};

class CToolFrameLogic : public CNotifyUIImpl, public Singleton<CToolFrameLogic> 
{
public:
	CToolFrameLogic();
	~CToolFrameLogic();

public:
    //ˢ�½����·��ؼ�
    void InitBottomLeb();
    //�޸��������
    void OnModifyPasswordEntry();
    //������Ϣ���
    void OnTabAboutInfo();

    void SetFirstLogin(bool bFirstLogin);

protected:
	/** ���ڴ��� 
	*  @param[in] ��Ϣ
	*  @return �Ƿ�ɹ�
	*/
	bool OnCreate(TNotifyUI& msg);

	/** �رմ�����Ϣ 
	 *  @param[in] 
	 *  @node 
	 *  @return 
     */
    bool OnDestory(TNotifyUI& msg);

	/** ��ʼ����Ϣ 
	 *  @param[in] 
	 *  @node 
	 *  @return 
     */
    bool OnInit(TNotifyUI& msg);

    //��������
    bool OnTabNetworkSetup(TNotifyUI& msg);
    //��Ƶ����
    bool OnTabRadioSetup(TNotifyUI& msg);
    //����ģʽ
    bool OnTabWorkingMode(TNotifyUI& msg);
    //��������
    bool OnTabBackupUpgrade(TNotifyUI& msg);

    bool OnRkcPowerReflesh( WPARAM wparam, LPARAM lparam, bool& bHandle );

    bool OnRkcDisconnected( WPARAM wparam, LPARAM lparam, bool& bHandle );

    APP_DECLARE_MSG_MAP()

private:
    bool m_bIsFirstLogin;//�Ƿ��ǵ�һ�ε�¼
};

#endif // !defined(AFX_TOOLFRAME_H_)