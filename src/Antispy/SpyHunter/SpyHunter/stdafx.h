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
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �� Windows ͷ���ų�����ʹ�õ�����
#endif

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ��������Ϊ�ʺ� Windows Me ����߰汾����Ӧֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0600	// ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��ֵ��
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxdlgs.h>

///////////////////////////////////////////////////////////////////////

#include "Config.h"
#include <shlwapi.h>
#include "common.h"
#include "ConnectDriver.h"

typedef enum WIN_VERSION {
	enumWINDOWS_UNKNOW,
	enumWINDOWS_2K,
	enumWINDOWS_XP,
	enumWINDOWS_2K3,
	enumWINDOWS_2K3_SP1_SP2,
	enumWINDOWS_VISTA,
	enumWINDOWS_VISTA_SP1_SP2,
	enumWINDOWS_7,
	enumWINDOWS_8
} WIN_VERSION;

extern WIN_VERSION g_WinVersion;
extern CConnectDriver g_ConnectDriver;
extern CTabCtrl *g_pTab;
extern CWnd *g_pRegistryDlg;
extern CWnd *g_pFileDlg;
extern CWnd *g_pServiceDlg;
extern WCHAR szWin32DriverName[32];
extern CString g_szVersion;
extern BOOL g_bLoadDriverOK;
extern CConfig g_Config;
extern LIST_ENTRY *g_pLdrpHashTable;
extern CWnd *g_pSetConfigDlg;			// ���������ļ��ĶԻ���
extern CWnd *g_pMainDlg;
extern CWnd *g_pProcessDlg;

extern COLORREF g_NormalItemClr;
extern COLORREF g_HiddenOrHookItemClr;
extern COLORREF g_MicrosoftItemClr;	
extern COLORREF g_NotSignedItemClr;

extern CBitmap m_bmExplorer;						
extern CBitmap m_bmRefresh;
extern CBitmap m_bmDelete;
extern CBitmap m_bmSearch;
extern CBitmap m_bmExport;
extern CBitmap m_bmShuxing;
extern CBitmap m_bmCopy;
extern CBitmap m_bmAntiSpy;
extern CBitmap m_bmDetalInof;
extern CBitmap m_bmLookFor;
extern CBitmap m_bmWindbg;
extern CBitmap m_bmSign;
extern CBitmap m_bmReg;
extern CBitmap m_bmRecover;
extern CBitmap m_bmOD;

extern pfnNtQueryInformationProcess NtQueryInformationProcess;

#ifdef _DEBUG
#define _TRACK
#endif

#define _OUTPUT_STRHEADER	_T("AntiSpy: ")
#define _0X_LEN				wcslen(L"0x")
#define	_0X08X				L"0x%08X"

#define _CSTDIO_
#define _CSTRING_
#define _CWCHAR_

#pragma   warning(disable:4311)
#pragma   warning(disable:4312)
#pragma   warning(disable:4267)

#include "EasySize.h"
#include "String.h"
#include "CommonMacro.h"
#include "sortlistctrl.h"

// #define _XTP_EXCLUDE_CALENDAR
// #define _XTP_EXCLUDE_SYNTAXEDIT
// #define _XTP_EXCLUDE_REPORTCONTROL
// 

///////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#include <XTToolkitPro.h>   // Codejock Software Components