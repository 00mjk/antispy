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
#include "MzfEdit.h"
// CMyEdit
IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{
	// ��ʼ��Ϊϵͳ����ʹ�����ɫ
// 	m_crText = GetSysColor(COLOR_WINDOWTEXT);
// 	m_crBackGnd = GetSysColor(COLOR_WINDOW);
// 	m_font.CreatePointFont(90, L"����"); 
// 	m_brBackGnd.CreateSolidBrush(GetSysColor(COLOR_WINDOW));

	m_crText = RGB(255,255,255);
	m_crBackGnd = RGB(0,0,0);
	m_font.CreatePointFont(150, L"����"); 
	m_brBackGnd.CreateSolidBrush(RGB(0,0,0));

// 	m_pFont = new CFont;
// 	m_pFont->CreateFont(-13,0,0,0,/*FW_BOLD:*/FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,FALSE,DEFAULT_PITCH,/*L"����"*/NULL);
}

CMyEdit::~CMyEdit()
{
// 	delete m_pFont;
// 	m_pFont = NULL;
}

BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	//WM_CTLCOLOR����Ϣ����
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CMyEdit ��Ϣ�������

void CMyEdit::SetBackColor(COLORREF rgb)
{
	//�������ֱ�����ɫ
	m_crBackGnd = rgb;

	//�ͷžɵĻ�ˢ
	if (m_brBackGnd.GetSafeHandle())
		m_brBackGnd.DeleteObject();
	//ʹ�����ֱ�����ɫ�����µĻ�ˢ,ʹ�����ֿ򱳾������ֱ���һ��
	m_brBackGnd.CreateSolidBrush(rgb);
	//redraw
	Invalidate(TRUE);
}
void CMyEdit::SetTextColor(COLORREF rgb)
{
	//����������ɫ
	m_crText = rgb;
	//redraw
	Invalidate(TRUE);
}
void CMyEdit::SetTextFont(const LOGFONT &lf)
{
	//�����µ�����,������ΪCEDIT������
	if(m_font.GetSafeHandle())
	{
		m_font.DeleteObject();
	}
	m_font.CreateFontIndirect(&lf);
	CEdit::SetFont(&m_font);
	//redraw
	Invalidate(TRUE);
}

BOOL CMyEdit::GetTextFont(LOGFONT &lf)
{ 
	if(m_font.GetLogFont(&lf)!=0)
	{      return TRUE;   }
	return FALSE;
}

HBRUSH CMyEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	//ˢ��ǰ�����ı���ɫ
	pDC->SetTextColor(m_crText);

	//ˢ��ǰ�����ı�����
	pDC->SetBkColor(m_crBackGnd);

//	pDC->SelectObject(m_pFont->m_hObject);
// 	HFONT hOldFont = NULL;
// 	if (m_pFont != NULL) 
// 	{
// 		hOldFont =  (HFONT)pMemDC->SelectObject(m_pFont->m_hObject);
// 	}

	//���ػ�ˢ,�������������ؼ�����
	return m_brBackGnd;
}

BOOL CMyEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;
//	return CEdit::OnEraseBkgnd(pDC);
}
