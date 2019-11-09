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
// RenameKeyDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SpyHunter.h"
#include "RenameKeyDlg.h"


// CRenameKeyDlg �Ի���

IMPLEMENT_DYNAMIC(CRenameKeyDlg, CDialog)

CRenameKeyDlg::CRenameKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRenameKeyDlg::IDD, pParent)
	, m_KeyNameStatic(_T(""))
	, m_szKeyName(_T(""))
{
	m_nDlgType = eCreateKey;
}

CRenameKeyDlg::~CRenameKeyDlg()
{
}

void CRenameKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_KEY_NAME, m_KeyNameStatic);
	DDX_Text(pDX, IDC_EDIT, m_szKeyName);
}


BEGIN_MESSAGE_MAP(CRenameKeyDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRenameKeyDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRenameKeyDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CRenameKeyDlg ��Ϣ�������

BOOL CRenameKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	switch (m_nDlgType)
	{
	case eRenameKey:
		SetWindowText(szRenameKey[g_enumLang]);
		m_KeyNameStatic = szKeyName[g_enumLang];
		break;

	case eRenameValue:
		SetWindowText(szRenameValue[g_enumLang]);
		m_KeyNameStatic = szValueName[g_enumLang];
		break;

	case eCreateKey:
		SetWindowText(szNewKey[g_enumLang]);
		m_KeyNameStatic = szKeyName[g_enumLang];
		break;

	case eSetValueKey:
		SetWindowText(szNewValue[g_enumLang]);
		m_KeyNameStatic = szValueName[g_enumLang];
		break;
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CRenameKeyDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	OnOK();
}

void CRenameKeyDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}
