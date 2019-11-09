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
#ifndef _INIT_WINDOWS_H_
#define _INIT_WINDOWS_H_

#include <ntifs.h>
#include <windef.h>

/////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////

typedef enum _VERIABLE_INDEX_
{	
	enumRandomKey,
	enumSameThreadApcFlags_Offset_ETHREAD,
	enumPsGetProcessCreateTimeQuadPart,
	enumAtapiParseDiskDriverObject,
	enumReloadAtapiStartIoDispatch,
	enumNtReadFileIndex,
	enumPsLookupThreadByThreadId,
	enumOriginAtapiStartIoDispatch,
	enumIoBuildDeviceIoControlRequest,
	enumBackupDriverPath,
	enumAntiSpyPid,
	enumAntiSpyEprocess,
	enumRtlInitAnsiString,
	enumRtlAnsiStringToUnicodeString,
	enumRtlFreeUnicodeString,
	enumCsrssEprocess,
	enumPsGetCurrentProcessId,
	enumIoFreeIrp,
	enumMmFlushImageSection,
	enumFastfatBase,
	enumFastfatSize,
	enumNtfsBase,
	enumKeInitializeApc,
	enumKeInsertQueueApc,
	enumActiveExWorkerOffset_ETHREAD,
	enumNtfsSize,
	enumExfatBase,
	enumNtCreateFile,
	enumNtWriteFile,
	enumNtReadFile,
	enumExfatSize,
	enumPsThreadType,
	enumPsProcessType,
	enumThreadListHeadOffset_KPROCESS,
	enumCidOffset_ETHREAD,
	enumStartAddressOffset_ETHREAD,
	enumKdVersionBlock,
	enumVadRootOffset_EPROCESS,
	enumPreviousModeOffset_KTHREAD,
	enumStateOffset_KTHREAD,
	enumWindowsVersion,	
	enumPriorityOffset_KTHREAD,
	enumContextSwitchesOffset_KTHREAD,
	enumTebOffset_KTHREAD,
	enumThreadListEntryOffset_KTHREAD,
	enumThreadListEntryOffset_ETHREAD,
	enumThreadsProcessOffset_ETHREAD,
	enumWin32StartAddressOffset_ETHREAD,
	enumBuildNumber,
	enumMmPfnDatabase,
	enumIdleThreadOffset_KPRCB,
	enumPspCidTable,
	enumNtoskrnlPath,
	enumWaitListHeadOffset_KPRCB,
	enumDispatcherReadyListHeadOffset_KPRCB,
	enumWaitInListHead,
	enumThreadListHeadOffset_EPROCESS,
	enumProcessOffset_KAPC_STATE,
	enumObjectHeaderSize,
	enumObjectTypeOffset_OBJECT_HEADER,
	enumSectionObjectOffset_EPROCESS,
	enumDispatcherReadyListHead,
	enumSpyHunterDriverObject,
	enumObjectTableOffset_EPROCESS,
	enumUniqueProcessIdOffset_HANDLE_TABLE,
	enumUniqueProcessIdOffset_EPROCESS,
	enumPebOffset_EPROCESS,
	enumSystemProcess,
	enumInheritedFromUniqueProcessIdOffset_EPROCESS,
	enumWaitListEntryOffset_KTHREAD,
	enumApcStateOffset_KTHREAD,
	enumOriginKernelBase,
	enumKernelModuleSize,
	enumNewKernelBase,					// ���ں˵Ļ���ַ��ж��ʱ��Ҫ�ͷ�
	enumNewKernelBaseWithoutFix,		// ���ں˵Ļ���ַ, û���޸�����ж��ʱ��Ҫ�ͷ�
	enumOriginKiServiceTable,			// ϵͳԭʼ��SSDT->ServiceTable��Ҳ�ǱȽϸɾ��ģ������ܻᱻinline hook��ж��ʱ��Ҫ�ͷ�
	enumOriginSSDT,						// ϵͳԭʼ��SSDT��, ж��ʱ��Ҫ�ͷ�
	enumReloadKiServiceTable,			// Reload Ntos֮��,�ض�λ������SSDT->ServiceTable,Ϊ�˷�ֹhook,һ�������ĺ���,ж��ʱ��Ҫ�ͷ�
	enumReloadSSDT,						// Reload Ntos֮��,�ض�λ������SSDT��,ж��ʱ��Ҫ�ͷ�
	enumReloadNtosKrnlSuccess,			// ����Reload Ntos�Ƿ�ɹ�
	enumNowSSDT,						// ϵͳ�е�SSDT��

	enumSsdtFunctionsCnt,
	enumShadowSsdtFunctionsCnt,
	enumSsdtFuncionsNameBuffer,			// ����ssdt��������һ�β��ɷ�ҳ�ڴ棬ж��ʱ��Ҫ�ͷš�

	enumOriginWin32kBase,				// ԭʼ��Win32k.sys����ַ
	enumNewWin32kBase,					// Reload��Win32k.sys����ַ
	enumWin32kSize,						// win32k.sys�Ĵ�С
	enumReloadWin32kSuccess,			// Reload Win32k.sys�Ƿ�ɹ�
	enumNowShadowSSDT,					// ��ǰϵͳ�е�Shadow SSDT��
	enumOriginShadowKiServiceTable,		// ϵͳԭʼ�ģ�δ��SSDT hook��Shadow SSDT->ServiceTable��ж��ʱ��Ҫ�ͷ�
	enumOriginShadowSSDT,				// ϵͳԭʼ�ģ�δ��SSDT hook��Shadow SSDT��ж��ʱ��Ҫ�ͷ�
	enumReloadShadowKiServiceTable,		// Reload��Shadow SSDT->ServiceTable��ж��ʱ��Ҫ�ͷ�
	enumReloadShadowSSDT,				// Reload��Shadow SSDT��ж��ʱ��Ҫ�ͷ�
	
	enumMmIsAddressValid,
	enumMmGetVirtualForPhysical,
	enumMmGetPhysicalAddress,
	enumObQueryNameString,
	enumObfDereferenceObject,
	enumIoVolumeDeviceToDosName,
	enumIoQueryFileDosDeviceName,
	enumObOpenObjectByPointer,
	enumObReferenceObjectByPointer,
	enumPsGetThreadId,
	enumPsLookupProcessByProcessId,
	enumIoGetCurrentProcess,
	enumIoThreadToProcess,
	enumExAllocatePoolWithTag,
	enumExFreePoolWithTag,
	enumKeSetEvent,
	enumKeQueryActiveProcessors,
	enumKeInitializeEvent,
	enumKeInitializeDpc,
	enumKeSetTargetProcessorDpc,
	enumKeSetImportanceDpc,
	enumKeInsertQueueDpc,
	enumKeWaitForSingleObject,
	enumKeStackAttachProcess,
	enumKeUnstackDetachProcess,
	enumProbeForRead,
	enumObReferenceObjectByHandle,
	enumObGetObjectType,
	enumPsGetCurrentThread,
	enumPsGetProcessId,
	enumNtQuerySystemInformation,
	enumNtQuerySystemInformationIndex,
	enumNtOpenProcess,
	enumNtQueryVirtualMemory,
	enumNtQueryVirtualMemoryIndex,
	enumNtClose,
	enumNtCloseIndex,
	enumNtOpenDirectoryObject,
	enumNtOpenDirectoryObjectIndex,
	enumNtQueryObject,
	enumNtQueryObjectIndex,
	enumMmIsNonPagedSystemAddressValid,
	enumIoAllocateMdl,
	enumMmBuildMdlForNonPagedPool,
	enumMmMapLockedPagesSpecifyCache,
	enumMmUnmapLockedPages,
	enumIoFreeMdl,
	enumProbeForWrite,
	
// { shadow hook
	enumNtUserGetForegroundWindowIndex,  // ������������˳������
	enumOriginNtUserGetForegroundWindow,
	enumFakeNtUserGetForegroundWindow,

	enumNtUserBuildHwndListIndex,
	enumOriginNtUserBuildHwndList,
	enumFakeNtUserBuildHwndList,

	enumNtUserDestroyWindowIndex,
	enumOriginNtUserDestroyWindow,
	enumFakeNtUserDestroyWindow,

	enumNtUserFindWindowExIndex,
	enumOriginNtUserFindWindowEx,
	enumFakeNtUserFindWindowEx,

	enumNtUserMessageCallIndex,
	enumOriginNtUserMessageCall,
	enumFakeNtUserMessageCall,

	enumNtUserPostThreadMessageIndex,
	enumOriginNtUserPostThreadMessage,
	enumFakeNtUserPostThreadMessage,
	
	enumNtUserQueryWindowIndex,
	enumOriginNtUserQueryWindow,
	enumFakeNtUserQueryWindow,
	
	enumNtUserSetParentIndex,
	enumOriginNtUserSetParent,
	enumFakeNtUserSetParent,

	enumNtUserSetWindowLongIndex,
	enumOriginNtUserSetWindowLong,
	enumFakeNtUserSetWindowLong,

	enumNtUserShowWindowIndex,
	enumOriginNtUserShowWindow,
	enumFakeNtUserShowWindow,

	enumNtUserWindowFromPointIndex,
	enumOriginNtUserWindowFromPoint,
	enumFakeNtUserWindowFromPoint,

	enumNtUserAttachThreadInputIndex,
	enumOriginNtUserAttachThreadInput,
	enumFakeNtUserAttachThreadInput,

	enumNtUserPostMessageIndex,
	enumOriginNtUserPostMessage,
	enumFakeNtUserPostMessage,

// }

// { ��ֹ��Ļ��ͼ
	enumNtGdiBitBltIndex,
	enumOriginNtGdiBitBlt,
	enumFakeNtGdiBitBlt,

	enumNtGdiStretchBltIndex,
	enumOriginNtGdiStretchBlt,
	enumFakeNtGdiStretchBlt,

	enumNtGdiMaskBltIndex,
	enumOriginNtGdiMaskBlt,
	enumFakeNtGdiMaskBlt,

	enumNtGdiPlgBltIndex,
	enumOriginNtGdiPlgBlt,
	enumFakeNtGdiPlgBlt,

	enumNtGdiAlphaBlendIndex,
	enumOriginNtGdiAlphaBlend,
	enumFakeNtGdiAlphaBlend,

	enumNtGdiTransparentBltIndex,
	enumOriginNtGdiTransparentBlt,
	enumFakeNtGdiTransparentBlt,

	enumNtUserGetClipboardDataIndex,
	enumOriginNtUserGetClipboardData,
	enumFakeNtUserGetClipboardData,

	enumNtGdiGetDCPointIndex,
	enumNtGdiGetDCPoint,

	enumNtGdiGetDCDwordIndex,
	enumNtGdiGetDCDword,
	
	enumNtUserWindowFromPoint,
	enumVMemCache,
// }

	enumNtUserBuildHwndList,
	enumNtUserQueryWindow,
	enumNtUserKillTimer,
	enumNtUserKillTimerIndex,
	enumNtUserValidateTimerCallbackIndex,
	enumNtUserValidateTimerCallback,
	enumNtUserRegisterHotKey,
	enumNtUserRegisterHotKeyIndex,

	enumInterlockedIncrement,
	enumInterlockedDecrement,
	enumAddend,				// ����Ǳ��ݼӺ͵ݼ��ļ���ֵ
	enumFakeWnd,

	enumNtOpenProcessToken,
	enumNtOpenProcessTokenIndex,
	enumNtQueryInformationToken,
	enumNtQueryInformationTokenIndex,
	enumNtUnmapViewOfSection,
	enumNtUnmapViewOfSectionIndex,
	enumNtTerminateThread,

	enumgptmrFirst,                // win32k��һ��ȫ�ֱ���������ö�ٽ��̵Ķ�ʱ��
	enumgphkFirst,				   // win32k��һ��ȫ�ֱ���������ö��ϵͳ�ȼ�
	enumInitialStackOffset_KTHREAD,
	enumStackLimitOffset_KTHREAD,
	enumKernelStackOffset_KTHREAD,
	enumKernelApcDisableOffset_KTHREAD,
	enumCrossThreadFlagsOffset_ETHREAD,
	enumNtSuspendThread,
	enumNtSuspendThreadIndex,
	enumNtResumeThread,
	enumNtResumeThreadIndex,
	enumSuspendCountOffset_KTHREAD,
	enumFreezeCountOffset_KTHREAD,
	enumNtSetInformationObject,
	enumNtSetInformationObjectIndex,
	enumNtFreeVirtualMemoryIndex,
	enumNtFreeVirtualMemory,
	enumNtProtectVirtualMemory,
	enumFileTypeIndex,
	enumNtProtectVirtualMemoryIndex,
	enumNtAdjustPrivilegesToken,
	enumNtAdjustPrivilegesTokenIndex,
	enumNtTerminateProcess,
// 	enumAntiSpyEprocess,

// { ssdt hook
	enumNtOpenProcessIndex,
	enumOriginNtOpenProcess,
	enumFakeNtOpenProcess,

	enumNtOpenThreadIndex,
	enumOriginNtOpenThread,
	enumFakeNtOpenThread,

	enumNtTerminateProcessIndex,
	enumOriginNtTerminateProcess,
	enumFakeNtTerminateProcess,

	enumNtTerminateThreadIndex,
	enumOriginNtTerminateThread,
	enumFakeNtTerminateThread,

	enumNtDuplicateObjectIndex,
	enumOriginNtDuplicateObject,
	enumFakeNtDuplicateObject,
// }

// { forbid functions

	enumNtCreateProcessIndex,
	enumOriginNtCreateProcess,
	enumFakeNtCreateProcess,

	enumNtCreateProcessExIndex,
	enumOriginNtCreateProcessEx,
	enumFakeNtCreateProcessEx,

	enumNtCreateThreadIndex,
	enumOriginNtCreateThread,
	enumFakeNtCreateThread,

	enumNtCreateFileIndex,
	enumOriginNtCreateFile,
	enumFakeNtCreateFile,

	enumNtUserSwitchDesktopIndex,
	enumOriginNtUserSwitchDesktop,
	enumFakeNtUserSwitchDesktop,

	enumNtCreateKeyIndex,
	enumOriginNtCreateKey,
	enumFakeNtCreateKey,

	enumNtLoadKey2Index,
	enumOriginNtLoadKey2,
	enumFakeNtLoadKey2,

	enumNtReplaceKeyIndex,
	enumOriginNtReplaceKey,
	enumFakeNtReplaceKey,

	enumNtRestoreKeyIndex,
	enumOriginNtRestoreKey,
	enumFakeNtRestoreKey,
	
	enumNtSetValueKeyIndex,
	enumOriginNtSetValueKey,
	enumFakeNtSetValueKey,

	enumCanSetValues,			// �Ƿ���Ը���ֵ������
	enumIsBackupDriver,			// �Ƿ񱸷������ļ�
	enumBackupDriverEvent,		// ����ͬ�������������¼�

	enumNtCreateSectionIndex,
	enumOriginNtCreateSection,
	enumFakeNtCreateSection,

	enumNtInitiatePowerActionIndex,
	enumOriginNtInitiatePowerAction,
	enumFakeNtInitiatePowerAction,

	enumNtSetSystemPowerStateIndex,
	enumOriginNtSetSystemPowerState,
	enumFakeNtSetSystemPowerState,

	enumNtUserCallOneParamIndex,
	enumOriginNtUserCallOneParam,
	enumFakeNtUserCallOneParam,

	enumNtShutdownSystemIndex,
	enumOriginNtShutdownSystem,
	enumFakeNtShutdownSystem,

	enumNtSetSystemTimeIndex,
	enumOriginNtSetSystemTime,
	enumFakeNtSetSystemTime,

	enumNtUserLockWorkStationIndex,
	enumOriginNtUserLockWorkStation,
	enumFakeNtUserLockWorkStation,

	enumNtLoadDriverIndex,
	enumOriginNtLoadDriver,
	enumFakeNtLoadDriver,

	enumRtlImageDirectoryEntryToData,

	enumKeUserModeCallback,
	enumKeUserModeCallbackIatAddress,

	enumNtCreateUserProcessIndex,
	enumOriginNtCreateUserProcess,
	enumFakeNtCreateUserProcess,

	enumNtCreateThreadExIndex,
	enumOriginNtCreateThreadEx,
	enumFakeNtCreateThreadEx,
// }

	enumNtShutdownSystem,
	enumNtSuspendProcess,
	enumNtSuspendProcessIndex,
	enumNtResumeProcess,
	enumNtResumeProcessIndex,
	enumPspTerminateThreadByPointer,
	enumTypeListOffset_OBJECT_TYPE,
	enumIoDeviceObjectType,
	enumIoDriverObjectType,
	enumDirectoryObjectType,
	enumNtoskrnl_KLDR_DATA_TABLE_ENTRY,
	enumNtOpenSymbolicLinkObject,
	enumNtOpenSymbolicLinkObjectIndex,
	enumNtQuerySymbolicLinkObject,
	enumNtQuerySymbolicLinkObjectIndex,
	enumIoCreateFile,
	enumIoFileObjectType,
	enumObMakeTemporaryObject,
	enumPsTerminateSystemThread,
	enumPsCreateSystemThread,
	enumIoDeleteDevice,
	enumSymbolicLinkObjectType,
	enumObOpenObjectByName,
	enumNtOpenKeyIndex,
	enumNtOpenKey,
	enumNtEnumerateKeyIndex,
	enumNtEnumerateKey,
	enumNtEnumerateValueKey,
	enumNtEnumerateValueKeyIndex,
	enumPsSetCreateProcessNotifyRoutine,
	enumPspCreateProcessNotifyRoutine,
	enumPspLoadImageNotifyRoutine,
	enumPsSetLoadImageNotifyRoutine,
	enumPsSetCreateThreadNotifyRoutine,
	enumPspCreateThreadNotifyRoutine,
	enumPsRemoveCreateThreadNotifyRoutine,
	enumPsRemoveLoadImageNotifyRoutine,
	enumIoRegisterShutdownNotification,
	enumIopNotifyShutdownQueueHead,
	enumCmUnRegisterCallback,
	enumCmpCallBackVector,
	enumIoUnregisterShutdownNotification,
	enumKeUpdateSystemTime,
	enumKiTimerTableListHead,
	enumExTimerObjectType,
	enumTIMER_TABLE_SIZE,
	enumKeCancelTimer,
	enumIoInitializeTimer,
	enumIoStartTimer,
	enumIoStopTimer,
	enumIopTimerQueueHead,
	enumExWorkerQueue,
	enumExQueueWorkItem,
	enumQueueListEntryOffset_KTHREAD,
	enumIoplOffset_KPROCESS,
	enumIopmOffsetOffset_KPROCESS,
	enumKe386IoSetAccessProcess,
	enumIoGetDeviceObjectPointer,
	enumObReferenceObjectByName,
	enumPsInitialSystemProcessAddress,
	enumNtOpenFile,
	enumNtCreateSection,
	enumNtMapViewOfSection,
	enumNtOpenFileIndex,
//	enumNtCreateSectionIndex,
	enumNtMapViewOfSectionIndex,
	
	enumNtfsImpMmFlushImageSection,			// NtfsImpMmFlushImageSection���������ַ
	enumFastfatImpMmFlushImageSection,
	enumOriginNtfsImpMmFlushImageSection,	// ϵͳ��ԭ����NtfsImpMmFlushImageSection������ַ�����ܱ�hook�ĵ�ַ
	enumOriginFastfatImpMmFlushImageSection,

	enumpIofCompleteRequest,
	enumIopfCompleteRequest,
	enumOriginIopfCompleteRequest,			// ϵͳ��ԭ����IopfCompleteRequest������ַ�����ܱ�hook�ĵ�ַ

	enumIofCompleteRequest,
	enumNtfsImpIofCompleteRequest,			// NtfsImpIofCompleteRequest���������ַ
	enumFastfatImpIofCompleteRequest,
	enumOriginNtfsImpIofCompleteRequest,	// ϵͳ��ԭ����NtfsImpIofCompleteRequest������ַ�����ܱ�hook�ĵ�ַ
	enumOriginFastfatImpIofCompleteRequest,

	enumNtfsDriverObject,
	enumFastfatDriverObject,
	enumNtfsOriginalDispatchs,	// Ntfsԭʼ��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	enumNtfsReloadDispatchs,	// Ntfs Reload��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	enumIopInvalidDeviceRequest,
	enumIoCreateDriver,
	enumReloadNtfsBase,			// ж��ʱ��Ҫ�ͷ�

	enumFastfatOriginalDispatchs,	// Fastfatԭʼ��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	enumFastfatReloadDispatchs,		// Fastfat Reload��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	enumReloadFastfatBase,			// ж��ʱ��Ҫ�ͷ�

	enumKbdclassBase,
	enumKbdclassSize,
	enumKbdClassDriverObject,
	enumReloadKbdClassBase,			// ж��ʱ��Ҫ�ͷ�
	enumKbdClassOriginalDispatchs,	// KbdClassԭʼ��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	enumKbdClassReloadDispatchs,	// KbdClass Reload��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	
	enumMouclassBase,
	enumMouclassSize,
	enumMouClassDriverObject,
	enumReloadMouClassBase,			// ж��ʱ��Ҫ�ͷ�
	enumMouClassOriginalDispatchs,	// MouClassԭʼ��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	enumMouClassReloadDispatchs,	// MouClass Reload��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�

	enumClasspnpBase,
	enumClasspnpSize,
	enumClasspnpDriverObject,
	enumReloadClasspnpBase,			// ж��ʱ��Ҫ�ͷ�
	enumClasspnpOriginalDispatchs,	// Classpnpԭʼ��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	enumClasspnpReloadDispatchs,	// Classpnp Reload��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�

	enumAtaPortSize,
	enumAtaPortBase,

	enumAtapiSize,
	enumAtapiBase,
	enumAtapiDriverObject,
	enumReloadAtapiBase,		// ж��ʱ��Ҫ�ͷ�
	enumAtapiOriginalDispatchs,	// Atapiԭʼ��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	enumAtapiReloadDispatchs,   // Atapi Reload��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�

	enumAcpiSize,
	enumAcpiBase,
	enumAcpiDriverObject,
	enumReloadAcpiBase,			// ж��ʱ��Ҫ�ͷ�
	enumAcpiOriginalDispatchs,	// Acpiԭʼ��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	enumAcpiReloadDispatchs,	// Acpi Reload��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�

	enumTcpipBase,
	enumTcpipSize,
	enumTcpipDriverObject,
	enumReloadTcpipBase,			// ж��ʱ��Ҫ�ͷ�
	enumTcpipOriginalDispatchs,		// Tcpipԭʼ��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�
	enumTcpipReloadDispatchs,		// Tcpip Reload��Dispatchs���̣�ж��ʱ��Ҫ�ͷš�

	enumOriginIdtTable,		// ԭʼ��Idt������ַ��ж��ʱ��Ҫ�ͷ�
	enumReloadIdtTable,		// Reload��Idt������ַ��ж��ʱ��Ҫ�ͷ�

	enumDirectoryType,		 // ϵͳ��ȫ��������
	enumMutantType,
	enumThreadType,
	enumFilterCommunicationPortType,
	enumControllerType,
	enumProfileType,
	enumEventType,
	enumTypeType,
	enumSectionType,
	enumEventPairType,
	enumSymbolicLinkType,
	enumDesktopType,
	enumTimerType,
	enumFileType,
	enumWindowStationType,
	enumDriverType,
	enumWmiGuidType,
	enumKeyedEventType,
	enumDeviceType,
	enumTokenType,
	enumDebugObjectType,
	enumIoCompletionType,
	enumProcessType,
	enumAdapterType,
	enumKeyType,
	enumJobType,
	enumWaitablePortType,
	enumPortType,
	enumCallbackType,
	enumFilterConnectionPortType,
	enumSemaphoreType,

	enumObGetObjectSecurity,
	enumObCreateObjectType,
	enumSeDefaultObjectMethod,

	enumObpDeleteObjectType, // ObpTypeObjectType

	enumObpDeleteSymbolicLink, // ObpSymbolicLinkObjectType
	enumObpParseSymbolicLink,

	enumIopParseDevice, // IoDeviceObjectType
	enumIopDeleteDevice,
	enumIopGetSetSecurityObject,

	enumIopDeleteDriver, // IoDriverObjectType

	enumIopDeleteIoCompletion, // IoCompletionObjectType
	enumIopCloseIoCompletion,

	enumIopCloseFile,   // IoFileObjectType
	enumIopDeleteFile,
	enumIopParseFile,
	enumIopFileGetSetSecurityObject,
	enumIopQueryName,

	enumLpcpClosePort, // LpcWaitablePortObjectType || LpcPortObjectType
	enumLpcpDeletePort,
	enumEtwpOpenRegistrationObject,

	enumMiSectionDelete, // MmSectionObjectType
	
	enumPspProcessDelete, // PsProcessType
	enumPspProcessOpen,
	enumPspProcessClose,

	enumPspThreadDelete, // PsThreadType
	enumPspThreadOpen,

	enumPspJobDelete, // PsJobType
	enumPspJobClose,

	enumSepTokenDeleteMethod, // SeTokenObjectType

	enumWmipSecurityMethod, // WmipGuidObjectType
	enumWmipDeleteMethod,
	enumWmipCloseMethod,

	enumExpWin32CloseProcedure, // ExWindowStationObjectType || ExDesktopObjectType
	enumExpWin32DeleteProcedure,
	enumExpWin32OkayToCloseProcedure,
	enumExpWin32ParseProcedure, // ExDesktopObjectType �޴˺���
	enumExpWin32OpenProcedure,
	
	enumTimerDeleteProcedure, // ExTimerObjectType

	enumExpDeleteMutant, // ExMutantObjectType

	enumExpProfileDelete, // ExProfileObjectType

	enumCmpCloseKeyObject, // CmpKeyObjectType
	enumCmpDeleteKeyObject,
	enumCmpParseKey,
	enumCmpSecurityMethod,
	enumCmpQueryKeyName,

	enumxHalLocateHiberRanges, // DbgkDebugObjectType || ExCallbackObjectType ��Delete Procedure
	enumDbgkpCloseObject,
	
	enumObpCloseDirectoryObject, // ObpDirectoryObjectType
	enumObpDeleteDirectoryObject,

	enumGetObjectTypeDispatchOk, // �Ƿ��Ѿ���ȡ��������Ϣ

	enumKiFastCallEntry,

	enumNewHalDllBaseWithoutFix,		// ��Hal.Dll�Ļ���ַ, û���޸�����ж��ʱ��Ҫ�ͷ�
	enumOriginHalDllBase,				// ԭʼ��Hal.Dll����ַ
	enumNewHalDllBase,					// Reload��Hal.Dll����ַ
	enumHalDllSize,						// Hal.Dll�Ĵ�С

	enumObCreateObjectTypeEx,

	enumSectionBaseAddressOffset_EPROCESS,

	enumNtDeleteKeyIndex,
	enumNtDeleteKey,
	enumNtCreateKey,
// 	enumNtCreateKeyIndex,
	enumNtSetValueKey,
//	enumNtSetValueKeyIndex,
	enumNtDeleteValueKey,
	enumNtDeleteValueKeyIndex,

	enumDebugportOffset_EPROCESS,

	enumNtReadVirtualMemory,
	enumNtWriteVirtualMemory,
	enumNtReadVirtualMemoryIndex,
	enumNtWriteVirtualMemoryIndex,
	enumNtRenameKeyIndex,
	enumNtRenameKey,
	
	enumNtWriteFileIndex,
}VERIABLE_INDEX;	

VOID SetGlobalVeriable(VERIABLE_INDEX Index, ULONG Value);
ULONG GetGlobalVeriable(VERIABLE_INDEX Index);
ULONG GetGlobalVeriableAddress(VERIABLE_INDEX Index);
ULONG *GetGlobalVeriable_Address(VERIABLE_INDEX Index);

/////////////////////////////////////////////////////////

BOOL InitWindows(PDRIVER_OBJECT DriverObject);
VOID UnInitProcessVariable();
VOID InitSSDTFunctions();
NTSTATUS CheckUpdate();

/////////////////////////////////////////////////////////

#endif



