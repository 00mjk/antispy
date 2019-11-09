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
#include "stdafx.h"
#include "ListProcess.h"
#include "Function.h"

CListProcess::CListProcess()
{

}

CListProcess::~CListProcess()
{

}

//
// ö�ٽ���
//
BOOL CListProcess::EnumProcess(vector<PROCESS_INFO> &vectorProcess)
{
	OPERATE_TYPE opType = enumListProcess;
 	ULONG nCnt = 1000;
 	PALL_PROCESSES pProcessInfo = NULL;
 	BOOL bRet = FALSE, bLast = FALSE;

	vectorProcess.clear();

	do 
	{
		if (pProcessInfo)
		{
			free(pProcessInfo);
			pProcessInfo = NULL;
		}

		ULONG nSize = sizeof(ALL_PROCESSES) + nCnt * sizeof(PROCESS_INFO);

		pProcessInfo = (PALL_PROCESSES)GetMemory(nSize);

		if (pProcessInfo)
		{	
			bRet = g_ConnectDriver.CommunicateDriver(&opType, sizeof(OPERATE_TYPE), (PVOID)pProcessInfo, nSize, NULL);
		}

		nCnt += 1000;

	} while (!bRet && GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	if (bRet && pProcessInfo->nCnt > 0)
	{
		ULONG dwNum = pProcessInfo->nCnt;

		for (ULONG i = 0; i < dwNum; i++)
		{
			vectorProcess.push_back(pProcessInfo->ProcessInfo[i]);
		}

		bLast = TRUE;
	}

	if (pProcessInfo)
	{
		free(pProcessInfo);
		pProcessInfo = NULL;
	}

	return bLast;
}

BOOL UDgreaterFileTime( PROCESS_INFO elem1, PROCESS_INFO elem2 )
{
	return elem1.ulCreateTime < elem2.ulCreateTime;
}

//
// ö�ٵ��Ľ��̸��ݴ���ʱ���Ⱥ�����
//
void CListProcess::SortByProcessCreateTime(vector<PROCESS_INFO> &ProcListEx)
{
	sort( ProcListEx.begin( ), ProcListEx.end( ), UDgreaterFileTime );
}

//
// ɱ����
//
BOOL CListProcess::KillProcess(ULONG nPid, ULONG pEprocess)
{
	COMMUNICATE_PROCESS cP;
	cP.OpType = enumKillProcess;
	cP.op.KillProcess.nPid = nPid;
	cP.op.KillProcess.pEprocess = pEprocess;
	return g_ConnectDriver.CommunicateDriver(&cP, sizeof(COMMUNICATE_PROCESS), NULL, 0, NULL);
}

//
// �������
//
BOOL CListProcess::SuspendProcess(ULONG nPid, ULONG pEprocess)
{
	COMMUNICATE_PROCESS cp;
	cp.OpType = enumSuspendOrResumeProcess;
	cp.op.SuspendOrResumeProcess.nPid = nPid;
	cp.op.SuspendOrResumeProcess.pEprocess = pEprocess;
	cp.op.SuspendOrResumeProcess.bSuspend = 1;
	return g_ConnectDriver.CommunicateDriver(&cp, sizeof(COMMUNICATE_PROCESS), NULL, 0, NULL);
}

//
// �ָ�����
//
BOOL CListProcess::ResumeProcess(ULONG nPid, ULONG pEprocess)
{
	COMMUNICATE_PROCESS cp;
	cp.OpType = enumSuspendOrResumeProcess;
	cp.op.SuspendOrResumeProcess.nPid = nPid;
	cp.op.SuspendOrResumeProcess.pEprocess = pEprocess;
	cp.op.SuspendOrResumeProcess.bSuspend = 0;
	return g_ConnectDriver.CommunicateDriver(&cp, sizeof(COMMUNICATE_PROCESS), NULL, 0, NULL);
}

//
// Զ��ע��ģ��
//
BOOL CListProcess::InjectDllByRemoteThread(const TCHAR* ptszDllFile, DWORD dwProcessId)
{
	// ������Ч
	if (NULL == ptszDllFile || 0 == ::_tcslen(ptszDllFile) || dwProcessId == 0 || -1 == _taccess(ptszDllFile, 0))
	{
		return false;
	}

	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	DWORD dwSize = 0;
	TCHAR* ptszRemoteBuf = NULL;
	LPTHREAD_START_ROUTINE lpThreadFun = NULL;

	// ��ȡĿ����̾��
	hProcess = m_ProcessFunc.OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwProcessId);
	if (NULL == hProcess)
	{
		return false;
	}

	// ��Ŀ������з����ڴ�ռ�
	dwSize = (DWORD)::_tcslen(ptszDllFile) + 1;
	ptszRemoteBuf = (TCHAR*)::VirtualAllocEx(hProcess, NULL, dwSize * sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);
	if (NULL == ptszRemoteBuf)
	{
		::CloseHandle(hProcess);
		return false;
	}

	// ��Ŀ����̵��ڴ�ռ���д���������(ģ����)
	if (FALSE == ::WriteProcessMemory(hProcess, ptszRemoteBuf, (LPVOID)ptszDllFile, dwSize * sizeof(TCHAR), NULL))
	{
		::VirtualFreeEx(hProcess, ptszRemoteBuf, dwSize, MEM_DECOMMIT);
		::CloseHandle(hProcess);
		return false;
	}

	// �� Kernel32.dll �л�ȡ LoadLibrary ������ַ
#ifdef _UNICODE
	lpThreadFun = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
#else
	lpThreadFun = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryA");
#endif

	if (NULL == lpThreadFun)
	{
		::VirtualFreeEx(hProcess, ptszRemoteBuf, dwSize, MEM_DECOMMIT);
		::CloseHandle(hProcess);
		return false;
	}

	DebugLog(L"LoadLibraryW: 0x%08X", lpThreadFun);

	// ����Զ���̵߳��� LoadLibrary
	hThread = ::CreateRemoteThread(hProcess, NULL, 0, lpThreadFun, ptszRemoteBuf, 0, NULL);
	if (NULL == hThread)
	{
		::VirtualFreeEx(hProcess, ptszRemoteBuf, dwSize, MEM_DECOMMIT);
		::CloseHandle(hProcess);
		return false;
	}

	// �ȴ�Զ���߳̽���
	::WaitForSingleObject(hThread, INFINITE);
	// ����
	::VirtualFreeEx(hProcess, ptszRemoteBuf, dwSize, MEM_DECOMMIT);
	::CloseHandle(hThread);
	::CloseHandle(hProcess);

	return true;
}

//
// ��ȡ����������
//
CString CListProcess::GetProcessCmdLine(DWORD dwPid)
{
	CString szCommandLine = L"";

	if (!NtQueryInformationProcess)
	{
		return szCommandLine;
	}

	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;
	PEB                       Peb;
	PROCESS_PARAMETERS        ProcParam;
	DWORD                     dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	PVOID					  pCmdBuffer = NULL;

	// Get process handle
	hProcess = m_ProcessFunc.OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if (!hProcess)
		return FALSE;

	// Retrieve information
	status = NtQueryInformationProcess( hProcess,
		ProcessBasicInformation,
		(PVOID)&pbi,
		sizeof(PROCESS_BASIC_INFORMATION),
		NULL
		);

	if (status)
		goto cleanup;

	if (!ReadProcessMemory( hProcess,
		pbi.PebBaseAddress,
		&Peb,
		sizeof(PEB),
		&dwDummy
		)
		)
		goto cleanup;

	if (!ReadProcessMemory( hProcess,
		Peb.ProcessParameters,
		&ProcParam,
		sizeof(PROCESS_PARAMETERS),
		&dwDummy
		)
		)
		goto cleanup;

	lpAddress = ProcParam.CommandLine.Buffer;
	dwSize = ProcParam.CommandLine.Length;

	pCmdBuffer = malloc(dwSize + sizeof(WCHAR));
	if (!pCmdBuffer)
		goto cleanup;

	memset(pCmdBuffer, 0, dwSize + sizeof(WCHAR));
	if (!ReadProcessMemory( hProcess,
		lpAddress,
		pCmdBuffer,
		dwSize,
		&dwDummy
		)
		)
		goto cleanup;

	int nLen = (int)wcslen((WCHAR*)pCmdBuffer);
	int nOffset = 0;
	if (nLen > 100)
	{
		while (nLen >= 100)
		{
			WCHAR szTemp[101] = {0};
			wcsncpy_s(szTemp, 101, ((WCHAR*)pCmdBuffer + nOffset), 100);
			szCommandLine += L"      ";
			szCommandLine += szTemp;
			szCommandLine += L"\n";
			nLen -= 100;
			nOffset += 100;
		}

		if (nLen < 100 && nLen > 0)
		{
			//	szCommandLine += L"\n";
			WCHAR szTemp[101] = {0};
			wcsncpy_s(szTemp, 101, ((WCHAR*)pCmdBuffer + nOffset), nLen);
			szCommandLine += L"      ";
			szCommandLine += szTemp;
		}
	}
	else
	{
		szCommandLine += L"      ";
		szCommandLine += (WCHAR*)pCmdBuffer;
	}

cleanup:

	CloseHandle (hProcess);
	if (pCmdBuffer)
	{
		free(pCmdBuffer);
		pCmdBuffer = NULL;
	}

	return szCommandLine;
} 

//
// ��ȡ���̵�ǰ�ļ���
//
CString CListProcess::GetProcessCurrentDirectory(DWORD dwPid)
{
	CString szRet = L"";

	if (!NtQueryInformationProcess)
	{
		return szRet;
	}

	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;
	PEB                       Peb;
	PROCESS_PARAMETERS        ProcParam;
	DWORD                     dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	PVOID					  pCmdBuffer = NULL;

	// Get process handle
	hProcess = m_ProcessFunc.OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if (!hProcess)
		return FALSE;

	// Retrieve information
	status = NtQueryInformationProcess( hProcess,
		ProcessBasicInformation,
		(PVOID)&pbi,
		sizeof(PROCESS_BASIC_INFORMATION),
		NULL
		);

	if (status)
		goto cleanup;

	if (!ReadProcessMemory( hProcess,
		pbi.PebBaseAddress,
		&Peb,
		sizeof(PEB),
		&dwDummy
		)
		)
		goto cleanup;

	if (!ReadProcessMemory( hProcess,
		Peb.ProcessParameters,
		&ProcParam,
		sizeof(PROCESS_PARAMETERS),
		&dwDummy
		)
		)
		goto cleanup;

	lpAddress = ProcParam.CurrentDirectory.Buffer;
	dwSize = ProcParam.CurrentDirectory.Length;

	pCmdBuffer = malloc(dwSize + sizeof(WCHAR));
	if (!pCmdBuffer)
		goto cleanup;

	memset(pCmdBuffer, 0, dwSize + sizeof(WCHAR));
	if (!ReadProcessMemory( hProcess,
		lpAddress,
		pCmdBuffer,
		dwSize,
		&dwDummy
		)
		)
		goto cleanup;

	szRet = (WCHAR*)pCmdBuffer;

cleanup:

	CloseHandle (hProcess);
	if (pCmdBuffer)
	{
		free(pCmdBuffer);
		pCmdBuffer = NULL;
	}

	return szRet;
}

//
// ��ȡ��������ʱ��
//
CString CListProcess::GetProcessStartTime(DWORD dwPid)
{
	CString szRet;
	HANDLE hProcess = m_ProcessFunc.OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if (!hProcess)
		return szRet;

	FILETIME ftCt, a, b, c;
	if (GetProcessTimes( hProcess, &ftCt, &a, &b, &c))
	{
		FILETIME ftCtLocal;
		if (FileTimeToLocalFileTime(&ftCt, &ftCtLocal))
		{
			SYSTEMTIME SystemTime;
			if (FileTimeToSystemTime(&ftCtLocal, &SystemTime))
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
		}
	}

	return szRet;
}

//
// ��ȡ����PEB��Ϣ
//
CString CListProcess::GetPebAddress(DWORD dwPid)
{
	CString szRet = L"";

	if (!NtQueryInformationProcess)
	{
		return szRet;
	}

	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;

	// Get process handle
	hProcess = m_ProcessFunc.OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if (!hProcess)
		return szRet;

	// Retrieve information
	status = NtQueryInformationProcess( hProcess,
		ProcessBasicInformation,
		(PVOID)&pbi,
		sizeof(PROCESS_BASIC_INFORMATION),
		NULL
		);

	if (status)
		goto cleanup;

	szRet.Format(L"0x%08X", pbi.PebBaseAddress);

cleanup:

	CloseHandle (hProcess);

	return szRet;
}