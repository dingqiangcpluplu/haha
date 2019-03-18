
// PolylineSmoothCPPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PolylineSmoothCPP.h"
#include "PolylineSmoothCPPDlg.h"
#include "afxdialogex.h"
#include <math.h>
#include <GdiPlus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

double PointToSegDist(int x, int y, int x1, int y1, int x2, int y2)
{
	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0) return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2) return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;
	return sqrt((x - px) * (x - px) + (py - y) * (py - y));
}
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPolylineSmoothCPPDlg dialog



CPolylineSmoothCPPDlg::CPolylineSmoothCPPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPolylineSmoothCPPDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nState = ES_INVALID;
	m_nDragIndex = -1;
	m_bDrawRegion = FALSE;
	m_Orgin.x = 0;
	m_Orgin.y = 0;
	nrOfInterpolatedPoints = 10;
}

void CPolylineSmoothCPPDlg::DoDataExchange(CDataExchange* pDX)

{
	DDX_Control(pDX, IDC_SLIDER1, m_slicer);
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPolylineSmoothCPPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON1, &CPolylineSmoothCPPDlg::OnBnClickedButton1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CPolylineSmoothCPPDlg::OnNMCustomdrawSlider1)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON2, &CPolylineSmoothCPPDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CPolylineSmoothCPPDlg message handlers

BOOL CPolylineSmoothCPPDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE)
	{
		bool bNeedPaint = false;
			if (m_nDragIndex != -1 && m_vectControlPoint.size() > 3)
			{
				std::vector<CPoint>::iterator _it = m_vectControlPoint.begin();
				int _nIndex = 0;
				while(_it != m_vectControlPoint.end())
				{
					if (_nIndex == m_nDragIndex)
					{
						m_vectControlPoint.erase(_it);
						bNeedPaint = true;
						m_nDragIndex = -1;
						break;
					}
					_nIndex++;
					_it++;
				}

			}
		if (bNeedPaint)
		{
			Invalidate();
		}
		return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

BOOL CPolylineSmoothCPPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	//初始化gdi+
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	// TODO: Add extra initialization here
	m_slicer.SetRangeMin(0);
	m_slicer.SetRangeMax(20);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPolylineSmoothCPPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void CPolylineSmoothCPPDlg::ScrToLog(std::vector<CPoint> vectIn, std::vector<CPoint>& vectOut)
{
	CRect _rc;
	GetClientRect(_rc);

	CPoint ptCenter;
	ptCenter.x = _rc.Width() / 2;
	ptCenter.y = _rc.Height() / 2;


	for (auto it : vectIn)
	{
		it = it - ptCenter;
		vectOut.push_back(it);
	}
}

void CPolylineSmoothCPPDlg::LogToScr(std::vector<CPoint> vectIn, std::vector<CPoint>& vectOut)
{

}


BOOL CPolylineSmoothCPPDlg::CatchControlPoint(CPoint ptSrc, int& nCatchIndex)
{
	int nIndex = 0;
	for (auto it : m_vectControlPoint)
	{
		if (abs(ptSrc.x - it.x) <= 2 && abs(ptSrc.y - it.y) <= 2
			&& abs(ptSrc.x - it.x) >= -2 && abs(ptSrc.y - it.y) >= -2)
		{
			nCatchIndex = nIndex;
			return TRUE;
		}
		nIndex++;
	}
	return FALSE;
}

BOOL CPolylineSmoothCPPDlg::getSplineInterpolationCatmullRom(std::vector<CPoint> vectContrlPoint, std::vector<CPoint>& vectDestPoint)
{
		// The Catmull-Rom Spline, requires at least 4 points so it is possible to extrapolate from 3 points, but not from 2.
		// you would get a straight line anyway
		if (vectContrlPoint.size()< 3)
		{
			return FALSE;
		}
		CPoint ptHead = vectContrlPoint[0];
		CPoint pHedad1 =  vectContrlPoint[1];
		CPoint ptTail = vectContrlPoint[vectContrlPoint.size() - 1];
		CPoint ptTail1 = vectContrlPoint[vectContrlPoint.size() - 2];

		//std::vector<CPoint> _vectContrlPoint;
		//ScrToLog(vectContrlPoint, _vectContrlPoint);

		// could throw an error on the following, but it is easily fixed implicitly
		// create a new pointlist to do splining on
		// if you don't do this, the original pointlist gets extended with the exptrapolated points
		std::vector<CPoint> vectDrawPoint = vectContrlPoint;
		vectDrawPoint.insert(vectDrawPoint.end(),ptHead );
		vectDrawPoint.insert(vectDrawPoint.end(),pHedad1 );

		vectDrawPoint.insert(vectDrawPoint.begin(),ptTail );
		vectDrawPoint.insert(vectDrawPoint.begin(),ptTail1 );

		// always extrapolate the first and last point out
		//int dx = vectDrawPoint[1].x - vectDrawPoint[0].x;
		//int dy = vectDrawPoint[1].y - vectDrawPoint[0].y;
		//vectDrawPoint.insert(vectDrawPoint.begin(), CPoint(vectDrawPoint[0].x - dx, vectDrawPoint[0].y - dy));
		//vectDrawPoint.insert(vectDrawPoint.begin(), vectDrawPoint.back());/*
		//dx = vectDrawPoint[vectDrawPoint.size() - 1].x - vectDrawPoint[vectDrawPoint.size() - 2].x;
		//dy = vectDrawPoint[vectDrawPoint.size() - 1].y - vectDrawPoint[vectDrawPoint.size() - 2].y;
		//vectDrawPoint.insert(vectDrawPoint.end(), CPoint(vectDrawPoint[vectDrawPoint.size()- 1].x + dx, vectDrawPoint[vectDrawPoint.size()- 1].y + dy));*/
		//vectDrawPoint.insert(vectDrawPoint.end(), vectDrawPoint[1]);

		// Note the nrOfInterpolatedPoints acts as a kind of tension factor between 0 and 1 because it is normalised
		// to 1/nrOfInterpolatedPoints. It can never be 0
		double t = 0;
		CPoint spoint;
		std::vector<CPoint> spline;
		 m_rectBound.left = INT_MAX;
		 m_rectBound.top = INT_MAX;
		 m_rectBound.right = INT_MIN;
		 m_rectBound.bottom = INT_MIN;
		int loopTo = vectDrawPoint.size() - 4;
		for (int i = 0; i <= loopTo; i++)
		{
			int loopTo1 = nrOfInterpolatedPoints - 1;
			for (int intp = 0; intp <= loopTo1; intp++)
			{
				t = 1 / (double)nrOfInterpolatedPoints * intp;

				spoint.x = 0.5 * (2 * vectDrawPoint[i + 1].x + (-1 * vectDrawPoint[i].x + vectDrawPoint[i + 2].x) * t + (2 * vectDrawPoint[i].x - 5 * vectDrawPoint[i + 1].x + 4 * vectDrawPoint[i + 2].x - vectDrawPoint[i + 3].x) * pow(t, 2) + (-1 * vectDrawPoint[i].x + 3 * vectDrawPoint[i + 1].x - 3 * vectDrawPoint[i + 2].x + vectDrawPoint[i + 3].x) * pow(t, 3));

				spoint.y = 0.5 * (2 * vectDrawPoint[i + 1].y + (-1 * vectDrawPoint[i].y + vectDrawPoint[i + 2].y) * t + (2 * vectDrawPoint[i].y - 5 * vectDrawPoint[i + 1].y + 4 * vectDrawPoint[i + 2].y - vectDrawPoint[i + 3].y) * pow(t, 2) + (-1 * vectDrawPoint[i].y + 3 * vectDrawPoint[i + 1].y - 3 * vectDrawPoint[i + 2].y + vectDrawPoint[i + 3].y) * pow(t, 3));

				m_rectBound.left = spoint.x < m_rectBound.left ? spoint.x : m_rectBound.left;
				m_rectBound.top = spoint.y < m_rectBound.top ? spoint.y : m_rectBound.top;
				m_rectBound.right = spoint.x > m_rectBound.right ? spoint.x : m_rectBound.right;
				m_rectBound.bottom = spoint.y > m_rectBound.bottom ? spoint.y : m_rectBound.bottom;
				vectDestPoint.push_back(spoint);
			}
		}

		std::vector<CPoint> _newPoint;
		for (int i = 0; i < vectDestPoint.size(); i++)
		{
			if (i > nrOfInterpolatedPoints - 1)
			{
				_newPoint.push_back(vectDestPoint[i]);
			}
		}
		vectDestPoint = _newPoint;
		// add the last point, but skip the interpolated last point, so second last...
		vectDestPoint.push_back(vectContrlPoint[0]);
		return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPolylineSmoothCPPDlg::OnPaint()
{
	//CDialogEx::OnPaint();
	CPaintDC _dc(this);
	CRect rc;
	GetClientRect(rc);
	_dc.FillSolidRect(rc, RGB(255,255,255));
	for (auto it : m_vectControlPoint)
	{

		_dc.FillSolidRect(it.x - 2, it.y - 2, 4, 4, RGB(255,0,0));
		CString _str;
		//_str.Format(_T("(%d,%d)"), it.x,it.y);
		//_dc.TextOut(it.x,it.y, _str);
	}
	if (m_vectControlPoint.size()<3)
	{
		return;
	}

	m_vectDrawPoint.clear();
	getSplineInterpolationCatmullRom(m_vectControlPoint, m_vectDrawPoint);

	_dc.MoveTo(m_vectDrawPoint[0]);
	for (auto it : m_vectDrawPoint)
	{
		_dc.LineTo(it);
	}
//	_dc.LineTo(m_vectDrawPoint[0]);

	Gdiplus::Graphics graphics(_dc.GetSafeHdc());
	graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
	Gdiplus::PointF* pGaussCurve = new Gdiplus::PointF[m_vectControlPoint.size()];
	int index = 0;
	for (auto i : m_vectControlPoint)
	{
		pGaussCurve[index].X = i.x;
		pGaussCurve[index].Y = i.y;
		index++;
	}

	Gdiplus::Color colorWhite(0xFF, 0xFF, 0x00, 0x00);//White
	Gdiplus::Pen penCurve(colorWhite, 1.0F);
	//graphics.DrawClosedCurve(&penCurve, pGaussCurve, m_vectControlPoint.size());

	delete pGaussCurve;
	pGaussCurve = NULL;
	if (m_nDragIndex != -1)
	{
		CPoint pt = m_vectControlPoint[m_nDragIndex];
		CRect _rectSelect(pt.x - 4, pt.y - 4, pt.x + 4, pt.y + 4);
		_dc.Rectangle(_rectSelect);
	}

	if (m_nState == ES_DRAG)
	{
		std::vector<CPoint> _vectDragPoint;
		getSplineInterpolationCatmullRom(m_vectDrawDragPoint, _vectDragPoint);

		CPen pen;
		pen.CreatePen(PS_DOT , 1 , RGB( 0 , 255 , 255 ) );
		CPen* pOldPen = _dc.SelectObject(&pen);
		_dc.MoveTo(_vectDragPoint[0]);
		for (auto it : _vectDragPoint)
		{
			_dc.LineTo(it);
		}
		//_dc.LineTo(_vectDragPoint[0]);
		_dc.SelectObject(pOldPen);
	}

	if (m_nState == ES_MOVEROI)
	{
		CPoint ptMove = m_ptMoveEnd - m_ptMoveStart;
		CRect _rc = m_rectBound;
		_rc.left += ptMove.x;
		_rc.right += ptMove.x;
		_rc.top += ptMove.y;
		_rc.bottom += ptMove.y;
		

		CPen penBound;
		penBound.CreatePen(PS_DOT , 1 , RGB( 255 , 0 , 0) );
		CPen* pOldPen = _dc.SelectObject(&penBound);
		_dc.SelectStockObject(NULL_BRUSH);
		int bk = _dc.SetBkMode(TRANSPARENT );
		_dc.Rectangle(_rc);
		_dc.SelectObject(pOldPen);
		_dc.SetBkMode(bk);
	}

	CPen pen;
	pen.CreatePen(PS_DOT , 4 , RGB( 0 , 0 , 255 ) );
	CPen* pOldPen = _dc.SelectObject(&pen);
	//for (auto i : insertVect)
	//{
	//	_dc.MoveTo(CPoint(i.x1, i.y1));
	//	_dc.LineTo(CPoint(i.x2, i.y2));
	//}
	DWORD _dw = GetTickCount();
	if (m_bDrawRegion && m_vectDrawPoint.size() > 1)
	{
		CRgn rgn;
		rgn.CreatePolygonRgn(&m_vectDrawPoint[0], m_vectDrawPoint.size(), ALTERNATE);


		CPen pen;
		pen.CreatePen(PS_DOT , 4 , RGB( 0 , 0 , 255 ) );
		CPen* pOldPen = _dc.SelectObject(&pen);
		GetClientRect(rc);
		for (int y = 0; y < rc.bottom; y++)
		{
			for (int x = 0; x < rc.right; x++)
			{
				if (rgn.PtInRegion(CPoint(x,y)))
				{
					_dc.SetPixel(CPoint(x,y), RGB(0,0,255));
				}

			}
		}
		_dc.SelectObject(pOldPen);
		m_bDrawRegion = FALSE;
	}
	_dw = GetTickCount() - _dw;
	if (m_bSelect && m_nState != ES_MOVEROI)
	{
		CPen penBound;
		penBound.CreatePen(PS_DOT , 1 , RGB( 255 , 0 , 0) );
		CPen* pOldPen = _dc.SelectObject(&penBound);
		_dc.SelectStockObject(NULL_BRUSH);
		int bk = _dc.SetBkMode(TRANSPARENT );
		_dc.Rectangle(m_rectBound);
		_dc.SelectObject(pOldPen);
		_dc.SetBkMode(bk);
	}

}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPolylineSmoothCPPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



bool CPolylineSmoothCPPDlg::intersection(const Line &l1, const Line &l2)
{
	//快速排斥实验
	if ((l1.x1 > l1.x2 ? l1.x1 : l1.x2) < (l2.x1 < l2.x2 ? l2.x1 : l2.x2) ||
		(l1.y1 > l1.y2 ? l1.y1 : l1.y2) < (l2.y1 < l2.y2 ? l2.y1 : l2.y2) ||
		(l2.x1 > l2.x2 ? l2.x1 : l2.x2) < (l1.x1 < l1.x2 ? l1.x1 : l1.x2) ||
		(l2.y1 > l2.y2 ? l2.y1 : l2.y2) < (l1.y1 < l1.y2 ? l1.y1 : l1.y2))
	{
		return false;
	}
	//跨立实验
	if ((((l1.x1 - l2.x1)*(l2.y2 - l2.y1) - (l1.y1 - l2.y1)*(l2.x2 - l2.x1))*
		((l1.x2 - l2.x1)*(l2.y2 - l2.y1) - (l1.y2 - l2.y1)*(l2.x2 - l2.x1))) > 0 ||
		(((l2.x1 - l1.x1)*(l1.y2 - l1.y1) - (l2.y1 - l1.y1)*(l1.x2 - l1.x1))*
		((l2.x2 - l1.x1)*(l1.y2 - l1.y1) - (l2.y2 - l1.y1)*(l1.x2 - l1.x1))) > 0)
	{
		return false;
	}
	return true;
}

float get_lines_arctan(float line_1_k, float line_2_k, int aaa)
{
	if (aaa == 0)
	{
		float tan_k = 0; //直线夹角正切值
		float lines_arctan;//直线斜率的反正切值
		tan_k = (line_2_k - line_1_k) / (1 + line_2_k*line_1_k); //求直线夹角的公式
		lines_arctan = atan(tan_k);
		return lines_arctan;
	}
	else
	{
		float tan_k = 0; //直线夹角正切值
		float lines_arctan;//直线斜率的反正切值
		tan_k = (line_2_k - line_1_k) / (1 + line_2_k*line_1_k); //求直线夹角的公式
		lines_arctan = atan(tan_k)* 180.0 / 3.1415926;

		return lines_arctan;
	}
}

#define PI 3.1415926535
double Angle(CPoint o, CPoint s, CPoint e)
{
	double cosfi = 0;
	double fi =0;
	double norm = 0;
	double dsx = s.x - o.x;
	double dsy = s.y - o.y;
	double dex = e.x - o.x;
	double dey = e.y - o.y;
	cosfi = dsx*dex + dsy*dey;
	norm = (dsx*dsx + dsy*dsy)*(dex*dex + dey*dey);
	cosfi /= sqrt(norm);

	if (cosfi >= 1.0)
	{
		return 0;
	}

	if (cosfi <= -1.0)
	{
		return PI;
	}
	fi = acos(cosfi);

	if (180*fi/PI<180)
	{
		return 180*fi/PI;
	}
	else
	{
		return 360 - 180*fi/PI;
	}
}

BOOL CPolylineSmoothCPPDlg::ptInRegion(CPoint point)
{
	//射线法求交点
		Line test;
	test.x1 = point.x;
	test.y1 = point.y;
	test.x2 = -INT_MAX;
	test.y2 = point.y;
	int nInsertCounts = 0;
	insertVect.clear();
	bool bIsBeforeInsert = false;
	std::vector<int> vectIndex;
	for (int i = 0; i < m_vectDrawPoint.size(); i++)
	{
		Line temp;
		if (i == m_vectDrawPoint.size() - 1)
		{
			temp.x1 = m_vectDrawPoint[i].x;
			temp.y1 = m_vectDrawPoint[i].y;
			temp.x2 = m_vectDrawPoint[0].x;
			temp.y2 = m_vectDrawPoint[0].y;

		}
		else
		{
			temp.x1 = m_vectDrawPoint[i].x;
			temp.y1 = m_vectDrawPoint[i].y;
			temp.x2 = m_vectDrawPoint[i + 1].x;
			temp.y2 = m_vectDrawPoint[i + 1].y;

		}
		if (intersection(test, temp) && bIsBeforeInsert== false )
		{
			bIsBeforeInsert = true;
			insertVect.push_back(temp);
			vectIndex.push_back(i);
			nInsertCounts++;
		}
		else
		{

			bIsBeforeInsert = false;
		}
	}


	if (nInsertCounts % 2 == 1)
	{
		return TRUE;
	}
	return FALSE;

	// 角度法
	//double nSum = 0.;
	//for (int i = 0; i < m_vectDrawPoint.size(); i++)
	//{
	//	if (i == m_vectDrawPoint.size() - 1)
	//	{
	//		nSum += Angle(point, m_vectDrawPoint[i],m_vectDrawPoint[0]);
	//	}
	//	else
	//	{

	//		nSum += Angle(point, m_vectDrawPoint[i],m_vectDrawPoint[i+1]);
	//	}
	//}

	//if (abs(nSum - 360)<0.0000001)
	//{
	//	return TRUE;
	//}

	// CRgn 法
	if (m_vectDrawPoint.size() < 1)
	{
		return FALSE;
	}
	CRgn rgn;
	rgn.CreatePolygonRgn(&m_vectDrawPoint[0], m_vectDrawPoint.size(), ALTERNATE);
	

	return rgn.PtInRegion(point);;

}

void CPolylineSmoothCPPDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//if (ptInRegion(point))
	//{
	//	SetDlgItemText(IDC_STATIC, _T("在区域内"));
	//}
	//else
	//{

	//	SetDlgItemText(IDC_STATIC, _T("不在区域内"));
	//}
	if (m_nState == ES_ADDPOINT)
	{
		m_vectControlPoint.push_back(point);
		Invalidate();
		return;
	}

	// TODO: Add your message handler code here and/or call default
	m_nDragIndex = -1;
	m_bSelect = FALSE;
	if (CatchControlPoint(point, m_nDragIndex))
	{
		//m_bSelect = TRUE;
		m_nState = ES_SELECTEDCTRLPOINT;
	}
	else
	{
		if (m_vectDrawPoint.size() > 0)
		{
			for (int i = 0; i < m_vectDrawPoint.size() - 1; i++)
			{

				double nLength = PointToSegDist(point.x, point.y, m_vectDrawPoint[i].x, m_vectDrawPoint[i].y, m_vectDrawPoint[i+1].x, m_vectDrawPoint[i+1].y);
				if (nLength < 5)
				{
					m_nState = ES_SELECTEDROI;
					m_bSelect = TRUE;
					m_ptMoveStart = point;
				}
			}
		}
	}

	Invalidate();
	CDialogEx::OnLButtonDown(nFlags, point);
}



void CPolylineSmoothCPPDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_vectControlPoint.clear();
	m_vectDrawPoint.clear();
	Invalidate(TRUE);
	CDialogEx::OnRButtonDblClk(nFlags, point);
}



void CPolylineSmoothCPPDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_nState == ES_ADDPOINT)
	{
		m_nState = ES_INVALID;
	}

	// TODO: Add your message handler code here and/or call default
	int _nIndexInControl = -1;
	bool bInserted = false;
	for (int i = 0; i < m_vectDrawPoint.size() - 1; i++)
	{
		int nIndex = 0;
		for (int j = 0; j < m_vectControlPoint.size(); j++)
		{
			if (m_vectDrawPoint[i] == m_vectControlPoint[j])
			{
				_nIndexInControl = j;
				break;
			}
		}

		double nLength = PointToSegDist(point.x, point.y, m_vectDrawPoint[i].x, m_vectDrawPoint[i].y, m_vectDrawPoint[i+1].x, m_vectDrawPoint[i+1].y);
		if (nLength <= 5)
		{
			std::vector<CPoint>::iterator _it = m_vectControlPoint.begin();
			int nindex = 0;
			while(_it != m_vectControlPoint.end())
			{
				if (nindex == _nIndexInControl + 1)
				{
					m_vectControlPoint.insert(_it, point);
					bInserted = true;
					break;
				}
				_it++;
				nindex++;
			}
			if (!bInserted)
			{
				m_vectControlPoint.push_back(point);
			}

			bInserted = true;
			Invalidate();
		}

		if (bInserted)
		{
			break;
		}
		
	}
	Invalidate(TRUE);
	CDialogEx::OnLButtonDblClk(nFlags, point);
}




void CPolylineSmoothCPPDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_nState == ES_SELECTEDCTRLPOINT)
	{
		m_nState = ES_DRAG;
	}

	if (m_nState == ES_DRAG)
	{

		m_vectDrawDragPoint = m_vectControlPoint;
		m_vectDrawDragPoint[m_nDragIndex] = point;
		Invalidate();
	}
	if (m_nState == ES_SELECTEDROI || m_nState == ES_MOVEROI)
	{
		m_nState = ES_MOVEROI;
		m_ptMoveEnd = point;
		Invalidate();
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

 void CPolylineSmoothCPPDlg::OnHScroll( 
	UINT nSBCode, 
	UINT nPos, 
	CScrollBar* pScrollBar  
	) 
{
	if (nSBCode == SB_ENDSCROLL )
	{
		return;
	}
	nrOfInterpolatedPoints = nPos;
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
	Invalidate();
}

void CPolylineSmoothCPPDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_nState == ES_DRAG || m_nState == ES_SELECTEDCTRLPOINT || m_nState == ES_MOVEROI)
	{
		if (m_nState == ES_DRAG)
		{
			m_vectControlPoint = m_vectDrawDragPoint;
		}
		if (m_nState == ES_MOVEROI)
		{

			CPoint ptMove = m_ptMoveEnd - m_ptMoveStart;
			for (auto& i : m_vectControlPoint)
			{
				i += ptMove;
			}
		}
	}
	m_nState = ES_INVALID;

	Invalidate();
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CPolylineSmoothCPPDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_nState = ES_DRAG)
	{
		m_nState = ES_INVALID;
		Invalidate();
	}
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CPolylineSmoothCPPDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	bool bNeedPaint = false;
	if (nChar == 46)
	{
		if (m_nDragIndex != -1 && m_vectControlPoint.size() > 3)
		{
			std::vector<CPoint>::iterator _it = m_vectControlPoint.begin();
			int _nIndex = 0;
			while(_it != m_vectControlPoint.end())
			{
				if (_nIndex == m_nDragIndex)
				{
					m_vectControlPoint.erase(_it);
					bNeedPaint = true;
					m_nDragIndex = -1;
					break;
				}
				_nIndex++;
				_it++;
			}
			
		}
	}
	if (bNeedPaint)
	{
		Invalidate();
	}
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

struct CPoint2D
{
	double x;
	double y;
	CPoint2D()
	{
		x = 0.;
		y = 0.;
	}
	CPoint2D(double x, double y)
	{
		x = 0.;
		y = 0.;
	}

	CPoint2D& operator= (CPoint2D& a)
	{
		x = a.x;
		y = a.y;
		return *this;
	}
};
float bezier3funcX(float uu,CPoint2D *controlP){  
	float part0 = controlP[0].x * uu * uu * uu;  
	float part1 = 3 * controlP[1].x * uu * uu * (1 - uu);  
	float part2 = 3 * controlP[2].x * uu * (1 - uu) * (1 - uu);  
	float part3 = controlP[3].x * (1 - uu) * (1 - uu) * (1 - uu);     
	return part0 + part1 + part2 + part3;   
}      
float bezier3funcY(float uu,CPoint2D *controlP){  
	float part0 = controlP[0].y * uu * uu * uu;  
	float part1 = 3 * controlP[1].y * uu * uu * (1 - uu);  
	float part2 = 3 * controlP[2].y * uu * (1 - uu) * (1 - uu);  
	float part3 = controlP[3].y * (1 - uu) * (1 - uu) * (1 - uu);     
	return part0 + part1 + part2 + part3;   
}  
void createCurve(CPoint2D *originPoint,int originCount,std::vector<CPoint2D> &curvePoint){  
	//控制点收缩系数 ，经调试0.6较好，CPoint是opencv的，可自行定义结构体(x,y)  
	float scale = 0.6;  
	CPoint2D* midpoints = new CPoint2D[originCount];  
	//生成中点       
	for(int i = 0 ;i < originCount ; i++){      
		int nexti = (i + 1) % originCount;  
		midpoints[i].x = (originPoint[i].x + originPoint[nexti].x)/2.0;  
		midpoints[i].y = (originPoint[i].y + originPoint[nexti].y)/2.0;  
	}      

	//平移中点  
	CPoint2D* extrapoints = new CPoint2D[2 * originCount];   
	for(int i = 0 ;i < originCount ; i++){  
		int nexti = (i + 1) % originCount;  
		int backi = (i + originCount - 1) % originCount;  
		CPoint2D midinmid;  
		midinmid.x = (midpoints[i].x + midpoints[backi].x)/2.0;  
		midinmid.y = (midpoints[i].y + midpoints[backi].y)/2.0;  
		int offsetx = originPoint[i].x - midinmid.x;  
		int offsety = originPoint[i].y - midinmid.y;  
		int extraindex = 2 * i;  
		extrapoints[extraindex].x = midpoints[backi].x + offsetx;  
		extrapoints[extraindex].y = midpoints[backi].y + offsety;  
		//朝 originPoint[i]方向收缩   
		int addx = (extrapoints[extraindex].x - originPoint[i].x) * scale;  
		int addy = (extrapoints[extraindex].y - originPoint[i].y) * scale;  
		extrapoints[extraindex].x = originPoint[i].x + addx;  
		extrapoints[extraindex].y = originPoint[i].y + addy;  

		int extranexti = (extraindex + 1)%(2 * originCount);  
		extrapoints[extranexti].x = midpoints[i].x + offsetx;  
		extrapoints[extranexti].y = midpoints[i].y + offsety;  
		//朝 originPoint[i]方向收缩   
		addx = (extrapoints[extranexti].x - originPoint[i].x) * scale;  
		addy = (extrapoints[extranexti].y - originPoint[i].y) * scale;  
		extrapoints[extranexti].x = originPoint[i].x + addx;  
		extrapoints[extranexti].y = originPoint[i].y + addy;  

	}      

	CPoint2D controlPoint[4];  
	//生成4控制点，产生贝塞尔曲线  
	for(int i = 0 ;i < originCount ; i++){  
		controlPoint[0] = originPoint[i];  
		int extraindex = 2 * i;  
		controlPoint[1] = extrapoints[extraindex + 1];  
		int extranexti = (extraindex + 2) % (2 * originCount);  
		controlPoint[2] = extrapoints[extranexti];  
		int nexti = (i + 1) % originCount;  
		controlPoint[3] = originPoint[nexti];      
		float u = 1;  
		while(u >= 0){  
			int px = bezier3funcX(u,controlPoint);  
			int py = bezier3funcY(u,controlPoint);  
			//u的步长决定曲线的疏密  
			u -= 0.005;  
			CPoint2D tempP (px,py);  
			//存入曲线点   
			curvePoint.push_back(tempP);  
		}      
	}  
}  

void CPolylineSmoothCPPDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	m_bDrawRegion = TRUE;
	Invalidate();
}


void CPolylineSmoothCPPDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CPolylineSmoothCPPDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	m_nState = ES_ADDPOINT;
}
