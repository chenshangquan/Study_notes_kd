#pragma once


class CSystem : public Singleton<CSystem>
{
public:
/*
	xml��ǩ����
*/
	static const String strMainWindow;	// �����豸����
	static const String strSkin;		// ����·��
	static const String strDefaultSkin; // Ĭ��Ƥ��·��

public:
	CSystem(void);
	CSystem(String strFile);
	~CSystem(void);

/** ϵͳ���� 
 *  @param[in] ��xml�ļ� 
 *  @node 
 *  @return �Ƿ�ɹ�
 */
	bool Launch();

protected:
/** ��ʼ��ϵͳ 
 *  @param[in] 
 *  @node ��Ҫ�Ƕ�ȡxml������Ϣ
 *  @return �Ƿ�ɹ�
 */
	bool IniSystem();

protected:
/*
	���ļ�·��
*/
	String m_strFile;
};



extern const String g_stcStrMainFrameDlg;
extern const String g_stcStrLoginDlg;
extern const String g_stcStrToolFrameDlg;
extern const String g_stcStrPasswordHelpDlg;
