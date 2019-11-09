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
/********************************************************************
	created:	2012/06/06	13:36
	file:		Control.c
	author:		Ī�һ�
	blog��		http://blog.csdn.net/hu3167343

	purpose:	
*********************************************************************/

#include "Control.h"
#include "..\\..\\Common\\Common.h"
#include "InitWindows.h"
#include "Process.h"
#include "ProcessModule.h"
#include "ProcessThread.h"
#include "ProcessHandle.h"
#include "processmemory.h"
#include "ProcessWnd.h"
#include "ProcessTimer.h"
#include "HotKeys.h"
#include "ProcessPrivileges.h"
#include "Driver.h"
#include "Reg.h"
#include "Callback.h"
#include "DpcTimer.h"
#include "IoTimer.h"
#include "WorkerThread.h"
#include "DebugRegister.h"
#include "DirectIo.h"
#include "Filter.h"
#include "SSDT.h"
#include "ShadowSSDT.h"
#include "fsd.h"
#include "DispatchHook.h"
#include "kbdclass.h"
#include "mouclass.h"
#include "classpnp.h"
#include "Atapi.h"
#include "Acpi.h"
#include "Tcpip.h"
#include "Idt.h"
#include "CommonFunction.h"
#include "ObjectHook.h"
#include "KernelEntry.h"
#include "IatEat.h"
#include "ModifiedCode.h"
#include "MessageHook.h"
#include "DeleteFile.h"
#include "Disassmbly.h"
#include "ProcessFunc.h"
#include "FileOperation.h"
#include "Port.h"
#include "KernelMemory.h"
#include "SelfProtectSSDT.h"
#include "FakeFunc.h"
#include "Power.h"
#include "FileFunc.h"
#include "MBR.h"
#include "ParseNTFS.h"

NTSTATUS ComVersion(PVOID pInBuffer, ULONG uInSize, PVOID pOutBuffer, ULONG uOutSize, ULONG* dwRet);

typedef NTSTATUS (*pfnFunction)(PVOID pInBuffer, ULONG uInSize, PVOID pOutBuffer, ULONG uOutSize, ULONG* dwRet);

pfnFunction FunctionArray[] = {
	NULL,
	ReadSector,
	SetBackupDriverEvent,
	SetBackupDriverPath,
	ReadMBR,
	MzfReadFileEx,
	WriteMBR,
	MzfWriteFileEx,
	MzfCreateFileEx,
	Forbids,
	Powers,
	ModifyKernelMemory,
	QueryLockedRegistryHandle,
	SetProcessPid,
	StartOrStopSelfProtect,
	SetFileNormalAttributeDispatch,
	CloseFileHandleDispatch,
	QueryFileLockInformationDispatch,
	CreateFileDispatch,
	QueryDirectoryFileDispatch,
	ComVersion,
	KernelQueryVirtualMemory,
	ListProcess,							// ö�ٽ���
	KillProcess,							// ɱ����
	DumpProcess,							// ���������ڴ�
	ProcessMenu,							// �����Ҽ��˵�,�жϽ����Ƿ�ɹ����
	SuspendOrResumeProcess,					// ������߻ָ���������
	ListProcessModule,						// ö�ٽ���ģ��
	DumpDllModule,							// ����ģ���ڴ�
	UnloadDllModule,						// ж�ؽ���ģ��					
	ListProcessThread,						// ö�ٽ����߳�
	KillThread,								// ɱ�߳�
	SuspendOrResumeThread,					// ������߻ָ��߳�����
	GetThreadSuspendCount,					// ��ȡ�̵߳Ĺ������
	ListProcessHandle,						// ö�ٽ��̾��
	KillHandle,								// ɱ���
	ListProcessMemory,						// ö�ٽ����ڴ�
	FreeProcessMemory,						// �ͷ��ڴ�
	ModifyMemoryProtection,					// �����ڴ汣������
	ZeroProcessMemory,						// �����ڴ�
	ListProcessWnds,						// ö�ٽ��̴���
	ListProcessTimers,						// ö�ٽ��̶�ʱ��
	RemoveTimer,							// �Ƴ���ʱ��
	ModifyTimeOutValue,						// �޸Ķ�ʱ����ʱʱ��
	ListHotKeys,							// ö�ٽ����ȼ�
	RemoveHotKey,							// �Ƴ�hotkey
	ListProessPrivileges,					// ö�ٽ���Ȩ��
	AdjustProcessTokenPrivileges,			// ��������Ȩ��
	EnumDrivers,							// ö������
	EnumerateKey,
	EnumerateKeyValues,
	DumpDriverMemory,
	UnloadDriver,
	GetAllCallbackNotify,
	RemoveCallbackNotify,
	EnumDpcTimer,
	RemoveDpcTimer,
	EnumIoTimer,
	StartOrStopIoTimer,
	RemoveIoTimer,
	EnumWorkerThread,
	EnumDebugRegisters,
	GetDirectIoProcess,
	DisableDirectIo,
	EnumFilterDriver,
	EnumSsdtHookInfo,
	GetSdtInlineHookAddress,
	EnumShadowSsdtHookInfo,
	RestoreSsdtOrShadowHook,
	GetFsdDispatch,
	GetInlineAddress,
	RestoreDispatchHooks,
	GetKbdClassDispatch,
	GetMouClassDispatch,
	GetClasspnpDispatch,
	GetAtapiDispatch,
	GetAcpiDispatch,
	GetTcpipDispatch,
	EnumIdtHook,
	EnumObjectHook,
	EnumAllObjectTypes,
	GetSelfCreateObjectHookInfo,
	EnumKernelEntryHook,
	EnumIatEatHook,
	EnumExportFunctionsPatchs,
	EnumSsdtInlineHookInfo,
	EnumShadowSsdtInlineHookInfo,
	EnumModulePatchs,
	EnumMessageHook,
	RestoreObjectHook,
	RestoreIatEatHook,
	RestoreModifiedCode,
	RegOpenKey,
	RegEnumerateValueKey,
	RegEnumerateKey,
	RegDeleteKey,
	RegCreateKey,
	RegSetValueKey,
	RegDeleteValueKey,
	RegRenameKey,
	KernelDeleteFile,
	Disassmbly,
	RecoverRegisters,
	KernelOpenProcess,
	KernelReadProcessMemory,
	KernelWriteProcessMemory,
	KernelSuspendProcess,
	KernelResumeProcess,
	KernelVirtualProtectEx,
	EnumPort,
	NULL,
};

pfnFunction GetFunction(OPERATE_TYPE type)
{
	if (type > enumUnKnow && type < enumFunctionMax)
	{
		return FunctionArray[type];
	}

	return NULL;
}

NTSTATUS ComVersion(PVOID pInBuffer, ULONG uInSize, PVOID pOutBuffer, ULONG uOutSize, ULONG* dwRet)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	WCHAR g_szPeVersion[] = {'2','.','0','\0'};
	PVERSION_INFO pVersion = (PVERSION_INFO)pInBuffer;

	KdPrint(("pVersion->szVersion: %S\n", pVersion->szVersion));

	if (pVersion->szVersion && 
		wcslen(pVersion->szVersion) == wcslen(g_szPeVersion) &&
		!_wcsnicmp(pVersion->szVersion, g_szPeVersion, wcslen(g_szPeVersion))
		)
	{
		status = STATUS_SUCCESS;
	}

	return status;
}

NTSTATUS CommunicatNeitherControl(PVOID pInBuffer, ULONG uInSize, PVOID pOutBuffer, ULONG uOutSize, ULONG* dwRet)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	pfnProbeForRead MzfProbeForRead = (pfnProbeForRead)GetGlobalVeriable(enumProbeForRead);
	pfnProbeForWrite MzfProbeForWrite = (pfnProbeForWrite)GetGlobalVeriable(enumProbeForWrite);
	OPERATE_TYPE type = enumUnKnow;
	pfnFunction pFunc = NULL;

	if (!MzfProbeForRead || !MzfProbeForWrite)
	{
		return status;
	}

	__try
	{
		if (uInSize < sizeof(ULONG))
		{
			return STATUS_INVALID_PARAMETER;
		}
		
		MzfProbeForRead(pInBuffer, uInSize, 1);

		if (uOutSize > 0)
		{
			MzfProbeForWrite(pOutBuffer, uOutSize, 1);
		}
	}
	__except(1)
	{
		return STATUS_UNSUCCESSFUL;
	}

	type = *(OPERATE_TYPE *)pInBuffer;
	pFunc = GetFunction(type);
	if (!pFunc)
	{
		return status;
	}

	status = pFunc(pInBuffer, uInSize, pOutBuffer, uOutSize, dwRet);

	return status;
}