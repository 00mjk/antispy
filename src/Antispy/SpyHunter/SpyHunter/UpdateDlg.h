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
#include "afxwin.h"
#include"resource.h"

// CUpdateDlg �Ի���

class CUpdateDlg : public CDialog
{
	DECLARE_DYNAMIC(CUpdateDlg)

public:
	CUpdateDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUpdateDlg();

// �Ի�������
	enum { IDD = IDD_UPDATE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_Progress;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	BOOL CheckForUpdates();
	afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);  
	void ProgressUpdate( ULONG nMaxBytes, ULONG nDoneBytes );
	BOOL OnDownLoad();
	BOOL StartUnpack(CString szPath);
	CString m_szUpdateLogEdit;  // �ܵ�������û�����Ϣ
	CButton m_btnUpdate;    
	BOOL m_bUpdate;			// �Ƿ���Ҫ����
	CString m_szUpdateURL;	// ���°����ص�ַ
	CString m_szVersion;	// ���°汾��Ϣ
	CString m_szUpdateLog;	// ������־
	CEdit m_OutputEdit;	
	CString m_szRarPath;	// ���°汾�ļ��������������·����һ�������ص���ʱ�ļ���
	CString m_szNewVersionPath;
	BOOL m_bStopDownload;
	afx_msg void OnClose();
	HANDLE m_hDownloadThread;
	HANDLE m_hConnectThread;
};
