
// PolylineSmoothCPPDlg.h : header file
//

#pragma once
#include <vector>

// CPolylineSmoothCPPDlg dialog
class CPolylineSmoothCPPDlg : public CDialogEx
{
// Construction
public:
	CPolylineSmoothCPPDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_POLYLINESMOOTHCPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	BOOL PreTranslateMessage(MSG* pMsg);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	BOOL getSplineInterpolationCatmullRom(std::vector<CPoint> vectContrlPoint, std::vector<CPoint>& vectDrawPoint);
	void ScrToLog(std::vector<CPoint> vectIn, std::vector<CPoint>& vectOut);
	void LogToScr(std::vector<CPoint> vectIn, std::vector<CPoint>& vectOut);

	BOOL CatchControlPoint(CPoint ptSrc, int& nCatchIndex);
	BOOL ptInRegion(CPoint point);
	int m_nDragIndex;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	std::vector<CPoint> m_vectControlPoint;
	std::vector<CPoint> m_vectDrawPoint;
	std::vector<CPoint> m_vectDrawDragPoint;

	
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHScroll( 
		UINT nSBCode, 
		UINT nPos, 
		CScrollBar* pScrollBar  
		);

	enum EditState
	{
		ES_INVALID,
		ES_ADDPOINT,
		ES_SELECTEDCTRLPOINT,
		ES_SELECTEDROI,
		ES_MOVEROI,
		ES_DRAG,
	} m_nState;
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	struct Line {
		double x1;
		double y1;
		double x2;
		double y2;
	};
	std::vector<Line> insertVect;
	bool intersection(const Line &l1, const Line &l2);
	afx_msg void OnBnClickedButton1();
	BOOL m_bDrawRegion;
	CRect m_rectBound;
	BOOL m_bSelect;
	CPoint m_Orgin;
	CPoint m_ptMoveStart;
	CPoint m_ptMoveEnd;
	std::vector<CPoint> m_vectMovePoint;
	int nrOfInterpolatedPoints;
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_slicer;
	afx_msg void OnBnClickedButton2();
};
