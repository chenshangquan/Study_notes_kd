/** @defgroup ���ò˵��߼���Ԫ 
 *  @version V1.0.0
 *  @author  chenshangquan
 *  @date    2018.11.9
 */
#if !defined(AFX_SETMENU_H_)
#define AFX_SETMENU_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSetMenuLogic : public CNotifyUIImpl, public Singleton<CSetMenuLogic> 
{
public:
	CSetMenuLogic();
	~CSetMenuLogic();

public:

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

    // �رղ˵�
    bool OnCloseMenu(TNotifyUI& msg);
    // ����޸�����ѡ��
    bool OnModifyPasswordEntry(TNotifyUI& msg);
    // �������ѡ��
    bool OnAboutItemClicked(TNotifyUI& mst);
    APP_DECLARE_MSG_MAP()

private:

};

#endif // !defined(AFX_SETMENU_H_)