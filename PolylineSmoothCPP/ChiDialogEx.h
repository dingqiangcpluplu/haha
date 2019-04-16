// ChiDialogEx.h: interface for the CChiDialogEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHIDIALOGEX_H__DDAB3667_952C_4082_8E59_04CB046593BC__INCLUDED_)
#define AFX_CHIDIALOGEX_H__DDAB3667_952C_4082_8E59_04CB046593BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChiDialog.h"

class CChiDialogEx : public CChiDialog  
{
protected:
	// document variable
	CObject * m_piDoc;	// Document linker

	// Screen Layout variable
	CRect	m_WndRect;
	UINT	m_WndFlag;	// see CDWF defines
						// Notice:  this value must set in constructor
	COLORREF m_BkColor;	// Background Color
	COLORREF m_backcolor;
	

// Construction
public:
	CChiDialogEx(CWnd* pParent = NULL);   // standard constructor
	CChiDialogEx(int id , CWnd* pParent = NULL);   // standard constructor
	void SetLocation( CRect rect );
	void SetBkColor( COLORREF color  , BOOL update = TRUE);
	void DrawPanelBackupBk(CDC * dc);
	virtual void DrawAfterPaint( CDC & dc );
	virtual void iDrawClient( void );
	
	// Documment Module
	void	SetDocumentLink( CObject * pDoc );
	CObject * GetDocument( void );
	virtual LRESULT	DirectMessage( int id , int param1 = 0 , int param2 = 0 , CString * retMessage = NULL );	// for direct update

// Dialog Data
	//{{AFX_DATA(CChiDialogEx)
	enum { IDD = 101 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChiDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChiDialog)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnDrawClient(WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHIDIALOG_H__1DD5E811_4833_11D4_822A_0050DA05C64C__INCLUDED_)
