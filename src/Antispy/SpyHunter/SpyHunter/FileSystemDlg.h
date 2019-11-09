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
#include <vector>
#include "ListDriver.h"
using namespace std;


// CFileSystemDlg �Ի���

class CFileSystemDlg : public CDialog
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CFileSystemDlg)

public:
	CFileSystemDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFileSystemDlg();

// �Ի�������
	enum { IDD = IDD_FILE_SYSTEM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CSortListCtrl m_list;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult);
	void GetFsdDispatch();
	CString GetDriverPath(ULONG pCallback);
	void GetDriver();
	void InsertFsdItem(DISPATCH_HOOK_INFO Info, FILE_SYSTEM_TYPE Type);
	vector<DISPATCH_HOOK_INFO> m_NtfsHookVector;
	vector<DISPATCH_HOOK_INFO> m_FastfatHookVector;
	CConnectDriver m_Driver;
	CommonFunctions m_Functions;
	vector<DRIVER_INFO> m_CommonDriverList;
	BOOL m_bOnlyShowHooks;
	ULONG m_nHookedCnt;
	CString m_szStatus;
	CListDrivers m_clsDrivers;
	afx_msg void OnSdtRefresh();
	afx_msg void OnSdtOnlyShowHook();
	afx_msg void OnUpdateSdtOnlyShowHook(CCmdUI *pCmdUI);
	afx_msg void OnSdtDisCurrent();
	afx_msg void OnSdtDisOrigin();
	afx_msg void OnSdtRestore();
	afx_msg void OnSdtRestoreAll();
	afx_msg void OnSdtShuxing();
	afx_msg void OnSdtLocationModule();
	afx_msg void OnSdtVerifySign();
	afx_msg void OnSdtText();
	afx_msg void OnSdtExcel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnProcessLocationAtFileManager();
};
