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
// SpyHunter.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "ExceptionManager.h"

// CSpyHunterApp:
// �йش����ʵ�֣������ SpyHunter.cpp
//

class CSpyHunterApp : public CWinApp
{
public:
	CSpyHunterApp();

// ��д
	public:
	virtual BOOL InitInstance();
	
// ʵ��
private:
	CExceptionManager m_ExceptionManager;
	DECLARE_MESSAGE_MAP()
	LANGUAGE_TYPE GetLanguageID();
	HANDLE m_hMutex;
public:
	virtual int ExitInstance();
};

extern CSpyHunterApp theApp;