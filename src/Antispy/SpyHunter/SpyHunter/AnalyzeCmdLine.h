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

//#include "wtl/atlrx.h"
#include <atlrx.h>

//���������У������Ƿ�����ļ�
BOOL GetFirstPath( LPCTSTR pszCmdLine, LPTSTR pszFileName, int nSize );

BOOL SearchFirstPath( LPCTSTR pszCmdLine, LPCTSTR* ppszStart, LPCTSTR* ppszEnd )
{
	BOOL bRet = FALSE;
	CAtlREMatchContext<> mc;
	LPCTSTR pszFilePathReg = TEXT( "([a-zA-Z]:(\\\\[^\\\\/:*?\"<>|]+)+)" );


	CAtlRegExp<> re;
	REParseError status = re.Parse( pszFilePathReg );

	if( REPARSE_ERROR_OK != status )
		goto Exit0;

	if( !re.Match( pszCmdLine, &mc ) )
		goto Exit0;


	ptrdiff_t nLength = mc.m_Match.szEnd - mc.m_Match.szStart;
	if( nLength <= 0 )
		goto Exit0;

	*ppszStart = mc.m_Match.szStart;
	*ppszEnd   = mc.m_Match.szEnd;

	bRet = TRUE;

Exit0:

	return bRet;
}


LPCTSTR HasRundll( LPCTSTR pszCmdLine )
{
	if (pszCmdLine == NULL || _tcslen(pszCmdLine) == 0)
		return NULL;

	TCHAR* pszBuffer = (TCHAR*)malloc((MAX_PATH+1)*sizeof(TCHAR));
	if( pszBuffer == NULL )
		return NULL;

	::_tcscpy_s( pszBuffer, MAX_PATH, pszCmdLine );
	::_tcslwr_s( pszBuffer, MAX_PATH );

	LPCTSTR pszRunDllText = _T( "rundll" );
	LPCTSTR pszRunDll = ::_tcsstr( pszBuffer, pszRunDllText );

	if( pszRunDll != NULL )
	{
		pszRunDll = pszCmdLine + ( pszRunDll - pszBuffer ) + ::_tcslen( pszRunDllText );
	}

	free( pszBuffer );
	return pszRunDll; // ����"rundll"�ַ����λ��
}



inline
BOOL IsNumber( TCHAR c )
{
	return ( c >= '0' ) && ( c <= '9' );
}

inline 
BOOL IsAlpha( TCHAR c )
{
	return ( ( c >= 'a' ) && ( c <= 'z' ) ) || 
		( ( c >= 'A' ) && ( c <= 'Z' ) ) ;
}

inline
BOOL IsPossibleSeperator( TCHAR c )
{
	//return ( c == ' ' ) || ( c == ',' ) || ( c == ';' ) || ( c == '-' ) ;
	return ( c < 128 ) && !IsNumber( c ) && !IsAlpha( c );
}


BOOL IsFileExists( LPCTSTR pszFile )
{
	return ::PathFileExists( pszFile ) && !PathIsDirectory( pszFile );
}


BOOL _KillParamEx( LPCTSTR pszCmdLine, LPTSTR pszFileName, int nSize )
{
	BOOL   bRet       = FALSE;
	TCHAR* pszTemp    = NULL;

	if( pszCmdLine == NULL || pszFileName == NULL || nSize <= 0 )
		goto Exit0;

	int nTempSize = nSize + 128;

	pszTemp = new TCHAR[nTempSize];
	if( pszTemp == NULL )
		goto Exit0;

	//-----------------------------------------
	// �滻��������
	DWORD dwRet = ::ExpandEnvironmentStrings( pszCmdLine, pszTemp, nTempSize);
	if( (int)dwRet > nTempSize )
		goto Exit0;

	//-----------------------------------------
	// �ҳ��������еĵ�һ��·���ַ���
	LPCTSTR pszStart = NULL;
	LPCTSTR pszEnd   = NULL;
	bRet = SearchFirstPath( pszTemp, &pszStart, &pszEnd );

	if( !bRet )
		goto Exit0;

	ptrdiff_t nLength = pszEnd - pszStart;
	if( nLength <= 0 || nLength > nSize - 5 )
		goto Exit0;

	::_tcsncpy_s( pszFileName, nSize, pszStart, nLength );


	//-----------------------------------------
	// �����ļ�
	// <1> ���Ȳ��Ҹ��ļ���, ���������򸽼��Ͽ��ܵ���չ�����в���
	// <2> �����������, ����ļ��ض�����һ���ո�, ����<1>����
	// <3> ֱ���ļ����ڻ��Ѿ��ض���'\\'.
	LPTSTR pszIter      = pszFileName + nLength;
	LPTSTR pszLastSlash = ::_tcsrchr( pszFileName, '\\' );

	if( pszLastSlash == NULL )
		goto Exit0;

	while( pszIter > pszLastSlash )
	{
		pszIter[0] = 0;
		if( ::IsFileExists( pszFileName ) )
			break;

		::_tcsncpy_s( pszIter, 5, _T( ".exe" ), 5 );
		if( ::IsFileExists( pszFileName ) )
			break;

		::_tcsncpy_s( pszIter, 5, _T( ".bat" ), 5 );
		if( ::IsFileExists( pszFileName ) )
			break;

		::_tcsncpy_s( pszIter, 5, _T( ".com" ), 5 );
		if( ::IsFileExists( pszFileName ) )
			break;

		//pszIter = ::_tcsrchr( pszFileName, ' ' );

		do 
		{
			pszIter--;
		} 
		while( pszIter > pszLastSlash && !IsPossibleSeperator( *pszIter ) );
	}


	if( pszIter <= pszLastSlash )
		goto Exit0;

	bRet = TRUE;

Exit0:
	if( bRet == FALSE && pszFileName != NULL && nSize > 0 )
	{
		pszFileName[0] = 0;
	}

	delete [] pszTemp;
	return bRet;
}




BOOL GetFirstPath( LPCTSTR pszCmdLine, LPTSTR pszFileName, int nSize )
{
	BOOL bRet = FALSE;

	LPCTSTR pszAfterRundll = HasRundll( pszCmdLine );

	if( pszAfterRundll != NULL && pszAfterRundll[0] != 0 )
	{
		bRet = _KillParamEx( pszAfterRundll, pszFileName, nSize );
	}

	if( !bRet )
	{
		bRet = _KillParamEx( pszCmdLine, pszFileName, nSize );
	}

	return bRet;
}