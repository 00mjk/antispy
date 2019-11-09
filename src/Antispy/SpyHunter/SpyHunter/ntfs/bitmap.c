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
/**
 * bitmap.c - Bitmap handling code. Originated from the Linux-NTFS project.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <windows.h>
#include <stddef.h>


#include "types.h"
#include "attrib.h"
#include "bitmap.h"
#include "debug.h"
#include "logging.h"
#include "misc.h"


#pragma warning( disable: 4244 4761 )

//////////////////////////////////////////////////////////////////////////



/**
 * ntfs_bit_set - set a bit in a field of bits
 * @bitmap:	field of bits
 * @bit:	bit to set
 * @new_value:	value to set bit to (0 or 1)
 *
 * Set the bit @bit in the @bitmap to @new_value. Ignore all errors.
 */
void ntfs_bit_set(u8 *bitmap, const u64 bit, const u8 new_value)
{
	if (!bitmap || new_value > 1)
		return;
	if (!new_value)
		bitmap[bit >> 3] &= ~(1 << (bit & 7));
	else
		bitmap[bit >> 3] |= (1 << (bit & 7));
}

/**
 * ntfs_bit_get - get value of a bit in a field of bits
 * @bitmap:	field of bits
 * @bit:	bit to get
 *
 * Get and return the value of the bit @bit in @bitmap (0 or 1).
 * Return -1 on error.
 */
char ntfs_bit_get(const u8 *bitmap, const u64 bit)
{
	if (!bitmap)
		return -1;
	return (bitmap[bit >> 3] >> (bit & 7)) & 1;
}

/**
 * ntfs_bit_get_and_set - get value of a bit in a field of bits and set it
 * @bitmap:	field of bits
 * @bit:	bit to get/set
 * @new_value:	value to set bit to (0 or 1)
 *
 * Return the value of the bit @bit and set it to @new_value (0 or 1).
 * Return -1 on error.
 */
char ntfs_bit_get_and_set(u8 *bitmap, const u64 bit, const u8 new_value)
{
	register u8 old_bit, shift;

	if (!bitmap || new_value > 1)
		return -1;
	shift = bit & 7;
	old_bit = (bitmap[bit >> 3] >> shift) & 1;
	if (new_value != old_bit)
		bitmap[bit >> 3] ^= 1 << shift;
	return old_bit;
}



int 
ntfs_bitmap_set_bits_in_run(
	IN ntfs_attr *na, 
	IN s64 start_bit,
	IN s64 count, 
	IN int value
	)
/*++

Author: sudami [sudami@163.com]
Time  : 2009/03/13 [13:3:2009 - 10:28]

Routine Description:
  ����ָ�����Ե�bitmapλ. ����4(@value)Ϊ0��ʾ�����Խ�������Ϊδʹ��(�����Ϊɾ��);
  Ϊ1��ʾ�����Խ�������Ϊʹ����(�������ļ����õ�)
    
Arguments:
  na - �ýṹ���а���bitmap
  start_bit - bitmap��ָ��Ҫ���õ���ʼλ��
  count - Ҫ���õ�bit��
  value - 0 ��ʾ���Ϊɾ��; 1��ʾ�����Խ�������Ϊʹ����

Return Value:
  0 - �ɹ�; -1 - ʧ��
    
--*/
{
	s64 bufsize, br;
	u8 *pBufTmp_bark, *pBuf, *buf, *lastbyte_buf, bLittleOffset ;
	int bit, firstbyte, lastbyte, lastbyte_pos, tmp, err, nSectors, wsssssss;

	if (!na || start_bit < 0 || count < 0) { // �����Ϸ��Լ��
		errno = EINVAL;
		return -1;
	}

	// �鿴�˴�bit�ĵ�3λ�Ƿ����
	bit = start_bit & 7;
	if (bit)
		firstbyte = 1;
	else
		firstbyte = 0;

	// Calculate the required buffer size in bytes, capping it at 8kiB.
	bufsize = ((count - (bit ? 8 - bit : 0) + 7) >> 3) + firstbyte;
	if (bufsize > 8192) { bufsize = 8192; }

	// ��д������ֻ����512�ı���.���Է�������Ҳһ��.����Ҫ��ȡ��������
	wsssssss = ((DWORD)bufsize % 512) > 0 ? 1 : 0 ;
	nSectors =  (DWORD)bufsize / 512 + wsssssss ;

	buf = (u8 *)ntfs_malloc( nSectors * 512 );
	pBuf = buf ;
	if (!buf) { return -1; }
	
	// ��ʼ���ڴ�
	memset(buf, value ? 0xff : 0, nSectors * 512);

	// ���ԭʼ�Ķ�����������,���ᱻ�޸�
	pBufTmp_bark = (u8 *)ntfs_malloc( nSectors * 512 );
	if (!pBufTmp_bark) { return -1; }

	if (bit) 
	{
		// ֻ�� 8-bit, ��һ���ֽ�(char)������,��ReadFileֻ�ܶ�ȡ512�ı���.
		br = ntfs_attr_pread( na, start_bit >> 3, 1, buf );
		if (br != 512) {
			free(buf);
			errno = EIO;
			return -1;
		}

		// ����һ�����ݵ������ڴ���ȥ
		memcpy( (PVOID)pBufTmp_bark, (PVOID)buf, nSectors * 512 );

		//
		// ��λ��ʵ��Ҫ���ĵط�
		//
		bLittleOffset = 0 ;
		bLittleOffset = (start_bit >> 3) & 0xff ;
		buf += bLittleOffset ;

		// and set or clear the appropriate bits in it.
		while ( (bit & 7) && count-- ) 
		{
			if (value) { // ���Ϊ�ѱ�ʹ��
				*buf |= 1 << bit++;

			} else { // ���Ϊ�ѱ�ɾ��

				*buf &= ~(1 << bit++);
			}
		}

		// Update @start_bit to the new position.
		start_bit = (start_bit + 7) & ~7;
	}

	// Loop until @count reaches zero.
	lastbyte = 0;
	lastbyte_buf = NULL;
	bit = count & 7;

	do {

		if (count > 0 && bit) 
		{
			lastbyte_pos = ((count + 7) >> 3) + firstbyte;
			if (!lastbyte_pos) {
				ntfs_log_trace("Eeek! lastbyte is zero. Leaving inconsistent metadata.\n");
				err = EIO;
				goto free_err_out;
			}

			// and it is in the currently loaded bitmap window... 
			if (lastbyte_pos <= bufsize) 
			{
				lastbyte_buf = buf + lastbyte_pos - 1;
				bLittleOffset = ((start_bit + count) >> 3) & 0xff ;

				memcpy( 
					(PVOID)lastbyte_buf, 
					(PVOID)(pBufTmp_bark + bLittleOffset), 
					sizeof(u8) 
					);

// 				br = ntfs_attr_pread( na, (start_bit + count) >> 3, 1, lastbyte_buf );
// 				if (br != 512) {
// 					ntfs_log_trace("Eeek! Read of last byte failed. Leaving inconsistent metadata.\n");
// 					err = EIO;
// 					goto free_err_out;
// 				}

				while (bit && count--) 
				{
					if (value)
						*lastbyte_buf |= 1 << --bit;
					else
						*lastbyte_buf &= ~(1 << --bit);
				}

				// We don't want to come back here... 
				bit = 0;

				// We have a last byte that we have handled.
				lastbyte = 1;
			}
		}

		//
		// unix�µĺ�������ֻдһ���ֽ�.����windows�±���д512��������
		// ��ô������ʵֻҪ��дһ��bit, ������$bitmap�е�ĳ������λΪFF
		// ��֮ǰ����Buff����һ�������Ĵ�С, ����ֻ�ı������Buff�е�
		// 1-bit,���ڰ��������д��ȥ������.
		// 

		tmp = (start_bit >> 3) - firstbyte;
		br = ntfs_attr_pwrite( na, tmp, bufsize, pBuf );
		if (br != 512) {
			ntfs_log_trace("Eeek! Failed to write buffer to bitmap. Leaving inconsistent metadata.\n");
			err = EIO;
			goto free_err_out;
		}

		// Update counters.
		tmp = (bufsize - firstbyte - lastbyte) << 3;
		if (firstbyte) 
		{
			firstbyte = 0;

			// Re-set the partial first byte so a subsequent write of the buffer does not have stale, incorrect bits.
			*buf = value ? 0xff : 0;
		}

		start_bit += tmp;
		count -= tmp;
		if (bufsize > (tmp = (count + 7) >> 3))
			bufsize = tmp;

		if (lastbyte && count != 0) {
			ntfs_log_trace("Eeek! Last buffer but count is not zero (= %lli). Leaving inconsistent metadata.\n",(LONG64)count);
			err = EIO;
			goto free_err_out;
		}

	} while (count > 0);

	// OK!
	free(pBuf);
	return 0;

free_err_out:
	free(pBuf);
	errno = err;
	return -1;
}

/**
 * ntfs_bitmap_set_run - set a run of bits in a bitmap
 * @na:		attribute containing the bitmap
 * @start_bit:	first bit to set
 * @count:	number of bits to set
 *
 * Set @count bits starting at bit @start_bit in the bitmap described by the
 * attribute @na.
 *
 * On success return 0 and on error return -1 with errno set to the error code.
 */
int ntfs_bitmap_set_run(ntfs_attr *na, s64 start_bit, s64 count)
{
	return ntfs_bitmap_set_bits_in_run(na, start_bit, count, 1);
}

/**
 * ntfs_bitmap_clear_run - clear a run of bits in a bitmap
 * @na:		attribute containing the bitmap
 * @start_bit:	first bit to clear
 * @count:	number of bits to clear
 *
 * Clear @count bits starting at bit @start_bit in the bitmap described by the
 * attribute @na.
 *
 * On success return 0 and on error return -1 with errno set to the error code.
 */
int ntfs_bitmap_clear_run(ntfs_attr *na, s64 start_bit, s64 count)
{
	ntfs_log_trace("Dealloc from bit 0x%llx, count 0x%llx.\n", (LONG64)start_bit, (LONG64)count);

	return ntfs_bitmap_set_bits_in_run(na, start_bit, count, 0);
}

