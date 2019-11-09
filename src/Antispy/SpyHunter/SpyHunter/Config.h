/* 
 * Copyright (c) [2010-2019] zhenfei.mzf@gmail.com rights reserved.
 * 
 * AntiSpy is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 *     http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
*/
#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <list>

class CConfig
{
public:
	CConfig();
	virtual ~CConfig();
	void WriteConfig();
	void ReadConfig();
	
	// ��ȡ����
	void GetAllRegPath(std::list <CString> &RegPathList);
	CString GetTitle()	{ return m_szTitle; }
	BOOL GetStayOnTop() { return m_bStayOnTop; }
	BOOL GetRandomTitle() { return m_bRandomTitle; }
	BOOL GetAutoDetectNewVersion() { return m_bAutoDetectNewVersion; }
	CHAR GetHotKeyForShowMainWiondow() { return m_chHotKeyForMainWindow; }
	BOOL GetShowProcessDetailInfo() { return m_bShowProcessDetailInfo; }
	BOOL GetShowProcessTreeAsDefault() { return m_bShowProcessTree; }
	BOOL GetShowLowerPaneAsDefault() { return m_bShowLowerPane; }
	BOOL GetSelfProtection() { return m_bSlefProtection; }
	BOOL GetEnableHotkeys() { return m_bUseHotKeys; }

	COLORREF GetHideProcColor();
	COLORREF GetMicrosoftProcColor();
	COLORREF GetNormalProcColor();
	COLORREF GetMicroHaveOtherModuleProcColor();

	COLORREF GetHiddenFileColor();
	COLORREF GetNormalFileColor();
	COLORREF GetSystemFileColor();
	COLORREF GetSystemAndHiddenFileColor();
	
	COLORREF GetNormalModuleColor();
	COLORREF GetHiddenOrHooksModuleColor();
	COLORREF GetNoSignatureModuleColor();
	COLORREF GetMicrosoftModuleColor();

	// ���ú���
	void AddRegPath(CString szRegPath);
	void SetTitle(CString szTitle) { m_szTitle = szTitle; }
	void SetStayOnTop(BOOL bStay) { m_bStayOnTop = bStay; }
	void SetRandomTitle(BOOL bRandom) { m_bRandomTitle = bRandom; }
	void SetAutoDetectNewVersion(BOOL bAtuto) { m_bAutoDetectNewVersion = bAtuto; }
	void SetHotKeyForShowMainWiondow(CHAR cChar) { m_chHotKeyForMainWindow = cChar; }
	void SetShowProcessDetailInfo(CHAR cChar) { m_bShowProcessDetailInfo = cChar; }
	void SetShowProcessTreeAsDefault(BOOL cChar) { m_bShowProcessTree = cChar; }
// 	void SetShowLowerPaneAsDefault(BOOL cChar) { m_bShowLowerPane = cChar; }
	void SetHideProcColor(COLORREF clr) {m_clrHideProc = clr;}
	void SetMicrosoftProcColor(COLORREF clr) {m_clrMicrosoftProc = clr;}
	void SetNormalProcColor(COLORREF clr) {m_clrNormalProc = clr;}
	void SetMicroHaveOtherModuleProcColor(COLORREF clr) {m_clrMicroHaveOtherModuleProc = clr;}
	void SetHiddenFileColor(COLORREF clr) {m_clrHideFile = clr;}
	void SetNormalFileColor(COLORREF clr) {m_clrNormalFile = clr;}
	void SetSystemFileColor(COLORREF clr) {m_clrSystemFile = clr;}
	void SetSystemAndHiddenFileColor(COLORREF clr) {m_clrSystemAndHideFile = clr;}
	void SetNormalModuleColor(COLORREF clr) {m_NormalItemClr = clr;}
	void SetHiddenOrHooksModuleColor(COLORREF clr) {m_HiddenOrHookItemClr = clr;}
	void SetNoSignatureModuleColor(COLORREF clr) {m_NotSignedItemClr = clr;}
	void SetMicrosoftModuleColor(COLORREF clr) {m_MicrosoftItemClr = clr;}
	void SetSelfProtection(BOOL bSet) { m_bSlefProtection = bSet; }
	void SetEnableHotkeys(BOOL bSet) { m_bUseHotKeys = bSet; }

private:
	CString ms2ws(LPCSTR szSrc, int cbMultiChar=-1);
	CStringA ws2ms(LPWSTR szSrc, int cbMultiChar=-1);
	void InitRegPathList();
	void EraseSpace(CString &szString);
	CString GetConfigFilePath();
	CHAR GetFirstChar(CString szString);
	int CString2_int(const CString & str,int radix = 10,int defaultValue = INT_MIN);
	COLORREF CString2_RGB(const CString & str);

private:
	CString m_szTitle;						// ������б���
	std::list <CString> m_RegPathList;		// ע���Ŀ��ٶ�λ��
	BOOL m_bStayOnTop;						// ������ǰ
	BOOL m_bRandomTitle;					// ����������ڱ���
	BOOL m_bAutoDetectNewVersion;			// �������ʱ, �Ƿ��Զ�����°汾
	BOOL m_bSlefProtection;					// �Ƿ��������ұ���
	BOOL m_bShowProcessDetailInfo;			// ����Ƶ����������Ƿ���ʾ��ϸ��Ϣ
	CHAR m_chHotKeyForMainWindow;			// ��ʾ�����ڵ��ȼ�
	BOOL m_bShowLowerPane;					// Ĭ���·���ʾ����ģ���б�
	BOOL m_bShowProcessTree;				// Ĭ����ʾ������
	COLORREF m_clrHideProc;					// ���ؽ��̵���ɫ
	COLORREF m_clrMicrosoftProc;			// ΢��Ľ�����ɫ
	COLORREF m_clrNormalProc;				// �����Ľ��̣�����΢����̵���ɫ
	COLORREF m_clrMicroHaveOtherModuleProc;	// ΢����з�΢��ģ�������ɫ

	COLORREF m_clrHideFile;					// �����ļ�����ɫ
	COLORREF m_clrSystemFile;				// ϵͳ�ļ���ɫ
	COLORREF m_clrSystemAndHideFile;		// ϵͳ�������ص��ļ���ɫ
	COLORREF m_clrNormalFile;				// �����ļ���ɫ

	COLORREF m_NormalItemClr;				// ������Ŀ����΢��ģ����ɫ		
	COLORREF m_HiddenOrHookItemClr;			// ���ػ��߹�����ɫ
	COLORREF m_MicrosoftItemClr;			// ΢�����Ŀ��ɫ
	COLORREF m_NotSignedItemClr;			// û��ǩ����ģ����ɫ
	BOOL m_bUseHotKeys;						// �Ƿ�ʹ���ȼ�
};

#endif