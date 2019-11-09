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
#pragma once
// CMyEdit
class CMyEdit : public CEdit
{
	DECLARE_DYNAMIC(CMyEdit)

public:
	CMyEdit();
	virtual ~CMyEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void SetBackColor(COLORREF rgb);//�����ı��򱳾�ɫ
	void SetTextColor(COLORREF rgb);//�����ı����������ɫ
	void SetTextFont(const LOGFONT &lf);//��������
	COLORREF GetBackColor(void){return m_crBackGnd;}//��ȡ��ǰ����ɫ
	COLORREF GetTextColor(void){return m_crText;}//��ȡ��ǰ�ı�ɫ
	BOOL GetTextFont(LOGFONT &lf);//��ȡ��ǰ����

private:
	COLORREF m_crText;//�������ɫ
	COLORREF m_crBackGnd;//����ı�����ɫ
	CFont m_font;//����
	CBrush m_brBackGnd;//�����ı����Ļ�ˢ
	CFont* m_pFont;

	//�ؼ��Լ�����Ϣ���亯��CtlColor,���ƿؼ�֮ǰ����
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};