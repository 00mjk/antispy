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

// CObjectHookDlg �Ի���

class CObjectHookDlg : public CDialog
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CObjectHookDlg)

public:
	CObjectHookDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CObjectHookDlg();

// �Ի�������
	enum { IDD = IDD_OBJECT_HOOK_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	virtual BOOL OnInitDialog();
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult);
	void GetDriver();
	CString GetDriverPath(ULONG pCallback);
	void GetObjectHook();
	void InsertObjectHookItem(OBJECT_HOOK_INOF HookInfo);
	CString GetObjectName(OBJECT_HOOK_INOF HookInfo);
	void GetObjectTypes();
	CString GetObjectName2(ULONG Object);
	void GetSelfCreateTypeHookInfo();
	void InsertSelfCreateObjectHookItem(POBJECT_HOOK_INOF pHookInfo);
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

public:
	CString m_szStatus;
	CSortListCtrl m_list;
	vector<OBJECT_HOOK_INOF> m_DispatchVector;
	CConnectDriver m_Driver;
	CommonFunctions m_Functions;
	vector<DRIVER_INFO> m_CommonDriverList;
	vector<OBJECT_TYPE_INFO> m_TypeVector;
	BOOL m_bOnlyShowHooks;
	ULONG m_nHookedCnt;
	ULONG m_nTotalCnt;
	CListDrivers m_clsDrivers;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnProcessLocationAtFileManager();
};
