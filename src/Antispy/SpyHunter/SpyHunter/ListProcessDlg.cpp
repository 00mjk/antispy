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
// ListProcessDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SpyHunter.h"
#include "ListProcessDlg.h"
#define STRSAFE_NO_DEPRECATE 
#include <strsafe.h>
#include <tlhelp32.h>
#include <algorithm>
#include "DllModuleDlg.h"
#include "ThreadDlg.h"
#include "HandleDlg.h"
#include "Function.h"
#include "ProcessMemoryDlg.h"
#include "ProcessWndDlg.h"
#include "ProcessTimerDlg.h"
#include "HotKeyDlg.h"
#include "ProcessPrivilegesDlg.h"
#include <shlwapi.h>
#include "FindModuleDlg.h"
#include "NotSignModuleDlg.h"
#include "ProcessInfoDlg.h"
#include "ProcessHookDlg.h"
#include <DbgHelp.h>
#include "SetConfigDlg.h"
#include "FindWindow.h"

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "Version.lib")

typedef enum _PROCESS_HEADER_INDEX_
{
	eProcessImageName,
	eProcessPid,
	eProcessParentPid,
	eProcessPath,
//	eProcessEprocess,
	eProcessR3Access,
	eProcessFileCompany,
	eProcessFileDescription,
	eProcessCreateTime,
}PROCESS_HEADER_INDEX;

typedef enum _PROCESS_TREE_HEADER_INDEX_
{
	eProcessTreeImageName,
	eProcessTreePid,
//	eProcessParentPid,
	eProcessTreePath,
//	eProcessEprocess,
	eProcessTreeR3Access,
	eProcessTreeFileCompany,
	eProcessTreeFileDescription,
	eProcessTreeCreateTime,
}PROCESS_TREE_HEADER_INDEX;

////////////////////////////////////////////////////////////////////

// CListProcessDlg �Ի���

IMPLEMENT_DYNAMIC(CListProcessDlg, CDialog)

CListProcessDlg::CListProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CListProcessDlg::IDD, pParent)
	, m_szProcessCnt(_T(""))
{
	m_bShowAsTreeList = FALSE;
	m_nProcessCnt = m_nHideCnt = m_nDenyAccessCnt = 0;
	m_bShowTipWindow = TRUE;
	m_clrHideProc = RGB( 255, 0, 0 );					// ���ؽ���Ĭ���� ��ɫ
	m_clrMicrosoftProc = RGB(0, 0, 0);					// ΢�����Ĭ���� ��ɫ
	m_clrNormalProc = RGB( 0, 0, 255 );					// �����Ľ���Ĭ���� ��ɫ
	m_clrMicroHaveOtherModuleProc = RGB( 255, 140, 0 );	// ΢����з�΢��ģ��Ĭ���� �ٻ�ɫ
	m_clrNoSigniture = RGB( 180, 0, 255 );				// Ĭ������ɫ
	m_bShowBelow = TRUE;								// ����б�ͷ��ʱ����ͣ��ʾ�·�����
	m_nBelowDialog = 0;									// �ܹ��м�������
	m_nCurSel = eDllModuleDlg;							// ����Ĵ�������ѡ������
}

CListProcessDlg::~CListProcessDlg()
{
	m_Ring0ProcessList.clear();
	m_Ring3ProcessList.clear();
}

void CListProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROCESS_CNT, m_szProcessCnt);
	DDX_Control(pDX, IDC_TAB, m_tab);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_processList);
	DDX_Control(pDX, IDC_PROCESS_TREE, m_processTree);
}


BEGIN_MESSAGE_MAP(CListProcessDlg, CDialog)
	ON_MESSAGE(WM_RESIZE_ALL_PROC_WND, ResizeAllProcWnd)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_PROCESS, &CListProcessDlg::OnNMCustomdrawProcessList) // list�ؼ�����ɫ����
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, &CListProcessDlg::OnNMRclickProcessList)			// list�ؼ����Ҽ��˵�
	ON_COMMAND(ID_PROCESS_REFRESH, &CListProcessDlg::OnProcessRefresh)
	ON_COMMAND(ID_CHECK_SIGN, &CListProcessDlg::OnCheckSign)
	ON_COMMAND(ID_CHECK_ALL_SIGN, &CListProcessDlg::OnCheckAllSign)
	ON_COMMAND(ID_KILL_PROCESS, &CListProcessDlg::OnKillProcess)
	ON_COMMAND(ID_KILL_DELETE, &CListProcessDlg::OnKillDelete)
	ON_COMMAND(ID_SUSPEND_PROCESS, &CListProcessDlg::OnSuspendProcess)
	ON_COMMAND(ID_RESUME_PROCESS, &CListProcessDlg::OnResumeProcess)
	ON_COMMAND(ID_COPY_PROCESS_NAME, &CListProcessDlg::OnCopyProcessName)
	ON_COMMAND(ID_COPY_PROCESS_PATH, &CListProcessDlg::OnCopyProcessPath)
	ON_COMMAND(ID_CHECK_ATTRIBUTE, &CListProcessDlg::OnCheckAttribute)
	ON_COMMAND(ID_LOCATION_EXPLORER, &CListProcessDlg::OnLocationExplorer)
	ON_COMMAND(ID_SEARCH_ONLINE, &CListProcessDlg::OnSearchOnline)
	ON_WM_SIZE()
	ON_COMMAND(ID_SHOW_ALL_PROCESS_INFO, &CListProcessDlg::OnShowAllProcessInfo)
	ON_MESSAGE(WM_VERIFY_SIGN_OVER, VerifySignOver)
	ON_MESSAGE(WM_CLICK_LIST_HEADER_START, OnClickListHeaderStart)
	ON_MESSAGE(WM_CLICK_LIST_HEADER_END, OnClickListHeaderEnd)
	ON_MESSAGE(WM_ENUM_TIMER, MsgEnumTimer)
	ON_MESSAGE(WM_ENUM_MODULES, MsgEnumModules)
	ON_MESSAGE(WM_ENUM_HANDLES, MsgEnumHandles)
	ON_MESSAGE(WM_ENUM_THREADS, MsgEnumThreads)
	ON_MESSAGE(WM_ENUM_WINDOWS, MsgEnumWindows)
	ON_MESSAGE(WM_ENUM_HOTKEYS, MsgEnumHotKeys)
	ON_MESSAGE(WM_ENUM_MEMORY, MsgEnumMemory)
	ON_MESSAGE(WM_ENUM_PRIVILEGES, MsgEnumPrivileges)
	ON_COMMAND(ID_EXPORT_EXCEL, &CListProcessDlg::OnExportExcel)
	ON_COMMAND(ID_EXPORT_TEXT, &CListProcessDlg::OnExportText)
	ON_COMMAND(ID_LISTPROCESS_DUMP, &CListProcessDlg::OnDumpProcessMemory)
	ON_COMMAND(ID_PROCESS_FIND_MODULE, &CListProcessDlg::OnProcessFindModule)
	ON_COMMAND(ID_PROCESS_FIND_UNSIGNED_MODUES, &CListProcessDlg::OnProcessFindUnsignedModues)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROCESS, &CListProcessDlg::OnNMDblclkProcessList)			// �����б�˫����ʾ�������� 
	ON_COMMAND(ID_PROCESS_HOOK, &CListProcessDlg::OnProcessHook)
	ON_COMMAND(ID_CREATE_MINIDUMP, &CListProcessDlg::OnCreateMinidump)
	ON_COMMAND(ID_CREATE_FULLDUMP, &CListProcessDlg::OnCreateFulldump)
	ON_NOTIFY(NM_RCLICK, IDC_COLUMNTREE, &CListProcessDlg::OnRclickedColumntree)
	ON_NOTIFY(NM_RCLICK, IDC_PROCESS_TREE, &CListProcessDlg::OnRclickedColumntree)			// �������ؼ����Ҽ��˵�
	ON_COMMAND(ID_SHOW_PROCESS_TREE, &CListProcessDlg::OnShowProcessTree)
	ON_UPDATE_COMMAND_UI(ID_SHOW_PROCESS_TREE, &CListProcessDlg::OnUpdateShowProcessTree)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_KILL_PROCESS_TREE, &CListProcessDlg::OnKillProcessTree)
	ON_COMMAND(ID_INJECT_DLL, &CListProcessDlg::OnInjectDll)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROCESS, &CListProcessDlg::OnLvnItemchangedProcessList)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROCESS_TREE, &CListProcessDlg::OnTvnSelchangedTreeList) // �������οؼ��ı���ѡ��
	ON_NOTIFY(NM_CLICK, IDC_PROCESS_TREE, &CListProcessDlg::OnNMClickTreeList) // �������οؼ��ı���ѡ��
	ON_COMMAND(ID_PROCESS_LOCATION_AT_FILE_MANAGER, &CListProcessDlg::OnProcessLocationAtFileManager)
	ON_MESSAGE(WM_UPDATE_PROCESS_DATA, OnUpdateProcessInfoStatus)
	ON_COMMAND(ID_LOOK_WINDOW_FOR_PROCESS, &CListProcessDlg::OnLookWindowForProcess)
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CListProcessDlg::OnTcnSelchangeTab)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PROCESS, &CListProcessDlg::OnNMClickListProcess)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CListProcessDlg)
  	EASYSIZE(IDC_PROCESS_CNT, ES_BORDER, ES_KEEPSIZE, ES_BORDER, ES_BORDER, 0)
 	EASYSIZE(IDC_TAB, ES_BORDER, ES_KEEPSIZE, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_X_SP, ES_BORDER, ES_KEEPSIZE, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_LIST_PROCESS, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_PROCESS_TREE, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP

// CListProcessDlg ��Ϣ�������

// 
// ��ʼ��������Ϣ����ı�ǩ����
// 
void CListProcessDlg::InitProcInfoWndTab(CDialog *pDlg)
{
	if (!pDlg)
	{
		return;
	}

// 	CRect rc;
// 	m_tab.GetClientRect(rc);
// 	rc.top += 20;
// 	rc.bottom -= 2;
// 	rc.left += 0;
// 	rc.right -= 2;

	CRect rc;
	m_tab.GetClientRect(rc);
	rc.top += 22;
	rc.bottom -= 3;
	rc.left += 2;
	rc.right -= 3;

	pDlg->MoveWindow(&rc);

	// �ѶԻ������ָ�뱣������
	m_pBelowDialog[m_nBelowDialog] = pDlg;

	// ��ʾ��ʼҳ��
	if (m_nBelowDialog == 0)
	{
		m_pBelowDialog[m_nBelowDialog]->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pBelowDialog[m_nBelowDialog]->ShowWindow(SW_HIDE);
	}

	m_nBelowDialog++;
}

//
// ��ʼ���Ի�������
//
BOOL CListProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_ySplitter.BindWithControl(this, IDC_X_SP);
	m_ySplitter.SetMinHeight(100, 50);

	// ��ʼ�������б�
 	m_ProImageList.Create(16, 16, ILC_COLOR32|ILC_MASK, 2, 2); 
	m_processList.SetImageList (&m_ProImageList, LVSIL_SMALL);
	m_processList.SetExtendedStyle(LVS_EX_FULLROWSELECT/* | LVS_EX_GRIDLINES*/);
	m_processList.InsertColumn(eProcessImageName, szImageName[g_enumLang], LVCFMT_LEFT, 130);
	m_processList.InsertColumn(eProcessPid, szPid[g_enumLang], LVCFMT_LEFT, 65);
	m_processList.InsertColumn(eProcessParentPid, szParentPid[g_enumLang], LVCFMT_LEFT, 65);
	m_processList.InsertColumn(eProcessPath, szImagePath[g_enumLang], LVCFMT_LEFT, 290);
// 	m_processList.InsertColumn(eProcessEprocess, szProcessObject[g_enumLang], LVCFMT_LEFT, 80);
	m_processList.InsertColumn(eProcessR3Access, szR3Access[g_enumLang], LVCFMT_CENTER, 75);
	m_processList.InsertColumn(eProcessFileCompany, szFileCorporation[g_enumLang], LVCFMT_LEFT, 140);
	m_processList.InsertColumn(eProcessFileDescription, szFileDescriptionNew[g_enumLang], LVCFMT_LEFT, 190);
	m_processList.InsertColumn(eProcessCreateTime, szProcessStartedTime[g_enumLang], LVCFMT_LEFT, 140);

	// ��ʼ����ݼ����
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

	// ��ʼ��Ĭ�ϵĽ���ͼ��
	m_hProcessIcon = LoadIcon(NULL, IDI_APPLICATION);

	// ��ʼ�������б�ؼ�
	m_imgList.Create(16, 16, ILC_COLOR32|ILC_MASK, 2, 2); 
	UINT uTreeStyle = TVS_HASBUTTONS | /*TVS_HASLINES|*/ TVS_LINESATROOT |/* TVS_CHECKBOXES |*/ TVS_FULLROWSELECT;
	
	// ��ʼ����ģ��Ľ������οؼ�
	m_processTree.GetTreeCtrl().SetImageList(&m_imgList, TVSIL_NORMAL);
	m_processTree.GetTreeCtrl().ModifyStyle(0, uTreeStyle);
	m_processTree.SetFirstColumnMinWidth(170);
	m_processTree.InsertColumn(eProcessTreeImageName, szImageName[g_enumLang], LVCFMT_LEFT, 220);
	m_processTree.InsertColumn(eProcessTreePid, szPid[g_enumLang], LVCFMT_LEFT, 65);
//	m_processTree.InsertColumn(eProcessParentPid, szParentPid[g_enumLang], LVCFMT_LEFT, 65);
	m_processTree.InsertColumn(eProcessTreePath, szImagePath[g_enumLang], LVCFMT_LEFT, 260);
// 	m_processTree.InsertColumn(eProcessEprocess, szProcessObject[g_enumLang], LVCFMT_LEFT, 85);
	m_processTree.InsertColumn(eProcessTreeR3Access, szR3Access[g_enumLang], LVCFMT_CENTER, 75);
	m_processTree.InsertColumn(eProcessTreeFileCompany, szFileCorporation[g_enumLang], LVCFMT_LEFT, 150);
	m_processTree.InsertColumn(eProcessTreeFileDescription, szFileDescriptionNew[g_enumLang], LVCFMT_LEFT, 170);
	m_processTree.InsertColumn(eProcessTreeCreateTime, szProcessStartedTime[g_enumLang], LVCFMT_LEFT, 150);
	
	m_bShowAsTreeList = g_Config.GetShowProcessTreeAsDefault();
	AdjustShowWindow();

	// ��ʼ��tip��ʾ��
	m_tipWindow.Create(this);
	m_tipWindow.ShowWindow(SW_HIDE);
	m_bShowTipWindow = g_Config.GetShowProcessDetailInfo();

	// ���ò�ͬ������ʾ����ɫ
	m_clrHideProc = g_Config.GetHideProcColor();
	m_clrMicrosoftProc = g_Config.GetMicrosoftProcColor();
	m_clrNormalProc = g_Config.GetNormalProcColor();
	m_clrMicroHaveOtherModuleProc = g_Config.GetMicroHaveOtherModuleProcColor();
	m_clrNoSigniture = g_Config.GetNoSignatureModuleColor();
	
	m_processTree.GetTreeCtrl().SetItemColor(
		m_clrNormalProc,
		m_clrHideProc,
		m_clrMicrosoftProc,
		m_clrMicroHaveOtherModuleProc);

	m_tab.InsertItem(eDllModuleDlg, szModuleInfo[g_enumLang]);
	m_tab.InsertItem(eHandleDlg, szHandleInfo[g_enumLang]);
	m_tab.InsertItem(eThreadDlg, szThreadInfo[g_enumLang]);
	m_tab.InsertItem(eMemoryDlg, szMemoryInfo[g_enumLang]);
	m_tab.InsertItem(eWndDlg, szWindowInfo[g_enumLang]);
	m_tab.InsertItem(eHotKeyDlg, szHotKeyInfo[g_enumLang]);
	m_tab.InsertItem(eTimerDlg, szTimerInfo[g_enumLang]);
	m_tab.InsertItem(ePrivilegeDlg, szPrivilegeInfo[g_enumLang]);

	m_ModuleDlg.Create(IDD_DLL_MODULE_DIALOG, &m_tab);
	m_HandleDlg.Create(IDD_HANDLE_DIALOG, &m_tab);
	m_ThreadDlg.Create(IDD_THREAD_DIALOG, &m_tab);
	m_MemoryDlg.Create(IDD_MEMORY_DIALOG, &m_tab);
	m_WndDlg.Create(IDD_PROCESS_WND_DIALOG, &m_tab);
	m_HotKeyDlg.Create(IDD_HOTKEY_DIALOG, &m_tab);
	m_TimerDlg.Create(IDD_PROCESS_TIMER_DIALOG, &m_tab);
	m_PrivilegeDlg.Create(IDD_PROCESS_PRIVILEGES_DIALOG, &m_tab);

	InitProcInfoWndTab(&m_ModuleDlg);
	InitProcInfoWndTab(&m_HandleDlg);
	InitProcInfoWndTab(&m_ThreadDlg);
	InitProcInfoWndTab(&m_MemoryDlg);
	InitProcInfoWndTab(&m_WndDlg);
	InitProcInfoWndTab(&m_HotKeyDlg);
	InitProcInfoWndTab(&m_TimerDlg);
	InitProcInfoWndTab(&m_PrivilegeDlg);

	ListProcess();
	
	g_pProcessDlg = this;

	INIT_EASYSIZE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//
// ring3 ʹ�ó��淽��ö�ٽ���
//
BOOL CListProcessDlg::GetRing3ProcessList()
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	BOOL bRet = FALSE;

	// �������������
	m_Ring3ProcessList.clear();

	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap != INVALID_HANDLE_VALUE )
	{
		pe32.dwSize = sizeof( PROCESSENTRY32 );

		if( Process32First( hProcessSnap, &pe32 ) )
		{
			do
			{
				m_Ring3ProcessList.push_back(pe32.th32ProcessID);
			} while( Process32Next( hProcessSnap, &pe32 ) );

			bRet = TRUE;
		}

		CloseHandle( hProcessSnap );
	}

	return bRet;
}

//
// ring0��pid��ring3�ĶԱȣ��ó��Ƿ������ؽ���
//
BOOL CListProcessDlg::IsProcessHide(ULONG pid)
{
	BOOL bHide = FALSE;

	if (!m_Ring3ProcessList.empty())
	{
		list<ULONG>::iterator findItr;
		findItr = find(m_Ring3ProcessList.begin(), m_Ring3ProcessList.end(), pid);
		if (findItr == m_Ring3ProcessList.end())
		{
			bHide = TRUE;
		}
	}

	return bHide;
}

//
// �Խ�������Ȩ�޴򿪣����Ƿ�ɹ�
//
BOOL CListProcessDlg::CanOpenProcess(DWORD dwPid)
{
	BOOL bOpen = TRUE;

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_VM_OPERATION, TRUE, dwPid);
	if (hProcess)
	{
		CloseHandle(hProcess);
	}
	else
	{
		bOpen = FALSE;
	}

	return bOpen;
}

//
// ����Snapshot�ķ���(����)���ж��Ƿ��з�΢���ģ��
//
BOOL CListProcessDlg::IsHaveNotMicrosoftModule(DWORD dwPID) 
{ 
	BOOL bRet    =    FALSE; 
	HANDLE hModuleSnap = NULL; 
	MODULEENTRY32 me32 = {0}; 

	hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if(hModuleSnap == INVALID_HANDLE_VALUE || dwPID == 0) 
	{    
		return FALSE; 
	} 

	me32.dwSize = sizeof(MODULEENTRY32); 
	if(::Module32First(hModuleSnap, &me32))
	{ 
		do{ 

			if (!m_Functions.IsMicrosoftAppByPath(m_Functions.TrimPath(me32.szExePath)))
			{
				bRet = TRUE;
				break;
			}

		}while(::Module32Next(hModuleSnap, &me32)); 
	}

	CloseHandle(hModuleSnap); 
	return bRet; 
} 

//
// ��ӽ���
//
void CListProcessDlg::AddProcessItem(PROCESS_INFO item)
{
	if (item.ulPid == 0 && item.ulParentPid == 0 && item.ulEprocess == 0)
	{
		return;
	}

	CString strPid, strParentPid, strEprocess, strCompany, strDescription, strR3Access, szDescription, szStartedTime;

	// �������·����pid
	CString strPath = TrimPath(item.szPath);
	CString strImageName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\') - 1);
	item.ulParentPid ? strParentPid.Format(L"%d", item.ulParentPid) : strParentPid.Format(L"-");

	// �������ͼ��
	SHFILEINFO sfi;
	DWORD_PTR nRet;
	HICON hIcon = NULL;

	ZeroMemory(&sfi, sizeof(SHFILEINFO));

	nRet = SHGetFileInfo(strPath, 
		FILE_ATTRIBUTE_NORMAL, 
		&sfi, 
		sizeof(SHFILEINFO), 
		SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES
		);

	int nIcon = -1;
	if (nRet)
	{
		nIcon = m_ProImageList.Add ( sfi.hIcon );
		DestroyIcon(sfi.hIcon);
	}
	else
	{
		nIcon = m_ProImageList.Add ( m_hProcessIcon );
	}

	if (CanOpenProcess(item.ulPid))
	{
		strR3Access = L"-";
	}
	else
	{
		strR3Access = szDeny[g_enumLang];
		m_nDenyAccessCnt++;
	}

	strEprocess.Format(L"0x%08X", item.ulEprocess);
	strPid.Format(L"%d", item.ulPid);
	strCompany = m_Functions.GetFileCompanyName(strPath);
	szDescription = m_Functions.GetFileDescription(strPath);
	szStartedTime = FileTime2SystemTime(item.ulCreateTime);

	int nCnt = m_processList.GetItemCount();
	int i = m_processList.InsertItem(nCnt, strImageName, nIcon);
	m_processList.SetItemText(i, eProcessPid, strPid);
	m_processList.SetItemText(i, eProcessParentPid, strParentPid);
	m_processList.SetItemText(i, eProcessPath, strPath);
//	m_processList.SetItemText(i, eProcessEprocess, strEprocess);
	m_processList.SetItemText(i, eProcessR3Access, strR3Access);
	m_processList.SetItemText(i, eProcessFileCompany, strCompany);
	m_processList.SetItemText(i, eProcessFileDescription, szDescription);
	m_processList.SetItemText(i, eProcessCreateTime, szStartedTime);
	
	// Ĭ�Ϻ�ɫ
	ITEM_COLOR_INFO clrInfo;
	clrInfo.textClr = enumBlack;

	// ��������صģ���ô��ʾ��ɫ
	if (IsProcessHide(item.ulPid))
	{
		m_nHideCnt++;
		clrInfo.textClr = enumRed;
	}

	// �������΢���ģ�飬��ô����ʾ��ɫ
	else if (!(m_Functions.IsMicrosoftApp(strCompany) || 
		!strPath.CompareNoCase( L"System" ) || 
		!strPath.CompareNoCase(L"Idle")))
	{
		clrInfo.textClr = enumBlue;
	}

	// ���΢����̱�ע��������ģ�飬��ô��ʾ����ɫ
	if (clrInfo.textClr == enumBlack && IsHaveNotMicrosoftModule(item.ulPid))
	{
		clrInfo.textClr = enumTuhuang;
	}
	
	m_vectorColor.push_back(clrInfo);
	m_vectorProcesses.push_back(item);

	m_processList.SetItemData(i, nCnt);

	m_nProcessCnt++;
}

//
// ��List����ʽչʾ����
//
void CListProcessDlg::ShowProcessAsList()
{
	m_szProcessCnt.Format(szProcessState[g_enumLang], 0, 0, 0);
	SendMessage(WM_UPDATE_PROCESS_DATA);

	// ���image�б������б���ʼ��Ĭ��ͼ��
	m_processList.DeleteAllItems();
	m_vectorColor.clear();
	m_vectorProcesses.clear();

	int nImageCount = m_ProImageList.GetImageCount();
	for(int j = 0; j < nImageCount; j++)   
	{   
		m_ProImageList.Remove(0);
	}

	// ��ʼ����
	m_nDenyAccessCnt = m_nProcessCnt = m_nHideCnt = 0;
	
	// �ں˻�ȡϵͳȫ������
	m_clsProcess.EnumProcess(m_Ring0ProcessList);
	if (m_Ring0ProcessList.empty())
	{
		return;
	}

	// ��CreateToolhelp32Snapshot�ķ�����ȡӦ�ò�Ľ���
	GetRing3ProcessList();
	
	// ��������ʱ������
	m_clsProcess.SortByProcessCreateTime(m_Ring0ProcessList);
	
	for ( vector <PROCESS_INFO>::iterator Iter = m_Ring0ProcessList.begin( ); 
		Iter != m_Ring0ProcessList.end( ); 
		Iter++)
	{
		AddProcessItem(*Iter);
	}

	m_szProcessCnt.Format(szProcessState[g_enumLang], m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);
	SendMessage(WM_UPDATE_PROCESS_DATA);
}

BOOL UDgreaterEx( PROCESS_INFO elem1, PROCESS_INFO elem2 )
{
	return elem1.ulParentPid < elem2.ulParentPid;
}

//
// ������ģʽ�£����ݸ�����ID�Ÿ���
//
VOID CListProcessDlg::SortByParentPid(vector<PROCESS_INFO> &ProcessListEx)
{
	sort( ProcessListEx.begin( ), ProcessListEx.end( ), UDgreaterEx );
}

//
// �ж�һ�������Ƿ��и����̣���Ҫ������������������IDһ�������Ҵ���ʱ���Ⱥ�Ҫһ��
//
BOOL CListProcessDlg::IsHaveParent(PROCESS_INFO_EX info)
{
	BOOL bRet = FALSE;

	// ���������PID��0����û�и�����
	if (info.ulParentPid == 0)
	{
		return bRet;
	}
	
	// ���������б��Ƿ񸸽��̻���
	for ( vector<PROCESS_INFO_EX>::iterator Iter = m_ProcessListEx.begin( ); 
		Iter != m_ProcessListEx.end( ); 
		Iter++)
	{
		// ����IDһ�������Ҵ���ʱ���Ⱥ�Ҫһ��
		if (info.ulParentPid == Iter->ulPid && info.nCreateTime > Iter->nCreateTime)
		{
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

//
// ��������
//
void CListProcessDlg::InsertTreeProcessChildItem(PROCESS_INFO_EX* pItem, HTREEITEM hRoot)
{
	for ( vector<PROCESS_INFO_EX>::iterator Iter = m_ProcessListEx.begin( ); 
		Iter != m_ProcessListEx.end( ); 
		Iter++)
	{
		// ����Ѿ����룬����ParentPid�����ڴ�������PID
		if (Iter->bInsert == TRUE				|| 
			Iter->ulParentPid != pItem->ulPid	|| 
			Iter->nCreateTime <= pItem->nCreateTime)
		{
			continue;
		}
	
		AddProcessTreeItem(&(*Iter), hRoot);
	}
}

//
// ת������,��PROCESS_INFO�ṹת����PROCESS_INFO_EX�ṹ������
//
void CListProcessDlg::TransferProcessInfo2ProcessInfoEx()
{
	for ( vector <PROCESS_INFO>::iterator Iter = m_Ring0ProcessList.begin( ); Iter != m_Ring0ProcessList.end( ); Iter++ )
	{
		PROCESS_INFO item = *Iter;

		if (item.ulPid == 0 && item.ulParentPid == 0 && item.ulEprocess == 0)
		{
			continue;
		}

		CString strPath = TrimPath(item.szPath);

		// �������ͼ��
		SHFILEINFO sfi;
		DWORD_PTR nRet;

		ZeroMemory(&sfi, sizeof(SHFILEINFO));

		nRet = SHGetFileInfo(strPath, 
			FILE_ATTRIBUTE_NORMAL, 
			&sfi, 
			sizeof(SHFILEINFO), 
			SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES
			);	

		int nIcon = -1;
		if (nRet)
		{
			nIcon = m_imgList.Add ( sfi.hIcon );
			DestroyIcon(sfi.hIcon);
		}
		else
		{
			nIcon = m_imgList.Add ( m_hProcessIcon );
		}

		PROCESS_INFO_EX info;
		info.ulPid = item.ulPid;
		info.ulParentPid = item.ulParentPid;
		info.ulEprocess = item.ulEprocess;
		info.nIconOrder = nIcon;
		info.szPath = strPath;
		info.bInsert = FALSE;
		info.nCreateTime = item.ulCreateTime;
		m_ProcessListEx.push_back(info);
	}
}

//
// ���һ�������б�Ľ�����
//
void CListProcessDlg::AddProcessTreeItem(PROCESS_INFO_EX* Iter, HTREEITEM hItem)
{
	CString strPid, strParentPid, strEprocess, strR3Access;
	BOOL bHide = FALSE;
	HTREEITEM hRoot;

	// �������·����pid
	CString strPath = Iter->szPath;
	CString strImageName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\') - 1);
	Iter->ulParentPid ? strParentPid.Format(L"%d", Iter->ulParentPid) : strParentPid.Format(L"-");

	if (CanOpenProcess(Iter->ulPid))
	{
		strR3Access = L"-";
	}
	else
	{
		m_nDenyAccessCnt++;
		strR3Access = szDeny[g_enumLang];
	}

	strEprocess.Format(L"0x%08X", Iter->ulEprocess);
	strPid.Format(L"%d", Iter->ulPid);
	CString strCompany = m_Functions.GetFileCompanyName(strPath);
	CString szDescription = m_Functions.GetFileDescription(strPath);
	CString m_szStartedTime = FileTime2SystemTime(Iter->nCreateTime);
	
	if (hItem == NULL)
	{
		hRoot = m_processTree.GetTreeCtrl().InsertItem(strImageName, Iter->nIconOrder, Iter->nIconOrder);
	}
	else
	{
		hRoot = m_processTree.GetTreeCtrl().InsertItem(strImageName, Iter->nIconOrder, Iter->nIconOrder, hItem);
	}
	
	m_processTree.SetItemText(hRoot, eProcessTreePid, strPid);
// 	m_processTree.SetItemText(hRoot, eProcessParentPid, strParentPid);
	m_processTree.SetItemText(hRoot, eProcessTreePath, strPath);
//	m_processTree.SetItemText(hRoot, eProcessEprocess, strEprocess);
	m_processTree.SetItemText(hRoot, eProcessTreeR3Access, strR3Access);
	m_processTree.SetItemText(hRoot, eProcessTreeFileCompany, strCompany);
	m_processTree.SetItemText(hRoot, eProcessTreeFileDescription, szDescription);
	m_processTree.SetItemText(hRoot, eProcessTreeCreateTime, m_szStartedTime);

	// Ĭ��Ϊ��ɫ
	ITEM_COLOR_INFO clrInfo;
	clrInfo.textClr = enumBlack;

	if (IsProcessHide(Iter->ulPid))
	{
		m_nHideCnt++;
		clrInfo.textClr = enumRed;
	}
	else if ( !(m_Functions.IsMicrosoftApp(strCompany) || 
		!strPath.CompareNoCase( L"System" ) || 
		!strPath.CompareNoCase(L"Idle")) )
	{
		clrInfo.textClr = enumBlue;
	}

	// ����·���ʾģ���б�����΢����̱�ע��������ģ�飬��ô��ʾ����ɫ
	if (clrInfo.textClr == enumBlack && IsHaveNotMicrosoftModule(Iter->ulPid))
	{
		clrInfo.textClr = enumTuhuang;
	}

	// �����Ѳ��룬�������
	Iter->bInsert = TRUE;
	Iter->hItem = hRoot;

	// ���µ���push���б���
	m_vectorProcessEx.push_back(*Iter);
	m_vectorColor.push_back(clrInfo);
	m_processTree.GetTreeCtrl().SetItemData(hRoot, m_nProcessCnt);
	
	m_nProcessCnt++;

	// ���PID��Ϊ0����ô�Ͳ�������
	if (Iter->ulPid != 0)
	{
		InsertTreeProcessChildItem(Iter, hRoot);
		m_processTree.GetTreeCtrl().Expand(hRoot, TVE_EXPAND);
	}
}

//
// �Խ���������ʽ��ʾ�����б�
//
void CListProcessDlg::ShowProcessAsTreeList()
{
	m_szProcessCnt.Format(szProcessState[g_enumLang], 0, 0, 0);
	SendMessage(WM_UPDATE_PROCESS_DATA);

	if (IsWindow(m_processTree.GetTreeCtrl().m_hWnd))
	{
		m_processTree.GetTreeCtrl().DeleteAllItems();
	}

	// ���image�б������б���ʼ��Ĭ��ͼ��
	m_ProcessListEx.clear();
	m_vectorColor.clear();
	m_vectorProcessEx.clear();
	
	m_nDenyAccessCnt = m_nProcessCnt = m_nHideCnt = 0;

	int nImageCount = m_imgList.GetImageCount();
	for(int j = 0; j < nImageCount; j++)   
	{   
		m_imgList.Remove(0);   
	}
	
	m_clsProcess.EnumProcess(m_Ring0ProcessList);
	if (m_Ring0ProcessList.empty())
	{
		return;
	}

	// ���ݸ�����ID�Ÿ���
	SortByParentPid(m_Ring0ProcessList);

	// ��ȡring3�����б�
	GetRing3ProcessList(); 
	
	// ת������
	TransferProcessInfo2ProcessInfoEx();
	if (m_ProcessListEx.empty())
	{
		return;
	}

	// ��һ�α�������û�и����̵���
	for ( vector<PROCESS_INFO_EX>::iterator Iter = m_ProcessListEx.begin( ); 
		Iter != m_ProcessListEx.end( ); 
		Iter++)
	{
		// �ж��Ƿ��Ѿ�����
		if (Iter->bInsert != FALSE)
		{
			continue;
		}

		// �ж��Ƿ��и����̣�û����ֱ���Ը����̲���
		if (!IsHaveParent(*Iter))
		{
			AddProcessTreeItem(&(*Iter), NULL);
		}
	}
	
	m_szProcessCnt.Format(szProcessState[g_enumLang], m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);
	SendMessage(WM_UPDATE_PROCESS_DATA);
}

//
// ö�ٽ��̺�������Ϊ������ģʽ���б�ģʽ������ʾ�����б�
//
VOID CListProcessDlg::ListProcess()
{
	if (m_bShowAsTreeList)
	{
		m_processTree.ShowWindow(FALSE);
		ShowProcessAsTreeList();
		m_processTree.ShowWindow(TRUE);
		m_processTree.Invalidate(TRUE);
	}
	else
	{
		ShowProcessAsList();
	}
}

//
// ����listģʽ����ɫ��ʾ
//
void CListProcessDlg::OnNMCustomdrawProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

	*pResult = CDRF_DODEFAULT;

	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
	{
		COLORREF clrNewTextColor, clrNewBkColor;
		int nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );

		clrNewTextColor = m_clrMicrosoftProc;
		clrNewBkColor = RGB( 255, 255, 255 );	

		int nData = (int)m_processList.GetItemData(nItem);
		ITEM_COLOR clrInfo = m_vectorColor.at(nData).textClr;
		if (clrInfo == enumRed)
		{
			clrNewTextColor = m_clrHideProc;
		}
		else if ( clrInfo == enumBlue )
		{
			clrNewTextColor = m_clrNormalProc;
		}
		else if (clrInfo == enumTuhuang)
		{
			clrNewTextColor = m_clrMicroHaveOtherModuleProc;
		}
		else if (clrInfo == enumPurple)
		{
			clrNewTextColor = m_clrNoSigniture;
		}
		
 		pLVCD->clrText = clrNewTextColor;
 		pLVCD->clrTextBk = clrNewBkColor;

		*pResult = CDRF_DODEFAULT;
	}
}

//
// ����Listģʽ�µ��Ҽ��˵�
//
void CListProcessDlg::OnNMRclickProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu exportto;
	exportto.CreatePopupMenu();
	exportto.AppendMenu(MF_STRING, ID_EXPORT_TEXT, szText[g_enumLang]);
	exportto.AppendMenu(MF_STRING, ID_EXPORT_EXCEL, szExcel[g_enumLang]);
	
	WCHAR szMiniDump[] = {'M','i','n','i',' ','D','u','m','p','\0'};
	WCHAR szFullDump[] = {'F','u','l','l',' ','D','u','m','p','\0'};

	CMenu createdump;
	createdump.CreatePopupMenu();
	createdump.AppendMenu(MF_STRING, ID_CREATE_MINIDUMP, szMiniDump);
	createdump.AppendMenu(MF_STRING, ID_CREATE_FULLDUMP, szFullDump);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_PROCESS_REFRESH, szProcessRefresh[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_SHOW_PROCESS_TREE, szShowProcessTree[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
 	menu.AppendMenu(MF_STRING, ID_SHOW_ALL_PROCESS_INFO, szShowAllProcessInfo[g_enumLang]);
 	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_KILL_PROCESS, szKillProcess[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_KILL_DELETE, szKillAndDeleteFile[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_SUSPEND_PROCESS, szSuspend[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_RESUME_PROCESS, szResume[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_PROCESS_HOOK, szScanHooks[g_enumLang]);
 	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_INJECT_DLL, szInjectDll[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_LISTPROCESS_DUMP, szProcessDump[g_enumLang]);
	menu.AppendMenu(MF_POPUP, (UINT)createdump.m_hMenu, szCreateDump[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_COPY_PROCESS_NAME, szCopyImageName[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_COPY_PROCESS_PATH, szCopyImagePath[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_CHECK_ATTRIBUTE, szProcessProperties[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_SEARCH_ONLINE, szSearchProcessOnline[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_LOCATION_EXPLORER, szFindInExplorer[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_PROCESS_LOCATION_AT_FILE_MANAGER, szFindInAntiSpy[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_CHECK_SIGN, szVerifyProcessSignature[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_CHECK_ALL_SIGN, szVerifyAllProcessSignature[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_LOOK_WINDOW_FOR_PROCESS, szFindProcessByWindwo[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_PROCESS_FIND_MODULE, szFindModule[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_PROCESS_FIND_UNSIGNED_MODUES, szFindUnsignedModules[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_POPUP, (UINT)exportto.m_hMenu, szExportTo[g_enumLang]);

	// ����Ĭ����
	menu.SetDefaultItem(ID_SHOW_ALL_PROCESS_INFO);
	
	int x = GetSystemMetrics(SM_CXMENUCHECK);
	int y = GetSystemMetrics(SM_CYMENUCHECK);
	
	DebugLog(L"x: %d, y: %d", x, y);

	if (x >= 15 && y >= 15)
	{
		// ���ò˵�ͼ��
		menu.SetMenuItemBitmaps(ID_PROCESS_REFRESH, MF_BYCOMMAND, &m_bmRefresh, &m_bmRefresh);
		menu.SetMenuItemBitmaps(ID_LOCATION_EXPLORER, MF_BYCOMMAND, &m_bmExplorer, &m_bmExplorer);
		menu.SetMenuItemBitmaps(ID_KILL_PROCESS, MF_BYCOMMAND, &m_bmDelete, &m_bmDelete);
		menu.SetMenuItemBitmaps(ID_KILL_DELETE, MF_BYCOMMAND, &m_bmDelete, &m_bmDelete);
		menu.SetMenuItemBitmaps(ID_SEARCH_ONLINE, MF_BYCOMMAND, &m_bmSearch, &m_bmSearch);
		menu.SetMenuItemBitmaps(ID_CHECK_ATTRIBUTE, MF_BYCOMMAND, &m_bmShuxing, &m_bmShuxing);
		menu.SetMenuItemBitmaps(ID_COPY_PROCESS_NAME, MF_BYCOMMAND, &m_bmCopy, &m_bmCopy);
		menu.SetMenuItemBitmaps(ID_COPY_PROCESS_PATH, MF_BYCOMMAND, &m_bmCopy, &m_bmCopy);
		menu.SetMenuItemBitmaps(ID_PROCESS_LOCATION_AT_FILE_MANAGER, MF_BYCOMMAND, &m_bmAntiSpy, &m_bmAntiSpy);
		menu.SetMenuItemBitmaps(ID_SHOW_ALL_PROCESS_INFO, MF_BYCOMMAND, &m_bmDetalInof, &m_bmDetalInof);
		menu.SetMenuItemBitmaps(ID_CHECK_SIGN, MF_BYCOMMAND, &m_bmSign, &m_bmSign);
		menu.SetMenuItemBitmaps(ID_CHECK_ALL_SIGN, MF_BYCOMMAND, &m_bmSign, &m_bmSign);
		menu.SetMenuItemBitmaps(ID_PROCESS_FIND_MODULE, MF_BYCOMMAND, &m_bmLookFor, &m_bmLookFor);
		menu.SetMenuItemBitmaps(ID_PROCESS_FIND_UNSIGNED_MODUES, MF_BYCOMMAND, &m_bmLookFor, &m_bmLookFor);
		menu.SetMenuItemBitmaps(ID_LOOK_WINDOW_FOR_PROCESS, MF_BYCOMMAND, &m_bmLookFor, &m_bmLookFor);

		menu.SetMenuItemBitmaps(16, MF_BYPOSITION, &m_bmWindbg, &m_bmWindbg);
		menu.SetMenuItemBitmaps(34, MF_BYPOSITION, &m_bmExport, &m_bmExport);
	}

	// ���m_list�ǿյģ���ô���ˡ�ˢ�¡�������ȫ���û�
	if (!m_processList.GetItemCount()) 
	{
		for (int i = 2; i < 35; i++)
		{
			menu.EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);
		}
	}
	else
	{
		int nSelCnt = m_processList.GetSelectedCount();
		if ( nSelCnt != 1 )
		{
			for (int i = 2; i < 33; i++)
			{
				menu.EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);
			}

			menu.EnableMenuItem(ID_LOOK_WINDOW_FOR_PROCESS, MF_BYCOMMAND | MF_ENABLED);
			menu.EnableMenuItem(ID_PROCESS_FIND_MODULE, MF_BYCOMMAND | MF_ENABLED);
			menu.EnableMenuItem(ID_PROCESS_FIND_UNSIGNED_MODUES, MF_BYCOMMAND | MF_ENABLED);
			menu.EnableMenuItem(ID_CHECK_ALL_SIGN, MF_BYCOMMAND | MF_ENABLED);

			if (nSelCnt >= 2)
			{
				menu.EnableMenuItem(ID_KILL_PROCESS, MF_BYCOMMAND | MF_ENABLED);
				menu.EnableMenuItem(ID_KILL_DELETE, MF_BYCOMMAND | MF_ENABLED);
			}
		}

		// ���ѡ����һ��
		else
		{
			SUSPEND_OR_RESUME_TYPE SorType = ProcessMenuResumeOrSuspend();
			switch (SorType)
			{
			case enumAllFalse:
				menu.EnableMenuItem(ID_SUSPEND_PROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				menu.EnableMenuItem(ID_RESUME_PROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				break;

			case enumAllOk:
				menu.EnableMenuItem(ID_SUSPEND_PROCESS, MF_BYCOMMAND | MF_ENABLED);
				menu.EnableMenuItem(ID_RESUME_PROCESS, MF_BYCOMMAND | MF_ENABLED);
				break;

			case enumResume:
				menu.EnableMenuItem(ID_SUSPEND_PROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				menu.EnableMenuItem(ID_RESUME_PROCESS, MF_BYCOMMAND | MF_ENABLED);
				break;

			case enumSuspend:
				menu.EnableMenuItem(ID_SUSPEND_PROCESS, MF_BYCOMMAND | MF_ENABLED);
				menu.EnableMenuItem(ID_RESUME_PROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				break;
			}
		}
	}
	
	menu.EnableMenuItem(ID_SHOW_PROCESS_TREE, MF_BYCOMMAND | MF_ENABLED);

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();
	exportto.DestroyMenu();

	*pResult = 0;
}

//
// ����ˢ�²˵�
//
void CListProcessDlg::OnProcessRefresh()
{
	ListProcess();
}

//
// ��֤��������ǩ��
//
void CListProcessDlg::OnCheckSign()
{
	CString szPath = L"";

	if (!m_bShowAsTreeList)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		if (nItem == -1)
		{
			return;
		}

		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo)
		{
			return;
		}
		
		szPath = pInfo->szPath;
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (!hItem)
		{
			return;
		}

		PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
		if (!pInfo)
		{
			return;
		}

		szPath = pInfo->szPath;
	}

	if (!szPath.IsEmpty()/* && PathFileExists(szPath)*/)
	{
		m_SignVerifyDlg.m_bSingle = TRUE;
		m_SignVerifyDlg.m_path = szPath;
		m_SignVerifyDlg.DoModal();
	}
}

//
// ��֤���н��̵�ǩ��
//
void CListProcessDlg::OnCheckAllSign()
{
	m_SignVerifyDlg.m_NotSignItemList.clear();
	m_SignVerifyDlg.m_bSingle = FALSE;
	m_SignVerifyDlg.m_pList = &m_processList;;
	m_SignVerifyDlg.m_nPathSubItem = eProcessPath;
	m_SignVerifyDlg.m_hWinWnd = this->m_hWnd;
	m_SignVerifyDlg.DoModal();
}

//
// �����б�ģʽ�£�����item�õ����ӦPROCESS_INFO����Ϣ
//
PPROCESS_INFO CListProcessDlg::GetProcessInfoByItem(int nItem)
{
	PPROCESS_INFO pInfo = NULL;
	
	if (nItem == -1)
	{
		return pInfo;
	}

	int nData = (int)m_processList.GetItemData(nItem);
	pInfo = &m_vectorProcesses.at(nData);
	
	return pInfo;
}

//
// ���̽�����ģʽ�£�����item�õ����ӦPROCESS_INFO_EX����Ϣ
//
PPROCESS_INFO_EX CListProcessDlg::GetProcessInfoExByItem(HTREEITEM nItem)
{
	PPROCESS_INFO_EX pInfo = NULL;

	if (nItem == NULL)
	{
		return pInfo;
	}
	
	int nData = (int)m_processTree.GetTreeCtrl().GetItemData(nItem);
	pInfo = &m_vectorProcessEx.at(nData);
	
	return pInfo;
}

//
// �Ҽ��˵� - ��������
//
void CListProcessDlg::OnKillProcess()
{
	//
	// �����ǵ���ȷ��
	//

	// �б�ģʽ��
	if (!m_bShowAsTreeList)
	{
		int nCnt = m_processList.GetSelectedCount();

		// ����ǵ�ѡ
		if ( nCnt == 1)
		{
			int nItem = m_Functions.GetSelectItem(&m_processList);
			if (nItem != -1)
			{
				CString szImage = m_processList.GetItemText(nItem, eProcessImageName);
				CString szShow;
				szShow.Format(szAreYOUSureKillProcess[g_enumLang], szImage);

				if (MessageBox(szShow, szToolName, MB_YESNO | MB_ICONQUESTION) != IDYES)
				{
					return;
				}
			}
		}

		// ��ѡ
		else if ( nCnt > 1)
		{
			if (MessageBox(szAreYOUSureKillTheseProcess[g_enumLang], szToolName, MB_YESNO | MB_ICONQUESTION) != IDYES)
			{
				return;
			}
		}

		// û��ѡ��
		else
		{
			return;
		}
	}

	// ������ģʽ��
	else
	{
		CString szImage = m_processTree.GetItemText(m_processTree.GetTreeCtrl().GetSelectedItem(), eProcessTreeImageName);
		CString szShow;
		szShow.Format(szAreYOUSureKillProcess[g_enumLang], szImage);

		if (MessageBox(szShow, szToolName, MB_YESNO | MB_ICONQUESTION) != IDYES)
		{
			return;
		}
	}
	
	//
	// ������ʼ��������
	//

	// �б�ģʽ�±���ȫ��ѡ�е���
	if (!m_bShowAsTreeList)
	{
		POSITION pos = m_processList.GetFirstSelectedItemPosition();
		while (pos)
		{
			int nItem = m_processList.GetNextSelectedItem(pos);
			PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);

			if (pInfo)
			{
				m_clsProcess.KillProcess(0, pInfo->ulEprocess);
			}
			
			m_processList.DeleteItem(nItem);
			pos = m_processList.GetFirstSelectedItemPosition();
		}
	}

	// ������ģʽ��
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (!hItem)
		{
			return;
		}
		
		PPROCESS_INFO_EX info = GetProcessInfoExByItem(hItem);
		if (!info)
		{
			return;
		}

		m_clsProcess.KillProcess(0, info->ulEprocess);
		
		// ����������ôˢ���½�����
		if (m_processTree.GetTreeCtrl().ItemHasChildren(hItem))
		{
			Sleep(500);
			OnProcessRefresh();
		}
		else
		{
			m_processTree.GetTreeCtrl().DeleteItem(hItem);
		}
	}
}

// 
// �Ҽ��˵� - �������̲�ɾ���ļ�
//
void CListProcessDlg::OnKillDelete()
{
	//
	// �����ǵ���ȷ��
	//

	// �б�ģʽ��
	if (!m_bShowAsTreeList)
	{
		int nCnt = m_processList.GetSelectedCount();
		if ( nCnt == 1)
		{
			int nItem = m_Functions.GetSelectItem(&m_processList);
			if (nItem != -1)
			{
				CString szImage = m_processList.GetItemText(nItem, eProcessImageName);
				CString szShow;
				szShow.Format(szAreYOUSureKillAndDeleteProcess[g_enumLang], szImage);

				if (MessageBox(szShow, szToolName, MB_YESNO | MB_ICONQUESTION) != IDYES)
				{
					return;
				}
			}
		}
		else if ( nCnt > 1)
		{
			if (MessageBox(szAreYOUSureKillAndDeleteProcesses[g_enumLang], szToolName, MB_YESNO | MB_ICONQUESTION) != IDYES)
			{
				return;
			}
		}
		else
		{
			return;
		}

		//
		// ��ʼɾ���ļ�����������
		//
		POSITION pos = m_processList.GetFirstSelectedItemPosition();
		while (pos)
		{
			int nItem = m_processList.GetNextSelectedItem(pos);
			PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);

			if (pInfo)
			{
				// ��������
				m_clsProcess.KillProcess(0, pInfo->ulEprocess);

				Sleep(100);

				// ɾ���ļ�
				m_Functions.KernelDeleteFile(pInfo->szPath);

				Sleep(100);
			}
						
			m_processList.DeleteItem(nItem);
			pos = m_processList.GetFirstSelectedItemPosition();
		}
	}

	// ������ģʽ
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (!hItem)
		{
			return;
		}
		
		CString szImage = m_processTree.GetItemText(hItem, eProcessTreeImageName);
		CString szShow;
		szShow.Format(szAreYOUSureKillAndDeleteProcess[g_enumLang], szImage);
		if (MessageBox(szShow, szToolName, MB_YESNO | MB_ICONQUESTION) != IDYES)
		{
			return;
		}

		PPROCESS_INFO_EX info = GetProcessInfoExByItem(hItem);
		if (!info)
		{
			return;
		}
		
		// ��������
		m_clsProcess.KillProcess(0, info->ulEprocess);
		
		Sleep(100);

		// ɾ���ļ�
		m_Functions.KernelDeleteFile(info->szPath);
		
		// ����������ôˢ���½�����
		if (m_processTree.GetTreeCtrl().ItemHasChildren(hItem))
		{
			Sleep(500);
			OnProcessRefresh();
		}
		else
		{
			m_processTree.GetTreeCtrl().DeleteItem(hItem);
		}
	}
}

//
// ��ͣ��������
//
void CListProcessDlg::OnSuspendProcess()
{
	// �б�ģʽ
	if (!m_bShowAsTreeList)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo)
		{
			return;
		}
		
		m_clsProcess.SuspendProcess(pInfo->ulPid, pInfo->ulEprocess);
	}

	// ������ģʽ
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
			if (!pInfo)
			{
				return;
			}

			m_clsProcess.SuspendProcess(pInfo->ulPid, pInfo->ulEprocess);
		}
	}
}

//
// �ָ���������
//
void CListProcessDlg::OnResumeProcess()
{
	// �б�ģʽ
	if (!m_bShowAsTreeList)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo)
		{
			return;
		}

		m_clsProcess.ResumeProcess(pInfo->ulPid, pInfo->ulEprocess);
	}

	// ������ģʽ
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
			if (!pInfo)
			{
				return;
			}

			m_clsProcess.ResumeProcess(pInfo->ulPid, pInfo->ulEprocess);
		}
	}
}

//
// ����������
//
void CListProcessDlg::OnCopyProcessName()
{
	CString szImageName;

	if (!m_bShowAsTreeList)
	{
		POSITION pos = m_processList.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nItem = m_processList.GetNextSelectedItem(pos);
			szImageName = m_processList.GetItemText(nItem, eProcessImageName);
		}
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			szImageName = m_processTree.GetItemText(hItem, eProcessTreeImageName);
		}
	}

	m_Functions.SetStringToClipboard(szImageName);
}

//
// ��������·��
//
void CListProcessDlg::OnCopyProcessPath()
{
	CString szPath;

	if (!m_bShowAsTreeList)
	{
		POSITION pos = m_processList.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nItem = m_processList.GetNextSelectedItem(pos);
			szPath = m_processList.GetItemText(nItem, eProcessPath);
		}
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			szPath = m_processTree.GetItemText(hItem, eProcessTreePath);
		}
	}

	m_Functions.SetStringToClipboard(szPath);
}

//
// �鿴��������
//
void CListProcessDlg::OnCheckAttribute()
{
	CString szPath;

	if (!m_bShowAsTreeList)
	{
		POSITION pos = m_processList.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nItem = m_processList.GetNextSelectedItem(pos);
			szPath = m_processList.GetItemText(nItem, eProcessPath);
		}
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			szPath = m_processTree.GetItemText(hItem, eProcessTreePath);
		}
	}

	m_Functions.OnCheckAttribute(szPath);
}

//
// ��λ����
//
void CListProcessDlg::OnLocationExplorer()
{
	CString szPath = L"";

	if (!m_bShowAsTreeList)
	{
		POSITION pos = m_processList.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nItem = m_processList.GetNextSelectedItem(pos);
			szPath = m_processList.GetItemText(nItem, eProcessPath);
		}
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			szPath = m_processTree.GetItemText(hItem, eProcessTreePath);
		}
	}

	m_Functions.LocationExplorer(szPath);
}

//
// ��������������
//
void CListProcessDlg::OnSearchOnline()
{
	CString szImageName;

	if (!m_bShowAsTreeList)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		if (nItem != -1)
		{
			szImageName = m_processList.GetItemText(nItem, eProcessImageName);
		}
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			szImageName = m_processTree.GetItemText(hItem, eProcessTreeImageName);
		}
	}

	m_Functions.SearchOnline(szImageName);
}

//
// ����������ʱ�����½����б��·�����Ϣ����
//
void CListProcessDlg::UpdateBelowDlg()
{
	if (m_tab.m_hWnd && m_ModuleDlg.GetSafeHwnd())
	{
//		CRect rc;
// 		m_tab.GetClientRect(rc);
// 		rc.top += 20;
// 		rc.bottom -= 2;
// 		rc.left += 0;
// 		rc.right -= 2;

		CRect rc;
		m_tab.GetClientRect(rc);
		rc.top += 22;
		rc.bottom -= 3;
		rc.left += 2;
		rc.right -= 3;
		
		for (ULONG i = 0; i < m_nBelowDialog; i++)
		{
			m_pBelowDialog[i]->MoveWindow(&rc);
		}
	}
}

//
// ϵͳ��Ϣ��Ӧ����
//
void CListProcessDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
	UpdateBelowDlg();
}

//
// �Ҽ��˵� - ��ʾ������ϸ��Ϣ
//
void CListProcessDlg::OnShowAllProcessInfo()
{
	if (!m_bShowAsTreeList)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		if ( -1 == nItem)
		{
			return;
		}

		ShowProcessListDetailInfo(m_Functions.GetSelectItem(&m_processList));
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (!hItem)
		{
			return;
		}

		ShowProcessTreeDetailInfo(GetTreeListSelectItem());
	}
}

//
// ���ݴ�������ITEM�ж��Ƿ���ǩ��
//
BOOL CListProcessDlg::IsNotSignItem(ULONG nItem)
{
	BOOL bRet = FALSE;

	if (!m_SignVerifyDlg.m_NotSignItemList.empty())
	{
		list<ULONG>::iterator findItr;
		findItr = find(m_SignVerifyDlg.m_NotSignItemList.begin(), m_SignVerifyDlg.m_NotSignItemList.end(), nItem);
		if (findItr != m_SignVerifyDlg.m_NotSignItemList.end())
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) 
{ 
	return lParam1 < lParam2;
}

//
// ������֤ǩ�������󣬻ᷢ��һ��WM_VERIFY_SIGN_OVER���Զ�����Ϣ
//
LRESULT CListProcessDlg::VerifySignOver(WPARAM wParam, LPARAM lParam)
{
	ULONG nCnt = m_processList.GetItemCount();

	for (ULONG i = 0; i < nCnt; i++)
	{
		if (IsNotSignItem(i))
		{
			// ������ɫ
			int nData = (int)m_processList.GetItemData(i);
			m_vectorColor.at(nData).textClr = enumPurple;

			// �����Ƿ�Vrified
			CString szComp = m_processList.GetItemText(i, eProcessFileCompany);
			CString szCompTemp = szNotVerified[g_enumLang] + szComp;
			m_processList.SetItemText(i, eProcessFileCompany, szCompTemp);
		}
		else
		{
			// �����Ƿ�Vrified
			CString szComp = m_processList.GetItemText(i, eProcessFileCompany);
			CString szCompTemp = szVerified[g_enumLang] + szComp;
			m_processList.SetItemText(i, eProcessFileCompany, szCompTemp);
		}
	}

//	m_processList.SortItems(MyCompareProc, NULL);

	return 0;
}

//
// ����б�ͷ��ʱ��ᴥ�������Ϣ����ô��ʱ�����б���·������ǲ���Ҫö�ٵ�
//
LRESULT CListProcessDlg::OnClickListHeaderStart(WPARAM wParam, LPARAM lParam)
{
	m_bShowBelow = FALSE;
	return 0;
}

//
// ����б�ͷ��ʱ��ᴥ�������Ϣ�����������Ѿ����
//
LRESULT CListProcessDlg::OnClickListHeaderEnd(WPARAM wParam, LPARAM lParam)
{
	m_bShowBelow = TRUE;
	return 0;
}

//
// ������Ϣ��EXCEL���
//
void CListProcessDlg::OnExportExcel()
{
	WCHAR szProcess[] = {'P','r','o','c','e','s','s','\0'};
	m_Functions.ExportListToExcel(&m_processList, szProcess, m_szProcessCnt);
}

//
// ������Ϣ��TXT�ļ�
//
void CListProcessDlg::OnExportText()
{
	m_Functions.ExportListToTxt(&m_processList, m_szProcessCnt);
}

//
// ���������ڴ�
//
void CListProcessDlg::OnDumpProcessMemory()
{
	CString szImage = L"";
	DWORD dwPid = 0, dwpEprocess = 0;

	if (!m_bShowAsTreeList)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		if (nItem != -1)
		{
			PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
			if (pInfo)
			{
				szImage = m_processList.GetItemText(nItem, eProcessImageName);
				dwPid = pInfo->ulPid;
				dwpEprocess = pInfo->ulEprocess;
			}
		}
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
			if (pInfo)
			{
				dwPid = pInfo->ulPid;
				dwpEprocess = pInfo->ulEprocess;
				szImage = m_processTree.GetItemText(hItem, eProcessTreeImageName);
			}
			
		}
	}

	if (dwPid == 0 || dwpEprocess == 0)
	{
		return;
	}

	WCHAR *szImageTemp = CString2WString(szImage);
	if (!szImageTemp)
	{
		return;
	}

	WCHAR szFile[MAX_PATH] = {0};
	wcsncpy_s(szFile, MAX_PATH, L"Dumped_", wcslen(L"Dumped_"));
	wcscat_s(szFile, MAX_PATH - wcslen(L"Dumped_"), szImageTemp);

	free(szImageTemp);

	CFileDialog fileDlg(
		FALSE, 
		0, 
		(LPWSTR)szFile, 
		0, 
		L"All Files (*.*)|*.*||",
		0
		);

	if (IDOK != fileDlg.DoModal())
	{
		return;
	}

	CString szFilePath = fileDlg.GetFileName();
	if ( !PathFileExists(szFilePath) ||
		(PathFileExists(szFilePath) && MessageBox(szFileExist[g_enumLang], szDumpMemory[g_enumLang], MB_YESNO | MB_ICONQUESTION) == IDYES))
	{
		BOOL bRet = FALSE;
		PVOID pBuffer = NULL;
		ULONG nSize = sizeof(ULONG);
		BOOL bFirst = TRUE;

		do 
		{
			if (!bFirst)
			{
				nSize = *(PULONG)pBuffer;
			}

			if (pBuffer)
			{
				free(pBuffer);
				pBuffer = NULL;
			}
			
			pBuffer = malloc(nSize);
			
			if (pBuffer)
			{
				memset(pBuffer, 0, nSize);
				COMMUNICATE_PROCESS cp;
				cp.OpType = enumDumpProcess;
				cp.op.DumpProcess.nPid = dwPid;
				cp.op.DumpProcess.pEprocess = dwpEprocess;
				bRet = m_Driver.CommunicateDriver(&cp, sizeof(COMMUNICATE_PROCESS), pBuffer, nSize, NULL);
			}
			
			bFirst = FALSE;

		} while (!bRet && GetLastError() == ERROR_INSUFFICIENT_BUFFER);

		BOOL bCopy = FALSE;
		if (bRet)
		{
			// �޸�PEӳ��
			if (MessageBox(szFixDump[g_enumLang], szDumpMemory[g_enumLang], MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				FixPeDump(pBuffer);
			}
			
			CFile file;
			TRY 
			{
				if (file.Open(szFilePath,  CFile::modeCreate | CFile::modeWrite))
				{
					file.Write(pBuffer, nSize);
					file.Close();
					bCopy = TRUE;
				}
			}
			CATCH_ALL( e )
			{
				file.Abort();   // close file safely and quietly
				//THROW_LAST();
			}
			END_CATCH_ALL
		}

		if (bCopy)
		{
			MessageBox(szDumpOK[g_enumLang], szDumpMemory[g_enumLang]);
		}
		else
		{
			MessageBox(szDumpFailed[g_enumLang], szDumpMemory[g_enumLang]);
		}

		free(pBuffer);
	}
}

//
// ����ģ����߾��
//
void CListProcessDlg::OnProcessFindModule()
{
	CFindModuleDlg FindModuleDlg;
	FindModuleDlg.DoModal();
}

//
// ����û��ǩ����ģ��
//
void CListProcessDlg::OnProcessFindUnsignedModues()
{
	CNotSignModuleDlg NotSignModuleDlg;
	NotSignModuleDlg.DoModal();
}

//
// ��ȡ���̹��𡢻ָ�״̬
//
SUSPEND_OR_RESUME_TYPE CListProcessDlg::GetProcessSuspendOrResumeStatus(ULONG nPid, ULONG pEprocess)
{	
	SUSPEND_OR_RESUME_TYPE type = enumAllFalse;
	COMMUNICATE_PROCESS cp;
	RESUME_OR_SUSPEND rs;

	memset(&rs, 0, sizeof(RESUME_OR_SUSPEND));

	cp.OpType = enumGetProcessSuspendStatus;
	cp.op.GetProcessSuspendStatus.nPid = nPid;
	cp.op.GetProcessSuspendStatus.pEprocess = pEprocess;

	if (m_Driver.CommunicateDriver(&cp, sizeof(COMMUNICATE_PROCESS), &rs, sizeof(RESUME_OR_SUSPEND), NULL))
	{
		if ((rs.bResume && rs.bSuspend))
		{
			type = enumAllOk;
		}
		else if (!rs.bResume && !rs.bSuspend)
		{
			type = enumAllFalse;
		}
		else if (rs.bResume)
		{
			type = enumResume;
		}
		else if (rs.bSuspend)
		{
			type = enumSuspend;
		}
	}

	return type;
}

//
// ���οؼ����Ҽ��˵���ȡ���̹��𡢻ָ�״̬
//
SUSPEND_OR_RESUME_TYPE CListProcessDlg::IsSuspendOrResumeProcessTree()
{
	SUSPEND_OR_RESUME_TYPE type = enumAllFalse;

	// ���û��ѡ��
	HTREEITEM hItem = GetTreeListSelectItem();
	if (!hItem)
	{
		return type;
	}

	PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
	if (!pInfo)
	{
		return type;
	}

	// ���ѡ�е������ǵĽ���
	if (pInfo->ulPid == GetCurrentProcessId())
	{
		return type;
	}
	
	type = GetProcessSuspendOrResumeStatus(pInfo->ulPid, pInfo->ulEprocess);

	return type;
}

// 
// ��ȡ���̵Ĺ���ͻָ�״̬
//
SUSPEND_OR_RESUME_TYPE CListProcessDlg::ProcessMenuResumeOrSuspend()
{
	SUSPEND_OR_RESUME_TYPE type = enumAllFalse;

	//
	// ���û��ѡ�У����������û�
	//
	int nItem = m_Functions.GetSelectItem(&m_processList);
	if (nItem == -1)
	{
		return type;
	}
	
	PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
	if (!pInfo)
	{
		return type;
	}
	
	//
	// ���ѡ�е������ǵĽ��̣����������û�
	//
	if (pInfo->ulPid == GetCurrentProcessId())
	{
		return type;
	}
	
	type = GetProcessSuspendOrResumeStatus(pInfo->ulPid, pInfo->ulEprocess);

	return type;
}

//
// ϵͳ����
//
BOOL CListProcessDlg::PreTranslateMessage(MSG* pMsg)
{
	// ���Ƚ�����ݰ���
	if (m_hAccel)   
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))   
		{
			return(TRUE);
		}
	}
	
	if (m_bShowTipWindow)
	{
		if(pMsg->message == WM_MOUSEMOVE && pMsg->wParam == 0)
		{
			int id = ::GetDlgCtrlID(pMsg->hwnd);
			CPoint point;
			point.x = LOWORD(pMsg->lParam);
			point.y = HIWORD(pMsg->lParam);

			// ��ʾlistģʽ
			if ( !m_bShowAsTreeList && id == IDC_LIST_PROCESS )
			{
				MouseOverListCtrl(point);
				return CDialog::PreTranslateMessage(pMsg);
			}

			if (m_bShowAsTreeList && id != IDC_TAB && id != IDC_X_SP)
			{
				MouseOverCtrl(point, pMsg->hwnd);
			}
		}
	}
	
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE; 
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE; 
	if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4 )  return TRUE; //����ALT+F4
	
	return CDialog::PreTranslateMessage(pMsg);
}

//
// ������ģʽ����ʾtips��ʾ
//
void CListProcessDlg::MouseOverCtrl(CPoint point, HWND hWnd)
{
	CWnd* pWnd = FromHandle(hWnd);
	if(!pWnd)
		return;

	int id = pWnd->GetDlgCtrlID();
	if(id == IDC_TAB || id == IDC_X_SP)
	{
		return;
	}

	int nCol = -1;
	HTREEITEM hItem = ListTreeHitTestEx(point, &nCol);
	if (hItem && nCol == 0)
	{
		PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
		if (!pInfo)
		{
			return;
		}

		DWORD dwPid = pInfo->ulPid;
		if (dwPid <= 0)
		{
			return;
		}

		CString szCommandLine = m_clsProcess.GetProcessCmdLine(dwPid);
		CString szCurrentDirectory = m_clsProcess.GetProcessCurrentDirectory(dwPid);
		CString szStartedTime = FileTime2SystemTime(pInfo->nCreateTime);
		CString szPebAddress = m_clsProcess.GetPebAddress(dwPid);
		CString szTitle = m_processTree.GetItemText(hItem, eProcessTreeImageName);
		CString szEprocess;
		szEprocess.Format(L"0x%08X", pInfo->ulEprocess);
		CString sItemText = L"";
		
		if (!szCommandLine.IsEmpty())
		{
			sItemText += L"Command Line:\n";
			sItemText += szCommandLine;
			sItemText += L"\n";
		}

		if (!szCurrentDirectory.IsEmpty())
		{
			sItemText += L"Current Directory:\n";
			sItemText += L"      ";
			sItemText += szCurrentDirectory;
			sItemText += L"\n";
		}

		if (!szStartedTime.IsEmpty())
		{
			sItemText += L"Started Time:\n";
			sItemText += L"      ";
			sItemText += szStartedTime;
			sItemText += L"\n";
		}

		if (!szPebAddress.IsEmpty())
		{
			sItemText += L"Peb Address:\n";
			sItemText += L"      ";
			sItemText += szPebAddress;
			sItemText += L"\n";
		}

		if (!szEprocess.IsEmpty())
		{
			sItemText += L"Process Object:\n";
			sItemText += L"      ";
			sItemText += szEprocess;
		}

		UpdateData();

		CRect rect;
		pWnd->GetClientRect(&rect);

		//Calculate the client coordinates of the dialog window
		pWnd->ClientToScreen(&rect);
		pWnd->ClientToScreen(&point);
		ScreenToClient(&rect);
		ScreenToClient(&point);

		m_tipWindow.SetTipText(szTitle, sItemText);
		m_tipWindow.ShowTipWindow(CRect(point.x - 50, point.y - 9, point.x + 50, point.y + 9) , point, TWS_XTP_DROPSHADOW | TWS_XTP_ALPHASHADOW, 0, 0, FALSE, TRUE);
	}
}

//
// ������ģʽ�£��жϵ�ǰ������ڵ����
//
HTREEITEM CListProcessDlg::ListTreeHitTestEx(CPoint& point, int* nCol)
{
	int colnum = 0;
	CTVHITTESTINFO info;
	info.pt.x = point.x;
	info.pt.y = point.y/*+5*/;
	HTREEITEM row = m_processTree.HitTest(&info, FALSE);
	if (nCol) *nCol = -1;

	if (row)
	{
		*nCol = info.iSubItem;
	}

	return row;
}

//
// �����б�Hit���ԣ����ص�ǰ���������һ��item���Լ���һ��
//
int CListProcessDlg::ListHitTestEx(CPoint& point, int* nCol)
{
	int colnum = 0;
	int row = m_processList.HitTest(point, NULL);

	if (nCol) *nCol = 0;

	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong(m_processList.m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return row;

	// Get the top and bottom row visible
	row = m_processList.GetTopIndex();
	int bottom = row + m_processList.GetCountPerPage();
	if (bottom > m_processList.GetItemCount())
		bottom = m_processList.GetItemCount();

	// get a pointer to the header control, if NULL return error.
	CHeaderCtrl* pHeaderCtrl = m_processList.GetHeaderCtrl();
	if (pHeaderCtrl == NULL)
		return -1;

	// Get the number of columns
	int nColumnCount = pHeaderCtrl->GetItemCount();

	// Loop through the visible rows
	for (; row <= bottom; row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		m_processList.GetItemRect(row, &rect, LVIR_BOUNDS);
		if (rect.PtInRect(point))
		{
			// Now find the column
			for (colnum = 0; colnum < nColumnCount; colnum++)
			{
				int colwidth = m_processList.GetColumnWidth(Header_OrderToIndex(pHeaderCtrl->m_hWnd, colnum));
				if (point.x >= rect.left
					&& point.x <= (rect.left + colwidth))
				{
					if (nCol) *nCol = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

//
// �ں��л�õĽ��̴���ʱ��ת����ϵͳʱ��
//
CString CListProcessDlg::FileTime2SystemTime(ULONGLONG ulCreateTile)
{
	if (!ulCreateTile)
	{
		return L"-";
	}

	CString szRet;
	SYSTEMTIME SystemTime;
	FILETIME ft, ftLocal;
	
	ft.dwLowDateTime = (ULONG)ulCreateTile;
	ft.dwHighDateTime = (ULONG)(ulCreateTile >> 32);

	FileTimeToLocalFileTime(&ft, &ftLocal);

	if (FileTimeToSystemTime(&ftLocal, &SystemTime))
	{
		szRet.Format(
			L"%04d/%02d/%02d  %02d:%02d:%02d", 
			SystemTime.wYear,
			SystemTime.wMonth,
			SystemTime.wDay,
			SystemTime.wHour,
			SystemTime.wMinute,
			SystemTime.wSecond);
	}

	return szRet;
}

//
// �����б�ģʽ�£���ʾ����tips��Ϣ
//
void CListProcessDlg::MouseOverListCtrl(CPoint point)
{
	int nItem = -1, nCol = -1;
	
	// ������ڵ�һ���ϣ�����ʾ����
	if ((nItem = ListHitTestEx(point, &nCol)) != -1 && nCol == 0)
	{
		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo)
		{
			return;
		}

		DWORD dwPid = pInfo->ulPid;
		if (dwPid <= 0)
		{
			return;
		}

		CString m_szCommandLine = m_clsProcess.GetProcessCmdLine(dwPid);
		CString m_szCurrentDirectory = m_clsProcess.GetProcessCurrentDirectory(dwPid);
		CString m_szStartedTime = FileTime2SystemTime(pInfo->ulCreateTime);
		CString m_szPebAddress = m_clsProcess.GetPebAddress(dwPid);
		CString szTitle = m_processList.GetItemText(nItem, eProcessImageName);
		CString szEprocess;
		szEprocess.Format(L"0x%08X", pInfo->ulEprocess);
		CString sItemText = L"";

		if (!m_szCommandLine.IsEmpty())
		{
			sItemText += L"Command Line:\n";
			sItemText += m_szCommandLine;
			sItemText += L"\n";
		}

		if (!m_szCurrentDirectory.IsEmpty())
		{
			sItemText += L"Current Directory:\n";
			sItemText += L"      ";
			sItemText += m_szCurrentDirectory;
			sItemText += L"\n";
		}

		if (!m_szStartedTime.IsEmpty())
		{
			sItemText += L"Started Time:\n";
			sItemText += L"      ";
			sItemText += m_szStartedTime;
			sItemText += L"\n";
		}

		if (!m_szPebAddress.IsEmpty())
		{
			sItemText += L"Peb Address:\n";
			sItemText += L"      ";
			sItemText += m_szPebAddress;
			sItemText += L"\n";
		}

		if (!szEprocess.IsEmpty())
		{
			sItemText += L"Process Object:\n";
			sItemText += L"      ";
			sItemText += szEprocess;
		}

#if _MSC_VER >= 1200 // MFC 5.0
		int nOriginalCol = m_processList.GetHeaderCtrl()->OrderToIndex(nCol);
// 		CString sItemText = m_processList.GetItemText(nItem, nOriginalCol);
// 		int iTextWidth = m_processList.GetStringWidth(sItemText) + 5; //5 pixed extra size

		CRect rect;
		m_processList.GetSubItemRect(nItem, nOriginalCol, LVIR_LABEL, rect);

		// ������ʾ�����ʱ�����Tips��ʾ
// 		if(iTextWidth > rect.Width())
// 		{
			rect.top--;

			sItemText.Replace(_T("\r\n"), _T("\n")); //Avoid ugly outputted rectangle character in the tip window
			m_tipWindow.SetMargins(CSize(1,1));
			m_tipWindow.SetLineSpace(0);
			m_tipWindow.SetTipText(szTitle, sItemText);
			//Calculate the client coordinates of the dialog window
			m_processList.ClientToScreen(&rect);
			m_processList.ClientToScreen(&point);
			ScreenToClient(&rect);
			ScreenToClient(&point);

			//Show the tip window
/*			UpdateData();*/
			m_tipWindow.ShowTipWindow(rect, point, TWS_XTP_DROPSHADOW | TWS_XTP_ALPHASHADOW, 0, 0, FALSE, TRUE /*m_nDelayMillisecs, 0, false, m_bMoveTipBelowCursor*/);
//		}
#endif
	}
}

//
// ����pid��ȡ�����̵�image name��Ϣ
//
CString CListProcessDlg::GetParentProcessInfo(DWORD dwParentPid)
{
	CString szRet;
	CString szImage;
	BOOL bFind = FALSE;
	for ( vector <PROCESS_INFO>::iterator Iter = m_Ring0ProcessList.begin( ); Iter != m_Ring0ProcessList.end( ); Iter++)
	{
		if (Iter->ulPid == dwParentPid)
		{
			CString szPath = Iter->szPath;
			szImage = szPath.Right(szPath.GetLength() - szPath.ReverseFind('\\') - 1);
			bFind = TRUE;
			break;
		}
	}

	if (!bFind)
	{
		szImage = szNonExistentProcess[g_enumLang];
	}

	szRet.Format(L"%s (%d)", szImage, dwParentPid);
	return szRet;
}

//
// �����б�ģʽ����ʾ��ϸ��Ϣ
//
void CListProcessDlg::ShowProcessListDetailInfo(int nItem)
{
	if (nItem == -1)
	{
		return;
	}

	PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
	if (!pInfo)
	{
		return;
	}

	CProcessInfoDlg ProcessInfoDlg;
	ProcessInfoDlg.m_nPid = pInfo->ulPid;
	ProcessInfoDlg.m_pEprocess = pInfo->ulEprocess;
	ProcessInfoDlg.m_szImageName = m_processList.GetItemText(nItem, eProcessImageName);
	ProcessInfoDlg.m_szPath = TrimPath(pInfo->szPath);
	ProcessInfoDlg.m_szParentImage = GetParentProcessInfo(pInfo->ulParentPid);
	ProcessInfoDlg.DoModal();
}

//
// ˫�������б�
//
void CListProcessDlg::OnNMDblclkProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_processList.GetItemCount() > 0 &&
		m_processList.GetSelectedCount() == 1)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		if (nItem == -1)
		{
			return;
		}
	
		ShowProcessListDetailInfo(nItem);
	}

	*pResult = 0;
}

//
// ɨ����̹���
//
void CListProcessDlg::OnProcessHook()
{
	DWORD dwPid = 0, dwEprocess = 0;
	CString szPath = L"";

	if (!m_bShowAsTreeList)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		if (nItem == -1)
		{
			return;
		}

		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo)
		{
			return;
		}

		dwPid = pInfo->ulPid;
		dwEprocess = pInfo->ulEprocess;
		szPath = TrimPath(pInfo->szPath);
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
			if (pInfo)
			{
				dwPid = pInfo->ulPid;
				dwEprocess = pInfo->ulEprocess;
				szPath = pInfo->szPath;
			}
		}
	}

	if (dwPid && dwEprocess)
	{
		CProcessHookDlg Dlg;
		Dlg.m_nPid = dwPid;
		Dlg.m_pEprocess = dwEprocess;
		Dlg.m_szPath = szPath;
		Dlg.DoModal();
	}
}

//
// ����MINI dump
//
void CListProcessDlg::OnCreateMinidump()
{
	CString szPath = CreateOpenFileDlg();
	if (szPath.IsEmpty())
	{
		return;
	}

	if (CreateDump(MiniDumpNormal, szPath))
	{
		MessageBox(szCreateDumpOK[g_enumLang], szToolName, MB_OK);
	}
	else
	{
		MessageBox(szCreateDumpFailed[g_enumLang], szToolName, MB_OK);
	}
}

//
// ����FULL dump
//
void CListProcessDlg::OnCreateFulldump()
{
	CString szPath = CreateOpenFileDlg();
	if (szPath.IsEmpty())
	{
		return;
	}

	DWORD dwFlg = 
		MiniDumpWithFullMemory | 
		MiniDumpWithHandleData | 
		MiniDumpWithUnloadedModules | 
		2048 |  // MiniDumpWithFullMemoryInfo
		4096;	// MiniDumpWithThreadInfo

	BOOL bRet = CreateDump(dwFlg, szPath);

	if (!bRet && GetLastError() == HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER))
	{
		dwFlg = MiniDumpWithFullMemory | MiniDumpWithHandleData;
		bRet = CreateDump(dwFlg, szPath);
	}
 
	if (bRet)
	{
		MessageBox(szCreateDumpOK[g_enumLang], szToolName, MB_OK);
	}
	else
	{
		MessageBox(szCreateDumpFailed[g_enumLang], szToolName, MB_OK);
	}
}

//
// һ���ļ��򿪶Ի���
//
CString CListProcessDlg::CreateOpenFileDlg()
{
	CString szRet = L"";
	CString szImage = L"";

	if (!m_bShowAsTreeList)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		if (nItem != -1)
		{
			szImage = m_processList.GetItemText(nItem, eProcessImageName);
		}
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			szImage = m_processTree.GetItemText(hItem, eProcessTreeImageName);
		}
	}

	if (!szImage.IsEmpty())
	{
		CString szFileName = szImage;
		szFileName += L".dmp";
		
		/*L"Dump Files (*.dmp)|*.dmp;|All Files (*.*)|*.*||"*/
		WCHAR szFileFilter[] = {'D','u','m','p',' ','F','i','l','e','s',' ','(','*','.','d','m','p',')','|','*','.','d','m','p',';','|','A','l','l',' ','F','i','l','e','s',' ','(','*','.','*',')','|','*','.','*','|','|','\0'};
		CFileDialog fileDlg( FALSE, 0, szFileName, 0, szFileFilter, 0 );
		if (IDOK == fileDlg.DoModal())
		{
			CString szFilePath = fileDlg.GetFileName();

			if ( !PathFileExists(szFilePath) ||
				(PathFileExists(szFilePath) && MessageBox(szFileExist[g_enumLang], szToolName, MB_YESNO | MB_ICONQUESTION) == IDYES))
			{
				szRet = szFilePath;
			}
		}
	}

	return szRet;
}

//
// ����dump
//
BOOL CListProcessDlg::CreateDump(DWORD dwFlag, CString szPath)
{
	BOOL bRet = FALSE;
	DWORD dwPid = 0;

	if (!m_bShowAsTreeList)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		if (nItem == -1)
		{
			return bRet;
		}
		
		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo)
		{
			return bRet;
		}

		dwPid = pInfo->ulPid;
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (!hItem)
		{
			return bRet;
		}

		PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
		if (!pInfo)
		{
			return bRet;
		}

		dwPid = pInfo->ulPid;
	}
	
	if (!szPath.IsEmpty() && dwPid > 0)
	{
		HANDLE hFile = CreateFile(szPath, FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hProcess = m_ProcessFunc.OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);

		if (hFile != INVALID_HANDLE_VALUE && hProcess)
		{
			bRet = MiniDumpWriteDump(
				hProcess,
				dwPid,
				hFile,
				(MINIDUMP_TYPE)dwFlag,
				NULL,
				NULL,
				NULL
				);
		}

		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
		}

		if (hProcess)
		{
			CloseHandle(hProcess);
		}
	}		

	return bRet;
}

//
// �������ؼ���ǰѡ�����
//
HTREEITEM CListProcessDlg::GetTreeListSelectItem()
{
	HTREEITEM hItem = NULL;

	hItem = m_processTree.GetTreeCtrl().GetSelectedItem();

	return hItem;
}

//
// �������ؼ��Ҽ��˵�
//
void CListProcessDlg::OnRclickedColumntree(LPNMHDR pNMHDR, LRESULT* pResult)
{
	CMenu exportto;
	exportto.CreatePopupMenu();
	exportto.AppendMenu(MF_STRING, ID_EXPORT_TEXT, szText[g_enumLang]);
	exportto.AppendMenu(MF_STRING, ID_EXPORT_EXCEL, szExcel[g_enumLang]);

	WCHAR szMiniDump[] = {'M','i','n','i',' ','D','u','m','p','\0'};
	WCHAR szFullDump[] = {'F','u','l','l',' ','D','u','m','p','\0'};

	CMenu createdump;
	createdump.CreatePopupMenu();
	createdump.AppendMenu(MF_STRING, ID_CREATE_MINIDUMP, szMiniDump);
	createdump.AppendMenu(MF_STRING, ID_CREATE_FULLDUMP, szFullDump);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_PROCESS_REFRESH, szProcessRefresh[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_SHOW_PROCESS_TREE, szShowProcessTree[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_SHOW_ALL_PROCESS_INFO, szShowAllProcessInfo[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_KILL_PROCESS, szKillProcess[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_KILL_DELETE, szKillAndDeleteFile[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_KILL_PROCESS_TREE, szKillProcessTree[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_SUSPEND_PROCESS, szSuspend[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_RESUME_PROCESS, szResume[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_PROCESS_HOOK, szScanHooks[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_INJECT_DLL, szInjectDll[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_LISTPROCESS_DUMP, szProcessDump[g_enumLang]);
	menu.AppendMenu(MF_POPUP, (UINT)createdump.m_hMenu, szCreateDump[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_COPY_PROCESS_NAME, szCopyImageName[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_COPY_PROCESS_PATH, szCopyImagePath[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_CHECK_ATTRIBUTE, szProcessProperties[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_SEARCH_ONLINE, szSearchProcessOnline[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_LOCATION_EXPLORER, szFindInExplorer[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_PROCESS_LOCATION_AT_FILE_MANAGER, szFindInAntiSpy[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_CHECK_SIGN, szVerifyProcessSignature[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_LOOK_WINDOW_FOR_PROCESS, szFindProcessByWindwo[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_PROCESS_FIND_MODULE, szFindModule[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_PROCESS_FIND_UNSIGNED_MODUES, szFindUnsignedModules[g_enumLang]);
	
	// ����Ĭ�ϲ˵�
	menu.SetDefaultItem(ID_SHOW_ALL_PROCESS_INFO);

	// ���ò˵�ͼ��
	int x = GetSystemMetrics(SM_CXMENUCHECK);
	int y = GetSystemMetrics(SM_CYMENUCHECK);
	if (x >= 15 && y >= 15)
	{
		menu.SetMenuItemBitmaps(ID_PROCESS_REFRESH, MF_BYCOMMAND, &m_bmRefresh, &m_bmRefresh);
		menu.SetMenuItemBitmaps(ID_LOCATION_EXPLORER, MF_BYCOMMAND, &m_bmExplorer, &m_bmExplorer);
		menu.SetMenuItemBitmaps(ID_KILL_PROCESS, MF_BYCOMMAND, &m_bmDelete, &m_bmDelete);
		menu.SetMenuItemBitmaps(ID_KILL_DELETE, MF_BYCOMMAND, &m_bmDelete, &m_bmDelete);
		menu.SetMenuItemBitmaps(ID_KILL_PROCESS_TREE, MF_BYCOMMAND, &m_bmDelete, &m_bmDelete);
		menu.SetMenuItemBitmaps(ID_SEARCH_ONLINE, MF_BYCOMMAND, &m_bmSearch, &m_bmSearch);
		menu.SetMenuItemBitmaps(ID_CHECK_ATTRIBUTE, MF_BYCOMMAND, &m_bmShuxing, &m_bmShuxing);
		menu.SetMenuItemBitmaps(ID_COPY_PROCESS_NAME, MF_BYCOMMAND, &m_bmCopy, &m_bmCopy);
		menu.SetMenuItemBitmaps(ID_COPY_PROCESS_PATH, MF_BYCOMMAND, &m_bmCopy, &m_bmCopy);
		menu.SetMenuItemBitmaps(ID_PROCESS_LOCATION_AT_FILE_MANAGER, MF_BYCOMMAND, &m_bmAntiSpy, &m_bmAntiSpy);
		menu.SetMenuItemBitmaps(ID_SHOW_ALL_PROCESS_INFO, MF_BYCOMMAND, &m_bmDetalInof, &m_bmDetalInof);
		menu.SetMenuItemBitmaps(ID_CHECK_SIGN, MF_BYCOMMAND, &m_bmSign, &m_bmSign);
		menu.SetMenuItemBitmaps(17, MF_BYPOSITION, &m_bmWindbg, &m_bmWindbg);
		menu.SetMenuItemBitmaps(ID_LOOK_WINDOW_FOR_PROCESS, MF_BYCOMMAND, &m_bmLookFor, &m_bmLookFor);
		menu.SetMenuItemBitmaps(ID_PROCESS_FIND_MODULE, MF_BYCOMMAND, &m_bmLookFor, &m_bmLookFor);
		menu.SetMenuItemBitmaps(ID_PROCESS_FIND_UNSIGNED_MODUES, MF_BYCOMMAND, &m_bmLookFor, &m_bmLookFor);
	}

	CPoint pt;
	::GetCursorPos(&pt);

	m_processTree.ScreenToClient(&pt);

	CTVHITTESTINFO htinfo;
	htinfo.pt = pt;
	HTREEITEM hItem = m_processTree.HitTest(&htinfo);
	if (hItem)
	{
		m_processTree.GetTreeCtrl().SelectItem(hItem);
	}

	if (!m_imgList.GetImageCount()) // ���m_list�ǿյģ���ô���ˡ�ˢ�¡�������ȫ���û�
	{
		for (int i = 2; i < 33; i++)
		{
			menu.EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);
		}
	}
	else
	{
		if (!hItem)
		{
			for (int i = 2; i < 33; i++)
			{
				menu.EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);
			}
			
			menu.EnableMenuItem(ID_LOOK_WINDOW_FOR_PROCESS, MF_BYCOMMAND | MF_ENABLED);
			menu.EnableMenuItem(ID_PROCESS_FIND_MODULE, MF_BYCOMMAND | MF_ENABLED);
			menu.EnableMenuItem(ID_PROCESS_FIND_UNSIGNED_MODUES, MF_BYCOMMAND | MF_ENABLED);
		}
		else
		{
			SUSPEND_OR_RESUME_TYPE SorType = IsSuspendOrResumeProcessTree();
			switch (SorType)
			{
			case enumAllFalse:
				menu.EnableMenuItem(ID_SUSPEND_PROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				menu.EnableMenuItem(ID_RESUME_PROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				break;

			case enumAllOk:
				menu.EnableMenuItem(ID_SUSPEND_PROCESS, MF_BYCOMMAND | MF_ENABLED);
				menu.EnableMenuItem(ID_RESUME_PROCESS, MF_BYCOMMAND | MF_ENABLED);
				break;

			case enumResume:
				menu.EnableMenuItem(ID_SUSPEND_PROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				menu.EnableMenuItem(ID_RESUME_PROCESS, MF_BYCOMMAND | MF_ENABLED);
				break;

			case enumSuspend:
				menu.EnableMenuItem(ID_SUSPEND_PROCESS, MF_BYCOMMAND | MF_ENABLED);
				menu.EnableMenuItem(ID_RESUME_PROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				break;
			}
		}
	}
	
	menu.EnableMenuItem(ID_SHOW_PROCESS_TREE, MF_BYCOMMAND | MF_ENABLED);

	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();
	exportto.DestroyMenu();

	*pResult = 0;
}

//
// ������ģʽ����ʾ������ϸ��Ϣ
//
void CListProcessDlg::ShowProcessTreeDetailInfo(HTREEITEM hItem)
{
	if (!hItem)
	{
		return;
	}

	PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
	if (!pInfo)
	{
		return;
	}

	DebugLog(L"new DblclkColumntree");

	CProcessInfoDlg ProcessInfoDlg;
	ProcessInfoDlg.m_nPid = pInfo->ulPid;
	ProcessInfoDlg.m_pEprocess = pInfo->ulEprocess;
	ProcessInfoDlg.m_szImageName = m_processTree.GetItemText(hItem, eProcessTreeImageName);
	ProcessInfoDlg.m_szPath = pInfo->szPath;
	ProcessInfoDlg.m_szParentImage = GetParentProcessInfo(pInfo->ulParentPid);
	ProcessInfoDlg.DoModal();
}

//
// ���οؼ�˫������������ϸ��Ϣ
//
void CListProcessDlg::DblclkColumntree(HTREEITEM hItem)
{
	ShowProcessTreeDetailInfo(hItem);
}

//
// �Ҽ��˵� - ��ʾ������
//
void CListProcessDlg::OnShowProcessTree()
{
	m_bShowAsTreeList = !m_bShowAsTreeList;
	AdjustShowWindow();
	
	((CSetConfigDlg*)g_pSetConfigDlg)->m_bShowProcessTreeAsDeafult = m_bShowAsTreeList;
	((CSetConfigDlg*)g_pSetConfigDlg)->UpdateData(FALSE);

	OnProcessRefresh();
}

//
// �Ҽ��˵� - �Ƿ�ѡ����ʾ������
//
void CListProcessDlg::OnUpdateShowProcessTree(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowAsTreeList);
}

//
// �����ϵͳ����
//
void CListProcessDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	if(!bSysMenu && pPopupMenu)  
	{  
		CCmdUI cmdUI;  
		cmdUI.m_pOther = NULL;  
		cmdUI.m_pMenu = pPopupMenu;  
		cmdUI.m_pSubMenu = NULL;  

		UINT count = pPopupMenu->GetMenuItemCount();  
		cmdUI.m_nIndexMax = count;  
		for(UINT i=0; i<count; i++)  
		{  
			UINT nID = pPopupMenu->GetMenuItemID(i);  
			if(-1 == nID || 0 == nID)  
			{  
				continue;  
			}  
			cmdUI.m_nID = nID;  
			cmdUI.m_nIndex = i;  
			cmdUI.DoUpdate(this, FALSE);  
		}  
	}   
}

//
// ���������ݹ�����ӽ���
//
void CListProcessDlg::KillSubProcess(DWORD dwPid)
{
	for ( vector<PROCESS_INFO_EX>::iterator Iter = m_vectorProcessEx.begin( ); 
		Iter != m_vectorProcessEx.end( ); 
		Iter++)
	{
		if (Iter->ulParentPid == dwPid)
		{
			KillSubProcess(Iter->ulPid);
			m_clsProcess.KillProcess(0, Iter->ulEprocess);
		}
	}
}

//
// ����������
//
void CListProcessDlg::OnKillProcessTree()
{
	HTREEITEM hItem = GetTreeListSelectItem();
	if (!hItem)
	{
		return;
	}
	
	// ����ȷ��
	CString szImage = m_processTree.GetItemText(hItem, eProcessTreeImageName);
	CString szShow;
	szShow.Format(szAreYouSureKillDescendants[g_enumLang], szImage);
	if (MessageBox(szShow, szToolName, MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return;
	}
	
	PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
	if (!pInfo)
	{
		return;
	}

	KillSubProcess(pInfo->ulPid);
	m_clsProcess.KillProcess(0, pInfo->ulEprocess);
	
	Sleep(500);
	OnProcessRefresh();
}

//
// Զ��ע��ģ��
//
void CListProcessDlg::OnInjectDll()
{
	ULONG nPid = 0;

	if (m_bShowAsTreeList)
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem == NULL)
		{
			return;
		}
		
		PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
		if (!pInfo)
		{
			return;
		}

		nPid = pInfo->ulPid;
	}
	else
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		if (nItem == -1)
		{
			return;
		}
		
		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo)
		{
			return;
		}

		nPid = pInfo->ulPid;
	}
	
	if (nPid == 0)
	{
		return;
	}
	
	/*L"DLL Files(*.dll)\0*.dll\0All Files(*.*)\0*.*\0\0"*/
	WCHAR szDLLFilter[] = {'D','L','L',' ','F','i','l','e','s','(','*','.','d','l','l',')','\0','*','.','d','l','l','\0','A','l','l',' ','F','i','l','e','s','(','*','.','*',')','\0','*','.','*','\0','\0','\0'};
	WCHAR szInjectDLL[] = {'I','n','j','e','c','t',' ','D','L','L','\0'};
	CFileDialog fileDlg(TRUE);			
	fileDlg.m_ofn.lpstrTitle = szInjectDLL;
	fileDlg.m_ofn.lpstrFilter = szDLLFilter;

	if (IDOK != fileDlg.DoModal())
	{
		return;
	}

	CString strPath = fileDlg.GetPathName();
	if (PathFileExists(strPath))
	{
		WCHAR szPath[MAX_PATH] = {0};
		wcsncpy_s(szPath, strPath.GetBuffer(), strPath.GetLength());
		strPath.ReleaseBuffer();

		if (!m_clsProcess.InjectDllByRemoteThread(szPath, nPid))
		{
			::MessageBox(NULL, szInjectDllModuleFailed[g_enumLang], szToolName, MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			::MessageBox(NULL, szInjectDllModuleOk[g_enumLang], szToolName, MB_OK | MB_ICONINFORMATION);
		}
	}	
}

//
// ���������չʾ,��ʲôʱ�����ʾ��Щ�ؼ�
//
void CListProcessDlg::AdjustShowWindow()
{
	// ����������οؼ���ʾ
	if (m_bShowAsTreeList)
	{
		m_processTree.ShowWindow(TRUE);
		m_processList.ShowWindow(FALSE);

		m_ySplitter.AttachAsAbovePane(IDC_PROCESS_TREE);
		m_ySplitter.AttachAsBelowPane(IDC_TAB);
		m_ySplitter.RecalcLayout();
	}
	
	else
	{
		m_processList.ShowWindow(TRUE);
		m_processTree.ShowWindow(FALSE);
		
		m_ySplitter.AttachAsAbovePane(IDC_LIST_PROCESS);
		m_ySplitter.AttachAsBelowPane(IDC_TAB);
		m_ySplitter.RecalcLayout();
	}
}

//
// ö�ٽ���ģ��
//
void CListProcessDlg::EnumProcessModules(ULONG nPid, ULONG pEprocess, CString szImage)
{
	if (nPid == 0 && pEprocess == 0)
	{
		return;
	}

	ULONG nTotal = 0, nNotMicro = 0;
	m_ModuleDlg.SetProcessInfo(nPid, pEprocess, szImage, this->m_hWnd);
	m_ModuleDlg.EnumModules(&nTotal, &nNotMicro);

	// ����״̬
	CString szText;
	szText.Format(szProcessState[g_enumLang], m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);
	
	CString szModuleStatus;
	szModuleStatus.Format(szProcessModuleState[g_enumLang], szImage, nNotMicro, nTotal);
	
	m_szProcessCnt = szText + szModuleStatus;
	SendMessage(WM_UPDATE_PROCESS_DATA);
}

//
// ö�پ��
//
void CListProcessDlg::EnumProcessHandles(ULONG nPid, ULONG pEprocess, CString szImage)
{
	if (nPid == 0 && pEprocess == 0)
	{
		return;
	}
	
	ULONG nCnt = 0;
	m_HandleDlg.SetProcessInfo(nPid, pEprocess, szImage, this->m_hWnd);
	m_HandleDlg.EnumProcessHandles(&nCnt);

	// ����״̬
	CString szText;
	szText.Format(szProcessState[g_enumLang], m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);

	CString szModuleStatus;
	szModuleStatus.Format(szProcessHandleState[g_enumLang], szImage, nCnt);
	
	m_szProcessCnt = szText + szModuleStatus;
	SendMessage(WM_UPDATE_PROCESS_DATA);
}

//
// ö���߳�
//
void CListProcessDlg::EnumProcessThreads(ULONG nPid, ULONG pEprocess, CString szImage)
{
	if (nPid == 0 && pEprocess == 0)
	{
		return;
	}
	
	ULONG nCnt = 0;
	m_ThreadDlg.SetProcessInfo(nPid, pEprocess, szImage, this->m_hWnd);
	m_ThreadDlg.EnumThreads(&nCnt);

	// ����״̬
	CString szText;
	szText.Format(szProcessState[g_enumLang], m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);

	CString szModuleStatus;
	szModuleStatus.Format(szProcessThreadState[g_enumLang], szImage, nCnt);

	m_szProcessCnt = szText + szModuleStatus;
	SendMessage(WM_UPDATE_PROCESS_DATA);
}

//
// ö�ٽ����ڴ�
//
void CListProcessDlg::EnumProcessMemorys(ULONG nPid, ULONG pEprocess, CString szImage)
{
	if (nPid == 0 && pEprocess == 0)
	{
		return;
	}

	ULONG nCnt = 0;
	m_MemoryDlg.SetProcessInfo(nPid, pEprocess, szImage, this->m_hWnd);
	m_MemoryDlg.EnumMemorys(&nCnt);

	// ����״̬
	CString szText;
	szText.Format(szProcessState[g_enumLang], m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);

	CString szModuleStatus;
	szModuleStatus.Format(szProcessMemoryState[g_enumLang], szImage, nCnt);

	m_szProcessCnt = szText + szModuleStatus;
	SendMessage(WM_UPDATE_PROCESS_DATA);

}

//
// ö�ٴ���
//
void CListProcessDlg::EnumProcessWnds(ULONG nPid, ULONG pEprocess, CString szImage)
{
	if (nPid == 0 && pEprocess == 0)
	{
		return;
	}

	ULONG nCnt = 0;
	m_WndDlg.SetProcessInfo(nPid, pEprocess, szImage, this->m_hWnd);
	m_WndDlg.EnumProcessWnds(&nCnt);

	if (m_WndDlg.m_bShowAllProcess)
	{
		szImage = szAllProcesses[g_enumLang];
	}

	// ����״̬
	CString szText;
	szText.Format(szProcessState[g_enumLang], m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);

	CString szModuleStatus;
	szModuleStatus.Format(szProcessWindowsState[g_enumLang], szImage, nCnt);

	m_szProcessCnt = szText + szModuleStatus;
	SendMessage(WM_UPDATE_PROCESS_DATA);
}

//
// ö���ȼ�
//
void CListProcessDlg::EnumProcessHotKeys(ULONG nPid, ULONG pEprocess, CString szImage)
{
	if (nPid == 0 && pEprocess == 0)
	{
		return;
	}

	ULONG nCnt = 0;
	m_HotKeyDlg.SetProcessInfo(nPid, pEprocess, szImage, this->m_hWnd);
	m_HotKeyDlg.EnumHotKeys(&nCnt);

	if (m_HotKeyDlg.m_bShowAll)
	{
		szImage = szAllProcesses[g_enumLang];
	}

	// ����״̬
	CString szText;
	szText.Format(szProcessState[g_enumLang], m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);

	CString szModuleStatus;
	szModuleStatus.Format(szProcessHotKeysState[g_enumLang], szImage, nCnt);

	m_szProcessCnt = szText + szModuleStatus;
	SendMessage(WM_UPDATE_PROCESS_DATA);
}

//
// ö�ٶ�ʱ��
//
void CListProcessDlg::EnumProcessTimers(ULONG nPid, ULONG pEprocess, CString szImage)
{
	if (nPid == 0 && pEprocess == 0)
	{
		return;
	}
	
	ULONG nCnt = 0;
	m_TimerDlg.SetProcessInfo(nPid, pEprocess, szImage, this->m_hWnd);
	m_TimerDlg.EnumProcessTimers(&nCnt);
	
	if (m_TimerDlg.m_bShowAllTimer)
	{
		szImage = szAllProcesses[g_enumLang];
	}

	// ����״̬
	CString szText;
	szText.Format(szProcessState[g_enumLang], m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);

	CString szModuleStatus;
	szModuleStatus.Format(szProcessTimersState[g_enumLang], szImage, nCnt);

	m_szProcessCnt = szText + szModuleStatus;
	SendMessage(WM_UPDATE_PROCESS_DATA);
}

//
// ��ȡ��ǰ���̵�pid, eprocess, ��image name
//
CString CListProcessDlg::GetCurrentProcessInfo(ULONG &nPid, ULONG &pEprocess)
{
	CString szImage = L"";

	if (!m_bShowAsTreeList)
	{
		if (m_processList.GetSelectedCount() == 1)
		{
			int nItem = m_Functions.GetSelectItem(&m_processList);
			if (nItem != -1)
			{
				PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
				if (pInfo)
				{
					nPid = pInfo->ulPid;
					pEprocess = pInfo->ulEprocess;
					szImage = m_processList.GetItemText(nItem, eProcessImageName);
				}
			}
		}
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
			if (pInfo)
			{
				nPid = pInfo->ulPid;
				pEprocess = pInfo->ulEprocess;
				szImage = m_processTree.GetItemText(hItem, eProcessTreeImageName);
			}
		}
	}

	return szImage; 
}

//
// �����ˢ��,�ᷢ����Ϣ��ö�ٶ�ʱ����Ϣ
//
LRESULT CListProcessDlg::MsgEnumTimer(WPARAM wParam, LPARAM lParam)
{
	ULONG nPid = 0, pEprocess = 0;
	CString szImage = GetCurrentProcessInfo(nPid, pEprocess);
	EnumProcessTimers(nPid, pEprocess, szImage);
	return 0;
}

LRESULT CListProcessDlg::MsgEnumModules(WPARAM wParam, LPARAM lParam)
{
	ULONG nPid = 0, pEprocess = 0;
	CString szImage = GetCurrentProcessInfo(nPid, pEprocess);
	EnumProcessModules(nPid, pEprocess, szImage);
	return 0;
}

LRESULT CListProcessDlg::MsgEnumHandles(WPARAM wParam, LPARAM lParam)
{
	ULONG nPid = 0, pEprocess = 0;
	CString szImage = GetCurrentProcessInfo(nPid, pEprocess);
	EnumProcessHandles(nPid, pEprocess, szImage);
	return 0;
}

LRESULT CListProcessDlg::MsgEnumThreads(WPARAM wParam, LPARAM lParam)
{
	ULONG nPid = 0, pEprocess = 0;
	CString szImage = GetCurrentProcessInfo(nPid, pEprocess);
	EnumProcessThreads(nPid, pEprocess, szImage);
	return 0;
}

LRESULT CListProcessDlg::MsgEnumMemory(WPARAM wParam, LPARAM lParam)
{
	ULONG nPid = 0, pEprocess = 0;
	CString szImage = GetCurrentProcessInfo(nPid, pEprocess);
	EnumProcessMemorys(nPid, pEprocess, szImage);
	return 0;
}

LRESULT CListProcessDlg::MsgEnumWindows(WPARAM wParam, LPARAM lParam)
{
	ULONG nPid = 0, pEprocess = 0;
	CString szImage = GetCurrentProcessInfo(nPid, pEprocess);
	EnumProcessWnds(nPid, pEprocess, szImage);
	return 0;
}

LRESULT CListProcessDlg::MsgEnumHotKeys(WPARAM wParam, LPARAM lParam)
{
	ULONG nPid = 0, pEprocess = 0;
	CString szImage = GetCurrentProcessInfo(nPid, pEprocess);
	EnumProcessHotKeys(nPid, pEprocess, szImage);
	return 0;
}

LRESULT CListProcessDlg::MsgEnumPrivileges(WPARAM wParam, LPARAM lParam)
{
	ULONG nPid = 0, pEprocess = 0;
	CString szImage = GetCurrentProcessInfo(nPid, pEprocess);
	EnumProcessPrivileges(nPid, pEprocess, szImage);
	return 0;
}

//
// ö�ٽ���Ȩ��
//
void CListProcessDlg::EnumProcessPrivileges(ULONG nPid, ULONG pEprocess, CString szImage)
{
	if (nPid == 0 && pEprocess == 0)
	{
		return;
	}

	ULONG nEnable = 0, nDisable = 0;
	m_PrivilegeDlg.SetProcessInfo(nPid, pEprocess, szImage, this->m_hWnd);
	m_PrivilegeDlg.GetProcessPrivileges(&nEnable, &nDisable);

	// ����״̬
	CString szText;
	szText.Format(szProcessState[g_enumLang], m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);

	CString szModuleStatus;
	szModuleStatus.Format(szProcessPrivilegesState[g_enumLang], szImage, nEnable, nDisable);

	m_szProcessCnt = szText + szModuleStatus;
	SendMessage(WM_UPDATE_PROCESS_DATA);
}

//
// �����б�ģʽ�¸���ѡ�еĽ�����
//
static int preListItem = 0;
void CListProcessDlg::OnLvnItemchangedProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
// 	if (!m_bShowBelow)
// 	{
// 		*pResult = 0;
// 		return;
// 	}
// 
// 	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
// 	
// 	int nItem = pNMLV->iItem;
// 	if ( m_processList.GetSelectedCount() == 1 && nItem != -1 && nItem != preListItem ) // ѡ��һ����ʱ���ö��
// 	{
// 		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
// 		if (!pInfo)
// 		{
// 			return;
// 		}
// 
// 		CString szImage = m_processList.GetItemText(nItem, eProcessImageName);
// 		ULONG nPid = pInfo->ulPid;
// 		ULONG pEprocess = pInfo->ulEprocess;
// 
// 		switch (m_nCurSel)
// 		{
// 		case eDllModuleDlg:
// 			EnumProcessModules(nPid, pEprocess, szImage);
// 			break;
// 
// 		case eHandleDlg:
// 			EnumProcessHandles(nPid, pEprocess, szImage);
// 			break;
// 		}
// 		
// 		preListItem = nItem;
// 	}

	*pResult = 0;
}

static HTREEITEM preTreeItem = NULL;
void CListProcessDlg::OnTvnSelchangedTreeList(NMHDR *pNMHDR, LRESULT *pResult)
{
// 	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
// 
// 	if (!m_bShowBelow)
// 	{
// 		*pResult = 0;
// 		return;
// 	}
// 	
// 	HTREEITEM hItem = GetTreeListSelectItem();
// 	if (hItem && hItem != preTreeItem)
// 	{
// 		PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
// 		if (!pInfo)
// 		{
// 			return;
// 		}
// 
// 		CString szImage = m_processTree.GetItemText(hItem, eProcessTreeImageName);
// 		ULONG nPid = pInfo->ulPid;
// 		ULONG pEprocess = pInfo->ulEprocess;
// 
// 		switch (m_nCurSel)
// 		{
// 		case eDllModuleDlg:
// 			EnumProcessModules(nPid, pEprocess, szImage);
// 			break;
// 
// 		case eHandleDlg:
// 			EnumProcessHandles(nPid, pEprocess, szImage);
// 			break;
// 		}
// 
// 		preTreeItem = hItem;
// 	}

	*pResult = 0;
}

//
// ��λ��AntiSpy�ļ�������
//
void CListProcessDlg::OnProcessLocationAtFileManager()
{
	CString szPath = L"";

	if (!m_bShowAsTreeList)
	{
		int nItem = m_Functions.GetSelectItem(&m_processList);
		if (nItem == -1)
		{
			return;
		}

		szPath = m_processList.GetItemText(nItem, eProcessPath);
	}
	else
	{
		HTREEITEM hItem = GetTreeListSelectItem();
		if (hItem)
		{
			szPath = m_processTree.GetItemText(hItem, eProcessTreePath);
		}
	}

	m_Functions.JmpToFile(szPath);
}

//
// �����Զ�����ϢWM_UPDATE_PROCESS_DATA�����½����ϵ���Ϣ
//
LRESULT CListProcessDlg::OnUpdateProcessInfoStatus(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);
	return 0;
}

//
// �Ҽ��˵� - ���ݴ��ڲ��ҽ���
//
void CListProcessDlg::OnLookWindowForProcess()
{
	WCHAR szFind[] = {'m','Z','f','F','i','n','d','W','n','d','\0'};
	HANDLE hMutex = CreateMutex(NULL, TRUE, szFind);
	if (hMutex && GetLastError() == ERROR_ALREADY_EXISTS) 
	{
		CloseHandle(hMutex);
		return;
	} 

	if (hMutex)
	{
		CloseHandle(hMutex);
		hMutex = NULL;
	}

	CFindWindow *m_pFindRegDlg = new CFindWindow();
	m_pFindRegDlg->Create(IDD_FIND_WINDOW_DIALOG);
	m_pFindRegDlg->ShowWindow(SW_SHOWNORMAL);
}

//
// �������ָ���ҵ��������ڵ�PID����Ӧ�Ľ���
//
void CListProcessDlg::GotoProcess(DWORD dwPid)
{
	if (!dwPid)
	{
		return;
	}

	// ������б��ģʽ
	if (!m_bShowAsTreeList)
	{
		CString szPid;
		szPid.Format(L"%d", dwPid);

		// ѭ�������б�����ǰѡ�е����Ϊδѡ��
		POSITION pos = m_processList.GetFirstSelectedItemPosition();
		while (pos)
		{
			int nItem = m_processList.GetNextSelectedItem(pos);
			m_processList.SetItemState(nItem, 0, -1);
		}

		DWORD dwCnt = m_processList.GetItemCount();
		for (DWORD i = 0; i < dwCnt; i++)
		{
			// �Ƚ�PID�Ƿ�һ�£����һ�£���ô�������µ���Ϊѡ��
			if (!(m_processList.GetItemText(i, eProcessPid)).CompareNoCase(szPid))
			{
				m_processList.EnsureVisible(i, FALSE);
				m_processList.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				m_processList.SetFocus();
				break;
			}
		}
	}
	else
	{
		for (vector<PROCESS_INFO_EX>::iterator ir = m_vectorProcessEx.begin();
			ir != m_vectorProcessEx.end();
			ir++)
		{
			if (ir->ulPid == dwPid)
			{
				m_processTree.GetTreeCtrl().Select(ir->hItem, TVGN_FIRSTVISIBLE);
				m_processTree.GetTreeCtrl().SelectItem(ir->hItem);
				m_processTree.GetTreeCtrl().SetFocus();
				break;
			}
		}
	}
}

//
// �������ָ���ҵ��������ڵ�TID����Ӧ���߳�
//
void CListProcessDlg::GotoThread(DWORD dwPid, DWORD dwTid)
{
	// ����²���
	if (!dwPid || !dwTid)
	{
		return;
	}

	// ���ȶ�λ�½���
	GotoProcess(dwPid);

	// ��ʼ�л�TAB
	NMHDR nm; 
	nm.hwndFrom = m_tab.m_hWnd; 
	nm.code = TCN_SELCHANGE;
	m_tab.SetCurSel(eThreadDlg); 
	m_tab.SendMessage(WM_NOTIFY, IDC_TAB_MAIN, (LPARAM)&nm);
	
	//
	// ��λ����Ӧ���߳�
	//
	CString szTid;
	szTid.Format(L"%d", dwTid);

	// ѭ�������б�����ǰѡ�е����Ϊδѡ��
	POSITION pos = m_ThreadDlg.m_list.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_ThreadDlg.m_list.GetNextSelectedItem(pos);
		m_ThreadDlg.m_list.SetItemState(nItem, 0, -1);
	}

	DWORD dwCnt = m_ThreadDlg.m_list.GetItemCount();
	for (DWORD i = 0; i < dwCnt; i++)
	{
		// �Ƚ�PID�Ƿ�һ�£����һ�£���ô�������µ���Ϊѡ��
		if (!(m_ThreadDlg.m_list.GetItemText(i, 0)).CompareNoCase(szTid))
		{
			m_ThreadDlg.m_list.EnsureVisible(i, FALSE);
			m_ThreadDlg.m_list.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			m_ThreadDlg.m_list.SetFocus();
			break;
		}
	}
}

//
// ����PID������Ӧ�Ľ��̣���ɾ����Ӧ����
//
void CListProcessDlg::KillProcessByFindWindow(DWORD dwPid)
{
	if (!dwPid)
	{
		return;
	}
	
	if (!m_bShowAsTreeList)
	{
		CString szPid;
		szPid.Format(L"%d", dwPid);

		DWORD dwCnt = m_processList.GetItemCount();
		for (DWORD i = 0; i < dwCnt; i++)
		{
			if (!(m_processList.GetItemText(i, eProcessPid)).CompareNoCase(szPid))
			{
				PPROCESS_INFO pInfo = GetProcessInfoByItem(i);
				if (pInfo)
				{
					m_clsProcess.KillProcess(0, pInfo->ulEprocess);
					m_processList.DeleteItem(i);
				}

				break;
			}
		}
	}
	else
	{
		HTREEITEM hPre = GetTreeListSelectItem();
		PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hPre);
		if (pInfo)
		{
			m_clsProcess.KillProcess(0, pInfo->ulEprocess);

			// ����������ôˢ���½�����
			if (m_processTree.GetTreeCtrl().ItemHasChildren(hPre))
			{
				Sleep(500);
				OnProcessRefresh();
			}
			else
			{
				m_processTree.GetTreeCtrl().DeleteItem(hPre);
			}
		}
	}
}

//
// �ָ����ٱ��϶���ʱ��ᷢ��һ���Զ���WM_RESIZE_ALL_PROC_WND��Ϣ��
// �������²�������Ĵ���
//
LRESULT CListProcessDlg::ResizeAllProcWnd(WPARAM wParam, LPARAM lParam)
{
	UpdateBelowDlg();
	return 0;
}

//
// ����Ľ�����Ϣtab�����л�
//
void CListProcessDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_pBelowDialog[m_nCurSel]->ShowWindow(SW_HIDE);
	m_nCurSel = (BELOW_DLG_INDEX)m_tab.GetCurSel();
	m_pBelowDialog[m_nCurSel]->ShowWindow(SW_SHOW);
	
	ULONG nPid = 0, pEprocess = 0;
	CString szImage = GetCurrentProcessInfo(nPid, pEprocess);
	
	switch (m_nCurSel)
	{
	case eDllModuleDlg:
		EnumProcessModules(nPid, pEprocess, szImage);
		break;

	case eHandleDlg:
		EnumProcessHandles(nPid, pEprocess, szImage);
		break;

	case eThreadDlg:
		EnumProcessThreads(nPid, pEprocess, szImage);
		break;

	case eMemoryDlg:
		EnumProcessMemorys(nPid, pEprocess, szImage);
		break;

	case eWndDlg:
		EnumProcessWnds(nPid, pEprocess, szImage);
		break;

	case eHotKeyDlg:
		EnumProcessHotKeys(nPid, pEprocess, szImage);
		break;

	case eTimerDlg:
		EnumProcessTimers(nPid, pEprocess, szImage);
		break;

	case ePrivilegeDlg:
		EnumProcessPrivileges(nPid, pEprocess, szImage);
		break;
	}

	*pResult = 0;
}

//
// ����������б���ö��
//
void CListProcessDlg::OnNMClickListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (!m_bShowBelow)
	{
		*pResult = 0;
		return;
	}

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int nItem = pNMLV->iItem;
	if ( m_processList.GetSelectedCount() == 1 && nItem != -1 && nItem != preListItem ) // ѡ��һ����ʱ���ö��
	{
		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo)
		{
			return;
		}

		CString szImage = m_processList.GetItemText(nItem, eProcessImageName);
		ULONG nPid = pInfo->ulPid;
		ULONG pEprocess = pInfo->ulEprocess;

		switch (m_nCurSel)
		{
		case eDllModuleDlg:
			EnumProcessModules(nPid, pEprocess, szImage);
			break;

		case eHandleDlg:
			EnumProcessHandles(nPid, pEprocess, szImage);
			break;

		case eThreadDlg:
			EnumProcessThreads(nPid, pEprocess, szImage);
			break;

		case eMemoryDlg:
			EnumProcessMemorys(nPid, pEprocess, szImage);
			break;

		case eWndDlg:
			EnumProcessWnds(nPid, pEprocess, szImage);
			break;

		case eHotKeyDlg:
			EnumProcessHotKeys(nPid, pEprocess, szImage);
			break;

		case eTimerDlg:
			EnumProcessTimers(nPid, pEprocess, szImage);
			break;

		case ePrivilegeDlg:
			EnumProcessPrivileges(nPid, pEprocess, szImage);
			break;
		}

		preListItem = nItem;
	}

	*pResult = 0;
}

//
// �����������������ö��
//
void CListProcessDlg::OnNMClickTreeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if (!m_bShowBelow)
	{
		*pResult = 0;
		return;
	}
	
	HTREEITEM hItem = GetTreeListSelectItem();
	if (hItem && hItem != preTreeItem)
	{
		PPROCESS_INFO_EX pInfo = GetProcessInfoExByItem(hItem);
		if (!pInfo)
		{
			return;
		}

		CString szImage = m_processTree.GetItemText(hItem, eProcessTreeImageName);
		ULONG nPid = pInfo->ulPid;
		ULONG pEprocess = pInfo->ulEprocess;

		switch (m_nCurSel)
		{
		case eDllModuleDlg:
			EnumProcessModules(nPid, pEprocess, szImage);
			break;

		case eHandleDlg:
			EnumProcessHandles(nPid, pEprocess, szImage);
			break;

		case eThreadDlg:
			EnumProcessThreads(nPid, pEprocess, szImage);
			break;

		case eMemoryDlg:
			EnumProcessMemorys(nPid, pEprocess, szImage);
			break;

		case eWndDlg:
			EnumProcessWnds(nPid, pEprocess, szImage);
			break;

		case eHotKeyDlg:
			EnumProcessHotKeys(nPid, pEprocess, szImage);
			break;

		case eTimerDlg:
			EnumProcessTimers(nPid, pEprocess, szImage);
			break;

		case ePrivilegeDlg:
			EnumProcessPrivileges(nPid, pEprocess, szImage);
			break;
		}

		preTreeItem = hItem;
	}

	*pResult = 0;
}
