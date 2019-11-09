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
#pragma once
#include "afxcmn.h"
#include "ConnectDriver.h"
#include "Function.h"
#include "SignVerifyDlg.h"
#include "..\\..\\Common\Common.h"
#include <list>
#include <vector>
#include "ProcessFunc.h"
#include "afxwin.h"
#include "ColumnTreeCtrl.h"
#include "xySplitterWnd.h"
#include "ListProcess.h"
#include "DllModuleDlg.h"
#include "HandleDlg.h"
#include "ThreadDlg.h"
#include "ProcessMemoryDlg.h"
#include "ProcessWndDlg.h"
#include "HotKeyDlg.h"
#include "ProcessTimerDlg.h"
#include "ProcessPrivilegesDlg.h"

using namespace std;

typedef enum _BELOW_DLG_INDEX_
{
	eDllModuleDlg,
	eHandleDlg,
	eThreadDlg,
	eMemoryDlg,
	eWndDlg,
	eHotKeyDlg,
	eTimerDlg,
	ePrivilegeDlg,
}BELOW_DLG_INDEX;

typedef struct _PROCESS_INFO_EX_
{
	ULONG ulPid;
	ULONG ulParentPid;
	ULONG ulEprocess;
	ULONG nIconOrder; // ����ͼ�������
	CString szPath;
	BOOL bInsert;
	ULONGLONG nCreateTime;
	HTREEITEM hItem;
}PROCESS_INFO_EX, *PPROCESS_INFO_EX;

// CListProcessDlg �Ի���

class CListProcessDlg : public CDialog
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CListProcessDlg)

public:
	CListProcessDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CListProcessDlg();

// �Ի�������
	enum { IDD = IDD_PROCESS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL GetRing3ProcessList();
	VOID ListProcess();
	afx_msg void OnNMCustomdrawProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnProcessRefresh();
	BOOL IsProcessHide(ULONG pid);
	BOOL CanOpenProcess(DWORD dwPid);
	afx_msg void OnCheckSign();
	afx_msg void OnCheckAllSign();
	BOOL IsNotSignItem(ULONG nItem);
	afx_msg void OnKillProcess();
	afx_msg void OnKillDelete();
	afx_msg void OnSuspendProcess();
	afx_msg void OnResumeProcess();
	afx_msg void OnCopyProcessName();
	afx_msg void OnCopyProcessPath();
	afx_msg void OnCheckAttribute();
	afx_msg void OnLocationExplorer();
	afx_msg void OnSearchOnline();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowAllProcessInfo();
	afx_msg LRESULT VerifySignOver(WPARAM wParam, LPARAM lParam);  
	afx_msg void OnExportExcel();
	afx_msg void OnExportText();
	afx_msg void OnDumpProcessMemory();
	afx_msg void OnProcessFindModule();
	afx_msg void OnProcessFindUnsignedModues();
	SUSPEND_OR_RESUME_TYPE ProcessMenuResumeOrSuspend();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMDblclkProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnProcessHook();
	afx_msg void OnCreateMinidump();
	afx_msg void OnCreateFulldump();
	BOOL CreateDump(DWORD dwFlag, CString szPath);
	CString CreateOpenFileDlg();
	CString GetParentProcessInfo(DWORD dwParentPid);
	void ShowProcessAsList();
	void ShowProcessAsTreeList();
	VOID SortByParentPid(vector<PROCESS_INFO> &ProcessListEx);
	BOOL IsHaveParent(PROCESS_INFO_EX info);
	void InsertTreeProcessChildItem(PROCESS_INFO_EX* pItem, HTREEITEM hRoot);
	afx_msg void OnRclickedColumntree(LPNMHDR pNMHDR, LRESULT* pResult);
 	HTREEITEM GetTreeListSelectItem();
	void DblclkColumntree(HTREEITEM hItem);
	afx_msg void OnShowProcessTree();
	afx_msg void OnUpdateShowProcessTree(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnKillProcessTree();
	void KillSubProcess(DWORD dwPid);
	afx_msg void OnInjectDll();
	void AdjustShowWindow();
	afx_msg void OnLvnItemchangedProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeList(NMHDR *pNMHDR, LRESULT *pResult);
	void MouseOverListCtrl(CPoint point);
	int ListHitTestEx(CPoint& point, int* nCol);
	HTREEITEM ListTreeHitTestEx(CPoint& point, int* nCol);
	void MouseOverCtrl(CPoint point, HWND hWnd);
	afx_msg LRESULT OnClickListHeaderStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgEnumTimer(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgEnumModules(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgEnumHandles(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgEnumThreads(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgEnumMemory(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgEnumWindows(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgEnumHotKeys(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgEnumPrivileges(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClickListHeaderEnd(WPARAM wParam, LPARAM lParam);
	BOOL IsHaveNotMicrosoftModule(DWORD dwPID);
	CString FileTime2SystemTime(ULONGLONG ulCreateTile);
	afx_msg void OnProcessLocationAtFileManager();
	afx_msg LRESULT OnUpdateProcessInfoStatus(WPARAM wParam, LPARAM lParam);  
	SUSPEND_OR_RESUME_TYPE IsSuspendOrResumeProcessTree();
	void AddProcessItem(PROCESS_INFO item);
	afx_msg void OnLookWindowForProcess();
	void GotoProcess(DWORD dwPid);
	void GotoThread(DWORD dwPid, DWORD dwTid);
	void KillProcessByFindWindow(DWORD dwPid);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	void InitProcInfoWndTab(CDialog *pDlg);
	void UpdateBelowDlg();
	LRESULT ResizeAllProcWnd(WPARAM wParam, LPARAM lParam);
	PPROCESS_INFO GetProcessInfoByItem(int nItem);
	PPROCESS_INFO_EX GetProcessInfoExByItem(HTREEITEM nItem);
	void TransferProcessInfo2ProcessInfoEx();
	void AddProcessTreeItem(PROCESS_INFO_EX* pInfo, HTREEITEM hItem);
	SUSPEND_OR_RESUME_TYPE GetProcessSuspendOrResumeStatus(ULONG nPid, ULONG pEprocess);
	void ShowProcessListDetailInfo(int nItem);
	void ShowProcessTreeDetailInfo(HTREEITEM hItem);
	void EnumProcessModules(ULONG nPid, ULONG pEprocess, CString szImage);
	void EnumProcessHandles(ULONG nPid, ULONG pEprocess, CString szImage);
	afx_msg void OnNMClickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTreeList(NMHDR *pNMHDR, LRESULT *pResult);
	void EnumProcessThreads(ULONG nPid, ULONG pEprocess, CString szImage);
	void EnumProcessMemorys(ULONG nPid, ULONG pEprocess, CString szImage);
	void EnumProcessWnds(ULONG nPid, ULONG pEprocess, CString szImage);
	void EnumProcessHotKeys(ULONG nPid, ULONG pEprocess, CString szImage);
	void EnumProcessTimers(ULONG nPid, ULONG pEprocess, CString szImage);
	void EnumProcessPrivileges(ULONG nPid, ULONG pEprocess, CString szImage);
	CString GetCurrentProcessInfo(ULONG &nPid, ULONG &pEprocess);
public:	
	CString m_szProcessCnt;						// �����·�״̬,��ʾ������������Ϣ
	CImageList m_ProImageList;					// ����Image�б�
	list<ULONG> m_Ring3ProcessList;				// ����Ring3ö�ٵõ���list,ֻ����pid,�����Ƚ���Щ�����صĽ���
	CSignVerifyDlg m_SignVerifyDlg;				// ��֤ǩ���ĶԻ�����
	vector<PROCESS_INFO> m_Ring0ProcessList;	// R0ö�������Ľ�����Ϣ�б�
	CConnectDriver m_Driver;					// ������ͨ�ŵ���
	CommonFunctions m_Functions;				// ���õ�API��
	CTabCtrl m_tab;								// �����Tab�ؼ�
	HACCEL m_hAccel;							// ��ݼ����
	HICON m_hProcessIcon;						// û����֮��Ӧ�Ľ���Image,������һ��Ĭ�ϵ�Ico���
	CProcessFunc m_ProcessFunc;					// ���õĽ��̲����ຯ��, eg.�ں˴򿪽���
	CImageList m_imgList;						// �������οؼ���֮��Ӧ��Image�б�
	vector<PROCESS_INFO_EX> m_ProcessListEx;	// �����οؼ���صĽ����б���Ϣ
	BOOL m_bShowAsTreeList;						// �Ƿ������οؼ�����ʽ��ʾ
	CSortListCtrl m_processList;				// ���ײ�ģ���б�Ľ����б�
	CColumnTreeCtrl m_processTree;				// ���·�ģ���б�Ľ������οؼ�
	ULONG m_nProcessCnt;						// �ܹ��Ľ�������						
	ULONG m_nHideCnt;							// ���ؽ��̵�����
	ULONG m_nDenyAccessCnt;						// R3�޷��򿪵Ľ�������
	CXTPTipWindow m_tipWindow;					// ������ʾ������ϸ��Ϣ������
	BOOL m_bShowTipWindow;						// �Ƿ���ʾ������ϸ��Ϣ������
	COLORREF m_clrHideProc;						// ���ؽ��̵���ɫ
	COLORREF m_clrMicrosoftProc;				// ΢��Ľ�����ɫ
	COLORREF m_clrNormalProc;					// �����Ľ��̣�����΢����̵���ɫ
	COLORREF m_clrMicroHaveOtherModuleProc;		// ΢����з�΢��ģ�������ɫ
	COLORREF m_clrNoSigniture;					// û������ǩ���Ľ�����ɫ
	vector<ITEM_COLOR_INFO> m_vectorColor;		// ��ɫ�б�
	vector<PROCESS_INFO> m_vectorProcesses;		// ���б������ʾ�Ľ����б�
	CySplitterWnd m_ySplitter;					// �ָ���
	CListProcess m_clsProcess;					// ������
	BOOL m_bShowBelow;							// �Ƿ���Ҫ��ʾ�·����ݣ�������б�ͷ��ʱ���ǲ���Ҫ��ʾ��
	vector<PROCESS_INFO_EX> m_vectorProcessEx;	// �����οؼ���صĽ����б���Ϣ
	CDialog *m_pBelowDialog[20];				// ����Ĵ���
	ULONG m_nBelowDialog;						// �ܹ��м�������
	BELOW_DLG_INDEX m_nCurSel;					// ����Ĵ�������ѡ������
	CDllModuleDlg m_ModuleDlg;					// �����ģ��Ի���
	CHandleDlg m_HandleDlg;						// ����ľ���Ի���
	CThreadDlg m_ThreadDlg;						// ������̶߳Ի���
	CProcessMemoryDlg m_MemoryDlg;				// ������ڴ�Ի���
	CProcessWndDlg	m_WndDlg;					// ����Ĵ��ڶԻ���
	CHotKeyDlg	m_HotKeyDlg;					// ������ȼ��Ի���
	CProcessTimerDlg m_TimerDlg;				// ����Ķ�ʱ���Ի���
	CProcessPrivilegesDlg m_PrivilegeDlg;		// �����Ȩ�޶Ի���
};