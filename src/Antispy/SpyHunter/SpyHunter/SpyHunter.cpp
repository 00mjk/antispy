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
// SpyHunter.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "SpyHunter.h"
#include "SpyHunterDlg.h"
// #include <vld.h>
// #pragma comment(lib, "vld.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpyHunterApp

BEGIN_MESSAGE_MAP(CSpyHunterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSpyHunterApp ����

CSpyHunterApp::CSpyHunterApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSpyHunterApp ����

CSpyHunterApp theApp;


// CSpyHunterApp ��ʼ��

BOOL CSpyHunterApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

//	CoInitialize(NULL);

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
// 	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	// ��ȡ�����ļ�
	g_Config.ReadConfig();
	
	g_NormalItemClr = g_Config.GetNormalModuleColor();
	g_HiddenOrHookItemClr = g_Config.GetHiddenOrHooksModuleColor();
	g_MicrosoftItemClr = g_Config.GetMicrosoftModuleColor(); 	
	g_NotSignedItemClr = g_Config.GetNoSignatureModuleColor();

	// ��ȡϵͳ����
	g_enumLang = GetLanguageID();

	// �����ļ�������������,ֻ����һ��ʵ������
	WCHAR szPath[MAX_PATH * 2] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH * 2);
	if (wcslen(szPath))
	{
		CString strPath = szPath;
		CString szImage = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\') - 1);
		if (!szImage.IsEmpty())
		{
			m_hMutex = CreateMutex(NULL, TRUE, szImage);
			if (m_hMutex && GetLastError() == ERROR_ALREADY_EXISTS) 
			{
				MessageBox(NULL, szAlreadyRunning[g_enumLang], L"AntiSpy", MB_OK | MB_ICONWARNING);
				return FALSE; //�˳���������
			} 
		}
	}

	// �����쳣������
	m_ExceptionManager.SetUnHandleException();

	// ��ʾ������
	CSpyHunterDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//  �Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

LANGUAGE_TYPE CSpyHunterApp::GetLanguageID()
{
//	return enumEnglish; 
	LANGUAGE_TYPE languageId = enumEnglish;
	LCID nLangID = GetUserDefaultUILanguage();
	switch (nLangID)
	{
	case 0x0409:		//LANG_ENGLISH  SUBLANG_ENGLISH_US
	case 0x0C09:		//SUBLANG_ENGLISH_AUS
	case 0x2809:		//SUBLANG_ENGLISH_BELIZE
	case 0x1009:		//SUBLANG_ENGLISH_CAN
	case 0x2409:		//SUBLANG_ENGLISH_CARIBBEAN
	case 0x4009:		 //SUBLANG_ENGLISH_INDIA
	case 0x1809:		//SUBLANG_ENGLISH_EIRE
	case 0x2009:		//SUBLANG_ENGLISH_JAMAICA
	case 0x4409:		//SUBLANG_ENGLISH_MALAYSIA
	case 0x1409:		//SUBLANG_ENGLISH_NZ
	case 0x3409:		//SUBLANG_ENGLISH_PHILIPPINES
	case 0x4809:		//SUBLANG_ENGLISH_SINGAPORE
	case 0x1c09:		//SUBLANG_ENGLISH_SOUTH_AFRICA
	case 0x2C09:		//SUBLANG_ENGLISH_TRINIDAD
	case 0x0809:		//SUBLANG_ENGLISH_UK
	case 0x3009:		//SUBLANG_ENGLISH_ZIMBABWE
		languageId = enumEnglish;
		break;

	case 0x0C04:		//SUBLANG_CHINESE_HONGKONG
	case 0x1404:		//SUBLANG_CHINESE_MACAU
	case 0x1004:		//SUBLANG_CHINESE_SINGAPORE
	case 0x0804:		//SUBLANG_CHINESE_SIMPLIFIED
	case 0x0404:		//SUBLANG_CHINESE_TRADITIONAL
		languageId = enumChinese;
		break;

	default :
		languageId = enumEnglish;
		break;
	}

	return languageId;
}

int CSpyHunterApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	g_Config.WriteConfig();

	return CWinApp::ExitInstance();
}