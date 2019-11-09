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
// ProcessTimerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SpyHunter.h"
#include "ProcessTimerDlg.h"
#include <algorithm>

typedef enum _TIMER_HEADER_INDEX
{
	eTimerObject,
	eTimerOutValue,
	eTimerCallback,
	eTimerModulePath,
	eTimerTid,
	eTimerPid,
	eTimerProcessName,
}TIMER_HEADER_INDEX;

// CProcessTimerDlg �Ի���

IMPLEMENT_DYNAMIC(CProcessTimerDlg, CDialog)

CProcessTimerDlg::CProcessTimerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessTimerDlg::IDD, pParent)
{
	m_szImage = L"";
	m_dwEprocess = 0;
	m_dwPid = 0;
	m_bShowAllTimer = FALSE;
	m_nPreItem = 0;
	m_nCnt = 0;
	m_hDlgWnd = NULL;
}

CProcessTimerDlg::~CProcessTimerDlg()
{
	m_vectorTimersTemp.clear();
}

void CProcessTimerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT, m_edit);
}


BEGIN_MESSAGE_MAP(CProcessTimerDlg, CDialog)
	ON_WM_SIZE()
	ON_COMMAND(ID_TIMER_REFRESH, &CProcessTimerDlg::OnTimerRefresh)
	ON_COMMAND(ID_TIMER_SHOW_ALL_TIMER, &CProcessTimerDlg::OnTimerShowAllTimer)
	ON_COMMAND(ID_TIMER_REMOVE, &CProcessTimerDlg::OnTimerRemove)
	ON_COMMAND(ID_TIMER_EDIT_TIMER, &CProcessTimerDlg::OnTimerEditTimer)
	ON_COMMAND(ID_TIMER_EXPORT_TEXT, &CProcessTimerDlg::OnTimerExportText)
	ON_COMMAND(ID_TIMER_EXPORT_EXCEL, &CProcessTimerDlg::OnTimerExportExcel)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, &CProcessTimerDlg::OnNMRclickList)
	ON_UPDATE_COMMAND_UI(ID_TIMER_SHOW_ALL_TIMER, &CProcessTimerDlg::OnUpdateTimerShowAllTimer)
	ON_WM_INITMENUPOPUP()
	ON_EN_KILLFOCUS(IDC_EDIT, &CProcessTimerDlg::OnEnKillfocusEdit)
	ON_COMMAND(ID_TIMER_CHECK_SIGN, &CProcessTimerDlg::OnTimerCheckSign)
	ON_COMMAND(ID_TIMER_CHECK_ATTRIBUTE, &CProcessTimerDlg::OnTimerCheckAttribute)
	ON_COMMAND(ID_TIMER_LOCATION_EXPLORER, &CProcessTimerDlg::OnTimerLocationExplorer)
	ON_COMMAND(ID_TIMER_DISASSEMBLE, &CProcessTimerDlg::OnTimerDisassemble)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CProcessTimerDlg)
	EASYSIZE(IDC_LIST, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP

// CProcessTimerDlg ��Ϣ�������

BOOL CProcessTimerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT/* | LVS_EX_GRIDLINES*/);
	m_list.InsertColumn(eTimerObject, szTimerObject[g_enumLang], LVCFMT_LEFT, 100);
	m_list.InsertColumn(eTimerOutValue, szTimeOutValue[g_enumLang], LVCFMT_LEFT, 100);
	m_list.InsertColumn(eTimerCallback, szCallback[g_enumLang], LVCFMT_LEFT, 100);
	m_list.InsertColumn(eTimerModulePath, szTimerModulePath[g_enumLang], LVCFMT_LEFT, 380);
	m_list.InsertColumn(eTimerTid, szTid[g_enumLang], LVCFMT_LEFT, 70);
	m_list.InsertColumn(eTimerPid, szPid[g_enumLang], LVCFMT_LEFT, 70);
	m_list.InsertColumn(eTimerProcessName, szProcesseName[g_enumLang], LVCFMT_LEFT, 180);

	INIT_EASYSIZE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//
// ���ý��̵�����
//
void CProcessTimerDlg::SetProcessInfo(ULONG nPid, ULONG pEprocess, CString szImage, HWND hWnd)
{
	m_dwPid = nPid;
	m_dwEprocess = pEprocess;
	m_szImage = szImage;
	m_hDlgWnd = hWnd;
}

BOOL UDgreaterEprocess( TIMER_INFO elem1, TIMER_INFO elem2 )
{
	return elem1.pEprocess < elem2.pEprocess;
}

// 
// ����������
//
VOID CProcessTimerDlg::SortByEprocess()
{
	sort( m_vectorTimersTemp.begin( ), m_vectorTimersTemp.end( ), UDgreaterEprocess );
}

// 
// ö�ٽ��̶�ʱ��
//
void CProcessTimerDlg::EnumProcessTimers(ULONG *nCnt)
{
	// ����
	m_nCnt = 0;
	m_list.DeleteAllItems();
	m_vectorTimersTemp.clear();
	m_vectorTimers.clear();

	// ö��
	m_clsTimers.EnumTimers(m_vectorTimersTemp);
	if (m_vectorTimersTemp.empty())
	{
		return;
	}

	// ����������
	SortByEprocess();

	// ����
	InsertTimers();

	if (nCnt)
	{
		*nCnt = m_nCnt;
	}
}

//
// ���ݽ���EPROCESS���õ������б������һ��PROCESS_INFO�ṹ��Ϣ
//
PROCESS_INFO CProcessTimerDlg::FindProcessItem(vector<PROCESS_INFO> &vectorProcess, ULONG pEprocess)
{
	PROCESS_INFO item;

	memset(&item, 0, sizeof(PROCESS_INFO));

	if (vectorProcess.size() > 0 && pEprocess)
	{
		for ( vector <PROCESS_INFO>::iterator IterProcess = vectorProcess.begin( ); 
			IterProcess != vectorProcess.end( ); 
			IterProcess++ )
		{
			if (IterProcess->ulEprocess == pEprocess)
			{
				item = *IterProcess;
			}
		}
	}

	return item;
}

//
// ring3ö�ٽ��̵�ģ�飬Ϊ�˼ӿ�ö���ٶȣ����Բ�ʹ������
//
BOOL CProcessTimerDlg::GetProcessModuleBySnap(DWORD dwPID) 
{ 
	BOOL bRet    =    FALSE; 
	BOOL bFound    =    TRUE; 
	HANDLE hModuleSnap = NULL; 
	MODULEENTRY32 me32 ={0}; 

	m_TimerModuleList.clear();
	hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if(hModuleSnap == INVALID_HANDLE_VALUE) 
	{    
		return FALSE; 
	} 

	me32.dwSize = sizeof(MODULEENTRY32); 
	if(::Module32First(hModuleSnap, &me32))//��õ�һ��ģ�� 
	{ 
		do{ 

			TIMER_MODULE_INFO info;
			info.nBase = (ULONG)me32.modBaseAddr;
			info.nSize = me32.modBaseSize;
			info.szModule = TrimPath(me32.szExePath);
			m_TimerModuleList.push_back(info);

		}while(::Module32Next(hModuleSnap, &me32)); 
	}//�ݹ�ö��ģ�� 

	CloseHandle(hModuleSnap); 
	return bFound; 
} 

//
// ������̶�ʱ����
//
VOID CProcessTimerDlg::InsertTimers()
{
	// ��ʾ���ж�ʱ��
	if (m_bShowAllTimer)
	{
		vector<PROCESS_INFO> vectorProcess;
		CListProcess ListProc;
		ListProc.EnumProcess(vectorProcess);
		
		ULONG nPeprocess = 0;
		PROCESS_INFO info;
		BOOL bModule = FALSE;

		for ( vector <TIMER_INFO>::iterator Iter = m_vectorTimersTemp.begin( ); 
			Iter != m_vectorTimersTemp.end( ); 
			Iter++ )
		{
			// �����ҵ������ʱ�������ĸ�����
			if (nPeprocess != Iter->pEprocess)
			{
				info = FindProcessItem(vectorProcess, Iter->pEprocess);
				nPeprocess = info.ulEprocess;
				bModule = FALSE;
			}
			
			// Ȼ��ʼö��
			CString szProcessName, szPid;
			CString strPath = info.szPath;
			szProcessName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\') - 1); 
			szPid.Format(L"%d", info.ulPid);

			CString szTimerObject, szCallback, szInterval, szTid, szModePath;
			szTimerObject.Format(L"0x%08X", Iter->TimerObject);

			if (Iter->pCallback)
			{
				if (!bModule)
				{
					GetProcessModuleBySnap(info.ulPid);
					bModule = TRUE;
				}

				szCallback.Format(L"0x%08X", Iter->pCallback);
				szModePath = GetModulePathByCallbackAddress(Iter->pCallback);
			}
			else
			{
				szCallback = L"-";
			}

			szInterval.Format(L"%.2f", Iter->nInterval / 1000.0);
			szTid.Format(L"%d", Iter->tid);

			int n = m_list.InsertItem(m_list.GetItemCount(), szTimerObject);
			m_list.SetItemText(n, eTimerOutValue, szInterval);
			m_list.SetItemText(n, eTimerCallback, szCallback);
			m_list.SetItemText(n, eTimerModulePath, szModePath);
			m_list.SetItemText(n, eTimerTid, szTid);
			m_list.SetItemText(n, eTimerPid, szPid);
			m_list.SetItemText(n, eTimerProcessName, szProcessName);

			m_vectorTimers.push_back(*Iter);
			m_list.SetItemData(n, m_nCnt);
			m_nCnt++;
		}
	}
	else
	{	
		BOOL bEnumModule = FALSE;
		for ( vector <TIMER_INFO>::iterator Iter = m_vectorTimersTemp.begin( ); 
			Iter != m_vectorTimersTemp.end( ); 
			Iter++ )
		{
			// ����ǵ�ǰ���̾Ͳ���
			if (Iter->pEprocess == m_dwEprocess)
			{
				CString szTimerObject, szCallback, szInterval, szTid, szModePath, szPid;

				szTimerObject.Format(L"0x%08X", Iter->TimerObject);

				if (Iter->pCallback)
				{
					if (!bEnumModule)
					{
						GetProcessModuleBySnap(m_dwPid);
						bEnumModule = TRUE;
					}
				
					szCallback.Format(L"0x%08X", Iter->pCallback);
					szModePath = GetModulePathByCallbackAddress(Iter->pCallback);
				}
				else
				{
					szCallback = L"-";
				}

				szInterval.Format(L"%.2f", Iter->nInterval / 1000.0);
				szTid.Format(L"%d", Iter->tid);
				szPid.Format(L"%d", m_dwPid);

				int n = m_list.InsertItem(m_list.GetItemCount(), szTimerObject);
				m_list.SetItemText(n, 1, szInterval);
				m_list.SetItemText(n, 2, szCallback);
				m_list.SetItemText(n, 3, szModePath);
				m_list.SetItemText(n, 4, szTid);
				m_list.SetItemText(n, 5, szPid);
				m_list.SetItemText(n, 6, m_szImage);

				m_vectorTimers.push_back(*Iter);
				m_list.SetItemData(n, m_nCnt);
				m_nCnt++;
			}
		}
	}
}

// 
// ����callback�ĵ�ַ�ҵ�ģ��
//
CString CProcessTimerDlg::GetModulePathByCallbackAddress(DWORD dwAddress)
{
	if (dwAddress >= 0x80000000)
	{
		if (m_vectorDrivers.empty())
		{
			m_clsDrivers.ListDrivers(m_vectorDrivers);
		}

		return GetDriverPath(dwAddress);
	}
	else
	{
		for ( list<TIMER_MODULE_INFO>::iterator Iter = m_TimerModuleList.begin( ); 
			Iter != m_TimerModuleList.end( ); 
			Iter++)
		{	
			if (dwAddress >= Iter->nBase && dwAddress <= (Iter->nBase + Iter->nSize))
			{
				return Iter->szModule;
			}
		}
	}

	return NULL;
}

void CProcessTimerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

//
// �Ҽ�- ˢ��
//
void CProcessTimerDlg::OnTimerRefresh()
{
//	EnumProcessTimers();
	::SendMessage(m_hDlgWnd, WM_ENUM_TIMER, NULL, NULL);
}

//
// ��ʾȫ�����̶�ʱ��
//
void CProcessTimerDlg::OnTimerShowAllTimer()
{
	m_bShowAllTimer = !m_bShowAllTimer;
	OnTimerRefresh();
}

//
// �Ƴ���ʱ��
//
void CProcessTimerDlg::OnTimerRemove()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_list.GetNextSelectedItem(pos);
		int nData = (int)m_list.GetItemData(nItem);
		TIMER_INFO info = m_vectorTimers.at(nData);

		m_clsTimers.RemoveTimer(info.TimerObject);

		m_list.DeleteItem(nItem);
		pos = m_list.GetFirstSelectedItemPosition();
	}
}

//
// �޸Ķ�ʱ����ʱ��
// 
void CProcessTimerDlg::OnTimerEditTimer()
{
	int nItem = m_Functions.GetSelectItem(&m_list);
	if (nItem == -1)
	{
		return;
	}
	
	m_nPreItem = nItem;

	CRect rc;
	m_list.GetSubItemRect(nItem, eTimerOutValue, LVIR_LABEL, rc);
	rc.left += 3;
	rc.top += 1;
	rc.right += 3;
	rc.bottom += 4;

	CString szOutTime = m_list.GetItemText(nItem, eTimerOutValue);
	m_edit.SetWindowText(szOutTime);
	m_edit.MoveWindow(&rc);
	m_edit.ShowWindow(SW_SHOW);
	m_edit.SetFocus();
	m_edit.CreateSolidCaret(1, rc.Height() - 5);
	m_edit.ShowCaret(); 
	m_edit.SetSel(-1);
}

// 
// �������ı�
//
void CProcessTimerDlg::OnTimerExportText()
{
	CString szImage = L"";

	if (m_bShowAllTimer)
	{
		szImage = szAllProcesses[g_enumLang];
	}
	else
	{
		szImage = m_szImage;
	}

	CString szStatus;
	szStatus.Format(szTimerStateNew[g_enumLang], m_nCnt);
	CString szText = L"[" + szImage + L"] " + szStatus;

	m_Functions.ExportListToTxt(&m_list, szText);
}

//
// ������excel
//
void CProcessTimerDlg::OnTimerExportExcel()
{
	WCHAR szTimer[] = {'T','i','m','e','r','\0'};
	CString szImage = L"";

	if (m_bShowAllTimer)
	{
		szImage = szAllProcesses[g_enumLang];
	}
	else
	{
		szImage = m_szImage;
	}

	CString szStatus;
	szStatus.Format(szTimerStateNew[g_enumLang], m_nCnt);
	CString szText = L"[" + szImage + L"] " + szStatus;
	m_Functions.ExportListToExcel(&m_list, szTimer, szText);
}

void CProcessTimerDlg::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu exportto;
	exportto.CreatePopupMenu();
	exportto.AppendMenu(MF_STRING, ID_TIMER_EXPORT_TEXT, szText[g_enumLang]);
	exportto.AppendMenu(MF_STRING, ID_TIMER_EXPORT_EXCEL, szExcel[g_enumLang]);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_TIMER_REFRESH, szRefresh[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_TIMER_SHOW_ALL_TIMER, szShowAllProcessesTimer[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_TIMER_DISASSEMBLE, szDisassembleCallback[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_TIMER_REMOVE, szRemove[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_TIMER_EDIT_TIMER, szModifyTimeOutValue[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
// 	menu.AppendMenu(MF_STRING, ID_TIMER_CHECK_SIGN, szVerifyStartModuleSignature[g_enumLang]);
// 	menu.AppendMenu(MF_STRING, ID_TIMER_CHECK_ATTRIBUTE, szStartModuleProperties[g_enumLang]);
// 	menu.AppendMenu(MF_STRING, ID_TIMER_LOCATION_EXPLORER, szFindStartModuleInExplorer[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_TIMER_CHECK_ATTRIBUTE, szCallbackModuleProperties[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_TIMER_LOCATION_EXPLORER, szCallbackFindInExplorer[g_enumLang]);
	menu.AppendMenu(MF_STRING, ID_TIMER_CHECK_SIGN, szCallbackVerifyModuleSignature[g_enumLang]);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_POPUP, (UINT)exportto.m_hMenu, szExportTo[g_enumLang]);

	int x = GetSystemMetrics(SM_CXMENUCHECK);
	int y = GetSystemMetrics(SM_CYMENUCHECK);
	if (x >= 15 && y >= 15)
	{
		// ���ò˵�ͼ��
		menu.SetMenuItemBitmaps(ID_TIMER_REFRESH, MF_BYCOMMAND, &m_bmRefresh, &m_bmRefresh);
		menu.SetMenuItemBitmaps(ID_TIMER_DISASSEMBLE, MF_BYCOMMAND, &m_bmOD, &m_bmOD);
		menu.SetMenuItemBitmaps(ID_TIMER_REMOVE, MF_BYCOMMAND, &m_bmDelete, &m_bmDelete);
		menu.SetMenuItemBitmaps(ID_TIMER_CHECK_ATTRIBUTE, MF_BYCOMMAND, &m_bmShuxing, &m_bmShuxing);
		menu.SetMenuItemBitmaps(ID_TIMER_LOCATION_EXPLORER, MF_BYCOMMAND, &m_bmExplorer, &m_bmExplorer);
		menu.SetMenuItemBitmaps(ID_TIMER_CHECK_SIGN, MF_BYCOMMAND, &m_bmSign, &m_bmSign);
		menu.SetMenuItemBitmaps(13, MF_BYPOSITION, &m_bmExport, &m_bmExport);
	}

	if (!m_list.GetItemCount()) // ���m_list�ǿյģ���ô���ˡ�ˢ�¡�������ȫ���û�
	{
		for (int i = 4; i < 14; i++)
		{
			menu.EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);
		}
	}
	else
	{
		if (!m_list.GetSelectedCount()) 
		{
			menu.EnableMenuItem(ID_TIMER_REMOVE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
			menu.EnableMenuItem(ID_TIMER_DISASSEMBLE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
			menu.EnableMenuItem(ID_TIMER_EDIT_TIMER, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
			menu.EnableMenuItem(ID_TIMER_CHECK_SIGN, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
			menu.EnableMenuItem(ID_TIMER_CHECK_ATTRIBUTE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
			menu.EnableMenuItem(ID_TIMER_LOCATION_EXPLORER, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		}
		else if (m_list.GetSelectedCount() >= 2)
		{
			menu.EnableMenuItem(ID_TIMER_DISASSEMBLE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
			menu.EnableMenuItem(ID_TIMER_EDIT_TIMER, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
			menu.EnableMenuItem(ID_TIMER_CHECK_SIGN, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
			menu.EnableMenuItem(ID_TIMER_CHECK_ATTRIBUTE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
			menu.EnableMenuItem(ID_TIMER_LOCATION_EXPLORER, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		}
		else if (m_list.GetSelectedCount() == 1)
		{
			POSITION pos = m_list.GetFirstSelectedItemPosition();
			if (pos != NULL)
			{
				int nItem = m_list.GetNextSelectedItem(pos);
				CString szModule = m_list.GetItemText(nItem, 3);
				if (szModule.IsEmpty())
				{
					menu.EnableMenuItem(ID_TIMER_CHECK_SIGN, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
					menu.EnableMenuItem(ID_TIMER_CHECK_ATTRIBUTE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
					menu.EnableMenuItem(ID_TIMER_LOCATION_EXPLORER, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				}

				CString szAddress = m_list.GetItemText(nItem, 2);
				if (szAddress.IsEmpty() || !szAddress.CompareNoCase(L"-"))
				{
					menu.EnableMenuItem(ID_TIMER_DISASSEMBLE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				}
			}
		}
	}

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();
	exportto.DestroyMenu();

	*pResult = 0;
}

//
// ��ʾȫ�����̵Ĺ�
//
void CProcessTimerDlg::OnUpdateTimerShowAllTimer(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowAllTimer);
}

void CProcessTimerDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
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
		for(UINT i=0; i < count; i++)  
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
// �޸�ʱ����ı���ʧȥ����֮��
//
void CProcessTimerDlg::OnEnKillfocusEdit()
{
	CString szOutTime;
	m_edit.GetWindowText(szOutTime);
	if (szOutTime.IsEmpty())
	{
		return;
	}
	
	double dbTimeOut = _wtof(szOutTime);
	if (dbTimeOut == 0.0)
	{
		return;
	}
	
	ULONG ulTime = (ULONG)(dbTimeOut * 1000);
	if (ulTime >= USER_TIMER_MINIMUM && ulTime <= USER_TIMER_MAXIMUM)
	{
		int nData = (int)m_list.GetItemData(m_nPreItem);
		TIMER_INFO info = m_vectorTimers.at(nData);
		
		if (m_clsTimers.ModifyTimeOut(info.TimerObject, ulTime))
		{
			CString szTime;
			szTime.Format(L"%.2f", dbTimeOut);
			m_list.SetItemText(m_nPreItem, eTimerOutValue, szTime);
		}
	}

	m_edit.ShowWindow(SW_HIDE);
}

BOOL CProcessTimerDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->hwnd == (GetDlgItem(IDC_EDIT))->GetSafeHwnd() && pMsg->message == WM_KEYDOWN)
	{		
		char c = (char)pMsg->wParam; 
		if (c == VK_RETURN)
		{
			OnEnKillfocusEdit();
		}
	}

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE; 
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE; 

	return CDialog::PreTranslateMessage(pMsg);
}

//
// ���ģ��ǩ��
//
void CProcessTimerDlg::OnTimerCheckSign()
{
	int nItem = m_Functions.GetSelectItem(&m_list);
	CString szPath = m_list.GetItemText(nItem, eTimerModulePath);
	m_Functions.SignVerify(szPath);
}

//
// ����ļ�����
//
void CProcessTimerDlg::OnTimerCheckAttribute()
{
	int nItem = m_Functions.GetSelectItem(&m_list);
	CString szPath = m_list.GetItemText(nItem, eTimerModulePath);
	m_Functions.OnCheckAttribute(szPath);
}

//
// ��λ���ļ�
//
void CProcessTimerDlg::OnTimerLocationExplorer()
{
	int nItem = m_Functions.GetSelectItem(&m_list);
	CString szPath = m_list.GetItemText(nItem, eTimerModulePath);
	m_Functions.LocationExplorer(szPath);
}

//
// ���ݵ�ַ�ҵ���Ӧ������ģ��
//
CString CProcessTimerDlg::GetDriverPath(ULONG pCallback)
{
	CString szPath = L"";

	for (vector<DRIVER_INFO>::iterator itor = m_vectorDrivers.begin(); 
		itor != m_vectorDrivers.end(); 
		itor++)
	{
		ULONG nBase = itor->nBase;
		ULONG nEnd = itor->nBase + itor->nSize;

		if (pCallback >= nBase && pCallback <= nEnd)
		{
			szPath = itor->szDriverPath;
			break;
		}
	}

	return szPath;
}

//
// ����ඨʱ�����
//
void CProcessTimerDlg::OnTimerDisassemble()
{
	int nItem = m_Functions.GetSelectItem(&m_list);
	if (nItem == -1)
	{
		return;
	}
	
	CString szAddress = m_list.GetItemText(nItem, eTimerCallback);
	CString szPid = m_list.GetItemText(nItem, eTimerPid);
	DWORD dwAddress = m_Functions.HexStringToLong(szAddress);
	DWORD dwPid = 0;
	if (!szPid.IsEmpty())
	{
		swscanf_s(szPid.GetBuffer(), L"%d", &dwPid);
		szPid.ReleaseBuffer();
	}

	if (dwAddress > 0)
	{
		m_Functions.Disassemble(dwAddress, 100, dwPid, 0);
	}
}
