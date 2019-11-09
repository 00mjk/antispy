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
#include "ParseNTFS.h"
#include "..\\..\\Common\Common.h"
#include "ConnectDriver.h"
#include "CommonMacro.h"
#include "ntfs\ParaseDisk.h"

BOOL bInit = FALSE;

BOOL
read_sector ( 
	OUT PVOID buffer,		// �����buffer
	IN ULONG nSectorNum,	// Ҫ��ȡ��������
	IN ULONG nSectorCounts	// Ҫȥ����������
	) 
{
	READ_SECTORS_CONTEXT ctx;
	CConnectDriver Driver;

	if ( buffer == NULL || !nSectorCounts ) 
	{
		return FALSE ;
	}
	
	ctx.OpType = enumReadSector;
	ctx.nSectorCounts = nSectorCounts;
	ctx.nSectorNum = nSectorNum;

	return Driver.CommunicateDriver(&ctx, sizeof(READ_SECTORS_CONTEXT), buffer, nSectorCounts * 512, NULL);
}

BOOL
write_sector ( 
	IN PWrite_Sectors_Context pContext
	)
{
// 	int nReturn = -1 ;
// 
// 	if ( NULL == pContext || NULL == pContext->pBuffer ) {
// 		return FALSE ;
// 	}
// 
// 	if ( NULL == g_drv ) {
// 		return FALSE ;
// 	}
// 
// 	nReturn = g_drv->IoControl(
// 		IOCTL_WRITE_SECTORS,
// 		(PVOID)pContext->pBuffer,	// InBuffer
// 		pContext->uSectorSize,		// InBuffer Length
// 		NULL,						// OutBuffer
// 		pContext->uSectorSize		// OutBuffer Length
// 		);
// 	
// 	if ( -1 == nReturn ) 
// 	{
// 		return FALSE ;
// 	}
// 	
	return TRUE ;
}


#define PARTITION_TYPE_NTFS         0x07
#define PARTITION_TYPE_FAT32         0x0B
#define PARTITION_TYPE_FAT32_LBA     0x0C

int g_nNumber = 0 ;
ULONG g_sectors_per_cluster	= 0; 
PLARGE_INTEGER g_partitionA_sector_pos[10] = { NULL } ;

static ULONG g_nStartSector = 0;

VOID
parase_partion_depth (
					  PMBR_SECTOR mbrsec,
					  ULONG currentSectors,
					  PVOID pTmp
					  )
{
	int MainPartNumber = 0;
	ULONG startlba = 0;
	UCHAR type = 0;
	PPARTITION_ENTRY partition0 = NULL ;
	PBBR_SECTOR bootsec			= NULL ;
	PLARGE_INTEGER result		= NULL ;
	PVOID buffer				= pTmp ;

	if (!mbrsec || !pTmp)
	{
		return;
	}

	for ( MainPartNumber = 0; MainPartNumber < 4; MainPartNumber++ )
	{
		partition0	= &mbrsec->Partition[MainPartNumber]	;
		startlba	= partition0->StartLBA					;
		type		= partition0->PartitionType				;

		// �����ܹ���4��������, ��Ϊ0��ʾ��������������,����֮.
		if ( 0 == startlba || ( 0 != partition0->active && 0x80 != partition0->active ) ) {
			break ;
		}

		startlba += currentSectors ;

		//
		// У�鵱ǰ����������:
		//   07HΪNTFS����
		//   0BHΪFAT32����
		//   05HΪEXTEND��չ����(�÷�������ʼ����Ϊ "ǰ��ĵ�һ��EXTENDX������ʼ����" + "�÷��������ƫ��")
		//   0FHΪEXTENDX��չ����(�÷�������ʼ����Ϊ "�÷��������ƫ��" ���� "ǰ��ĵ�һ��EXTENDX������ʼ����" + "�÷��������ƫ��")
		//

		memset( buffer, 0, 512 );

		if ( 0x05 == type ) // ��EXTEND��չ����
		{
			startlba = /*currentSectors + */mbrsec->Partition[1].StartLBA + g_nStartSector
				/*+ mbrsec->Partition[1].TotalSector */;

			read_sector( buffer, startlba, 1 );
			memcpy( (PVOID)mbrsec, buffer, 512 );
			parase_partion_depth( mbrsec, startlba, buffer );

			continue ;
		} 

		else if ( PARTITION_TYPE_NTFS == type ) // ��NTFS���� ------- OK -------
		{
			read_sector( buffer, startlba, 1 );

			// �����ڴ�
			result = (PLARGE_INTEGER)malloc( sizeof(LARGE_INTEGER) );
			result->QuadPart = startlba ;

			// ��ȡ��������BPB�ṹ,��ñ�������������
			bootsec = (PBBR_SECTOR) buffer ; 
			g_sectors_per_cluster = bootsec->SectorsPerCluster ;

			result->QuadPart += bootsec->ReservedSectors ;
		}

		else if ( 0x0F == type ) // ��EXTENDX��չ����
		{
			g_nStartSector = startlba;

			read_sector( buffer, startlba, 1 );
			memcpy( (PVOID)mbrsec, buffer, 512 );
			parase_partion_depth( mbrsec, startlba, buffer );
			
			continue ;
		}

		// ����fat32��ʽ,��Ҫ����Դ�
		else if( type == PARTITION_TYPE_FAT32 || type == PARTITION_TYPE_FAT32_LBA ) 
		{
			read_sector( buffer, startlba, 1 );
			result->QuadPart += bootsec->NumberOfFATs * bootsec->SectorsPerFAT32 ;
		} 
		else 
		{
			break ;
		}

		// ��������������������ȫ�ֱ���
		g_partitionA_sector_pos[g_nNumber] = result ;
		g_nNumber++ ;

		DebugLog( L"����%d�ڴ����ϵ���ʼ����: 0x%08lx \n", g_nNumber, result->QuadPart );

		// Խ����. �˳�
		if ( g_nNumber > 10 ) 
		{ 
			break ;
		}
	}
}

// �õ��������з���������λ��(�����������̵�������)  
VOID 
get_all_partition_sector () 
{
	PVOID	buffer = NULL, pTmp = NULL ;
	PMBR_SECTOR		mbrsec	= NULL ;

	pTmp = malloc( 0x1000 );
	if (!pTmp)
	{
		return;
	}
	memset( pTmp, 0, 0x1000 );

	buffer = malloc( 512 );
	if (!buffer)
	{
		return;
	}
	memset( buffer, 0, 512 );

	if (read_sector( buffer, 0, 1 ))
	{
		mbrsec = (PMBR_SECTOR) buffer ;
		parase_partion_depth( mbrsec, 0, pTmp );
	}

	free( buffer );
	free( pTmp );

	return ;
}

BOOL
read_sector_ex ( 
				OUT PVOID buffer,		// �����buffer
				IN ULONG nOffset,	// ���ļ�����ڸ÷�����������
				IN ULONG nSectorCounts,	// Ҫȥ����������
				IN CHAR nDevice
			 ) 
{
	LARGE_INTEGER realdiskpos;
	READ_SECTORS_CONTEXT ctx;
	CConnectDriver Driver;

	if ( buffer == NULL || !nSectorCounts ) 
	{
		return FALSE ;
	}

	if (!bInit)
	{
		get_all_partition_sector ();
		bInit = TRUE;
	}

	int driveId = toupper(nDevice) - 'C';
	realdiskpos.QuadPart = g_partitionA_sector_pos[driveId]->QuadPart + nOffset ;

	ctx.OpType = enumReadSector;
	ctx.nSectorCounts = nSectorCounts;
	ctx.nSectorNum = realdiskpos.LowPart;

	return Driver.CommunicateDriver(&ctx, sizeof(READ_SECTORS_CONTEXT), buffer, nSectorCounts * 512, NULL);
}