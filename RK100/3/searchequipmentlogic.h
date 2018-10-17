/** @defgroup �������߼���Ԫ 
 *  @version V1.0.0
 *  @author  ylp
 *  @date    2018.9.12
 */
#if !defined(AFX_MAINFRAMELOGIC_H_)
#define AFX_MAINFRAMELOGIC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSearchEquipmentLogic : public CNotifyUIImpl, public Singleton<CSearchEquipmentLogic> 
{
public:
	CSearchEquipmentLogic();
	~CSearchEquipmentLogic();

public:
    void SetDevItem(TDevItem tDevItem);
    TDevItem GetDevItem();

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


    //�޸��������
    bool OnOpenMenu(TNotifyUI& msg);
    bool OnCloseMenu(TNotifyUI& msg);
    bool OnModifyPasswordEntry(TNotifyUI& msg);

    //�����С����ť
    bool OnMinBtnClicked(TNotifyUI& msg);
    //����رհ�ť
    bool OnCloseBtnClicked(TNotifyUI& msg);
    //���������ť
    bool OnSearchBtnClicked(TNotifyUI& msg);
    //������ð�ť
    bool OnResetBtnClicked(TNotifyUI& msg);
    //����б���԰�ť
    bool OnItemOperateBtnClicked(TNotifyUI& msg);
    //��¼�����˳���ť
    bool OnExitBtnClicked(TNotifyUI& msg);

    bool OnRkcDevReflesh( WPARAM wparam, LPARAM lparam, bool& bHandle );

    bool OnRkcConnected( WPARAM wparam, LPARAM lparam, bool& bHandle );

    APP_DECLARE_MSG_MAP()

private:
    static const String strEquipmentList;
    static const String strEquipmentListItem;

    TDevItem m_tDevItem;//������Ե��豸��Ϣ
};

#endif // !defined(AFX_MAINFRAMELOGIC_H_)