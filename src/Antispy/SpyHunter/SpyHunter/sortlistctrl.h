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
#ifndef _SORTLISTCTRL_INCLUDE__
#define _SORTLISTCTRL_INCLUDE__

/////////////////////////////////////////////////////////////////////////////////////////
//һ�������ͷʱ����������б���
class CSortListCtrl : public CListCtrl
{
// Construction
public:
	CSortListCtrl();
	void EnableSort(BOOL bEnable = TRUE){m_bEnableSort = bEnable;};
// Attributes
public:
	struct Info
	{
		CSortListCtrl* pListCtrl;
		int nSubItem;
		BOOL bAsc;		//�Ƿ�������
		BOOL bIsNumber;	//�����Ƿ�������
	};

	// Summary: �ı���ͷͼ�������״̬���������µ�����״̬��bClearΪ��ʱ�����ͷͼ�ꡣ
	BOOL ChangeHeardCtrlState(CHeaderCtrl* pHeardCtrl, int nItem, BOOL bClear);

	static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortListCtrl();

	// Generated message map functions
protected:
	CBitmap* MakeColorBoxImage(BOOL bUp);
	CImageList m_ImageList;
	BOOL m_bInit;
	BOOL m_bEnableSort;
	CBitmap *m_pBmp[2];

	//{{AFX_MSG(CSortListCtrl)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif