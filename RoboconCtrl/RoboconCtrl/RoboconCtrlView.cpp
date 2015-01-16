
// RoboconCtrlView.cpp : implementation of the CRoboconCtrlView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "RoboconCtrl.h"
#endif

#include "RoboconCtrlDoc.h"
#include "RoboconCtrlView.h"

#include "RobotMCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <gl/GL.h>
#include <gl/GLU.h>
#include <sstream>

// CRoboconCtrlView

IMPLEMENT_DYNCREATE(CRoboconCtrlView, CView)

BEGIN_MESSAGE_MAP(CRoboconCtrlView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_SEND_STRING, refresh_coordinates)
END_MESSAGE_MAP()

// CRoboconCtrlView construction/destruction

CRoboconCtrlView::CRoboconCtrlView()
{
	// TODO: add construction code here
	current_pos.x = 0.0f;
	current_pos.y = 0.0f;
	current_pos.valid = FALSE;

	cursor_pos.x = 0.0f;
	cursor_pos.y = 0.0f;
	cursor_pos.valid = FALSE;
}

CRoboconCtrlView::~CRoboconCtrlView()
{
}

BOOL CRoboconCtrlView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CS_OWNDC);
	return CView::PreCreateWindow(cs);
}

BOOL CRoboconCtrlView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

int CRoboconCtrlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}

	int pixel_format; // index of pixel format
	m_hDC = ::GetDC(m_hWnd);
	static PIXELFORMATDESCRIPTOR p = {
		sizeof(PIXELFORMATDESCRIPTOR), // size of struct
		1, // struct version
		PFD_DRAW_TO_WINDOW | //draw to window
		PFD_SUPPORT_OPENGL | //support opengl
		PFD_DOUBLEBUFFER, //double buffer mode
		PFD_TYPE_RGBA, //color mode rgba
		24, //24 bit color
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //options not used (13 0s)
		32, //size of depth buffer
		0, 0, //options not used (2 0s)
		PFD_MAIN_PLANE,
		0, 0, 0, 0 // options not used
	};
	// Choose the pixel format
	pixel_format = ChoosePixelFormat(m_hDC, &p);

	// set pixel format and verify that it is valid
	VERIFY(SetPixelFormat(m_hDC, pixel_format, &p));
	m_hRC = wglCreateContext(m_hDC);

	// make rendering context current, the initializes and deselects it
	VERIFY(wglMakeCurrent(m_hDC, m_hRC));

	// Set clearing background color 
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClearDepth(1.0f);
	SetTimer(1, 1, 0);

	OnDraw(NULL);

	return 0;
}

void CRoboconCtrlView::OnDestroy()
{
	wglMakeCurrent(m_hDC, NULL);
	wglDeleteContext(m_hRC);
	CView::OnDestroy();
}

// CRoboconCtrlView drawing

CRoboconCtrlView::GLCoord CRoboconCtrlView::GetGLCoord(CPoint wndCoord)
{
	CRect rect;
	GetClientRect(rect);
	GLCoord g;
	g.x = -(float)(((double)rect.Width() / 2 - (double)(wndCoord.x + 1)) / ((double)gl_width / 2));
	g.y = (float)(((double)rect.Height() / 2 - (double)(wndCoord.y + 1)) / ((double)gl_height / 2));

	if (g.x > 1.0f || g.y > 1.0f) {
		g.valid = FALSE;
	}
	else {
		g.valid = TRUE;
	}

	return g;
}

std::pair<CPoint, BOOL> CRoboconCtrlView::GetRobotCoord() {
	// CRect rect;
	// GetClientRect(rect);

	// start height = (height - gl_height) /2 scale to 3050
	// end height = (height + gl_height) / 2 scale to -3050
	// start width = (width - gl_width) /2
	// end width = (width + gl_width)/2

	int x, y = 0;

	if (cursor_pos.x < 0) {
		x = (int)-(cursor_pos.y * 3050.0);
		y = (int)((1.0 + cursor_pos.x) * 6700.0);
	}
	else {
		x = (int)(cursor_pos.y * 3050.0);
		y = (int)((1.0 - cursor_pos.x) * 6700.0);
	}

	CPoint p;
	p.SetPoint(x, y);

	return std::make_pair(p, cursor_pos.valid);
}
void CRoboconCtrlView::GLDrawScene()
{
	glBegin(GL_QUADS);
		glColor3f((103.0f / 255.0f), (196.0f / 255.0f), (100.0f / 255.0f));

		// Rectangle for game field
		glVertex2f(-1.0f, -1.0f);
		glVertex2f(-1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(1.0f, -1.0f);

		glColor3f((255.0f / 255.0f), (196.0f / 255.0f), (100.0f / 255.0f));

		// Serving zones
		glVertex2f(3.945f / 6.7f, 0.55f / 3.05f);
		glVertex2f(3.945f / 6.7f, 2.57f / 3.05f);
		glVertex2f(5.195f / 6.7f, 2.57f / 3.05f);
		glVertex2f(5.195f / 6.7f, 0.55f / 3.05f);

		glVertex2f(-3.945f / 6.7f, -0.55f / 3.05f);
		glVertex2f(-3.945f / 6.7f, -2.57f / 3.05f);
		glVertex2f(-5.195f / 6.7f, -2.57f / 3.05f);
		glVertex2f(-5.195f / 6.7f, -0.55f / 3.05f);
	glEnd();

	// Drawing game field lines
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);

		// Serving lines
		glVertex2f(2.0f / 6.7f, 1.0f);
		glVertex2f(2.0f / 6.7f, -1.0f);
		glVertex2f(-2.0f / 6.7f, 1.0f);
		glVertex2f(-2.0f / 6.7f, -1.0f);

		// Long service lines for doubles
		glVertex2f(5.92f / 6.7f, 1.0f);
		glVertex2f(5.92f / 6.7f, -1.0f);
		glVertex2f(-5.92f / 6.7f, 1.0f);
		glVertex2f(-5.92f / 6.7f, -1.0f);

		// Center lines
		glVertex2f(-1.0f, 0.0f);
		glVertex2f(-2.0f / 6.7f, 0.0f);
		glVertex2f(1.0f, 0.0f);
		glVertex2f(2.0f / 6.7f, 0.0f);

		// Side lines for singles
		glVertex2f(-1.0f, 2.57f / 3.05f);
		glVertex2f(1.0f, 2.57f / 3.05f);
		glVertex2f(-1.0f, -2.57f / 3.05f);
		glVertex2f(1.0f, -2.57f / 3.05f);

		glColor3f(1.0f, 0.0f, 0.0f);

		glLineWidth(2.0f);

		// Net
		glVertex2f(0.0f, -1.0f);
		glVertex2f(0.0f, 1.0f);
		
		glLineWidth(1.0f);

	glEnd();

	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
		// Game field border
		glVertex2f(-1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(1.0f, -1.0f);
		glVertex2f(-1.0f, -1.0f);
	glEnd();
	glLineWidth(1.0f);
	// Draw current position
	if (current_pos.valid) {
		glPointSize(10.0f);
		glBegin(GL_POINT);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(current_pos.x, current_pos.y);
		glEnd();
	}

	if (cursor_pos.valid) {
		glBegin(GL_LINE);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex2f(cursor_pos.x + 0.0002f * 61, cursor_pos.y);
			glVertex2f(cursor_pos.x - 0.0002f * 61, cursor_pos.y);
			glVertex2f(cursor_pos.x, cursor_pos.y + 0.0002f * 134);
			glVertex2f(cursor_pos.x, cursor_pos.y - 0.0002f * 134);
		glEnd();
	}
}

void CRoboconCtrlView::OnDraw(CDC* /*pDC*/)
{
	CRoboconCtrlDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLDrawScene();
	SwapBuffers(m_hDC);
	CPaintDC dc(this);
	CClientDC pDC(this);
	OnPrepareDC(&pDC);

	std::pair<CPoint, BOOL> b = GetRobotCoord();
	std::basic_ostringstream<TCHAR> oss;
	oss << _T("Mouse coordinates: (") << b.first.x << _T(", ") << b.first.y << _T(")");

	TextOut(pDC.GetSafeHdc(), 1, 2, oss.str().c_str(), oss.str().size());
}

void CRoboconCtrlView::OnMouseMove(UINT nFlags, CPoint point)
{
	cursor_pos = GetGLCoord(point);
	Invalidate();
}

void CRoboconCtrlView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	current_pos.valid = FALSE;
	Invalidate();
//	OnContextMenu(this, point);
}

void CRoboconCtrlView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif

	CRect rect;
	GetClientRect(&rect);
}

void CRoboconCtrlView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CView::OnLButtonDblClk(nFlags, point);
	current_pos = GetGLCoord(point);

	std::pair<CPoint, BOOL> b = GetRobotCoord();
	

	Invalidate();
}

LRESULT CRoboconCtrlView::refresh_coordinates(WPARAM w, LPARAM l) {
	std::vector<int> coordinates = *(reinterpret_cast<std::vector<int>*>(l));
	std::basic_ostringstream<TCHAR> oss;
	oss << _T("ROBOT RECEIVED COORD: (") << coordinates[0] << _T(", ") << coordinates[1] << _T(", ") << coordinates[2] << _T(")");
	OutputDebugString(oss.str().c_str());

	robot_x_pos = coordinates[0];
	robot_y_pos = coordinates[1];
	robot_angle = coordinates[2];

	return 0;
}

// CRoboconCtrlView diagnostics

#ifdef _DEBUG
void CRoboconCtrlView::AssertValid() const
{
	CView::AssertValid();
}

void CRoboconCtrlView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRoboconCtrlDoc* CRoboconCtrlView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRoboconCtrlDoc)));
	return (CRoboconCtrlDoc*)m_pDocument;
}
#endif //_DEBUG

// CRoboconCtrlView message handlers

void CRoboconCtrlView::OnSize(UINT nType, int cx, int cy)
{
	double ratio = 61.0 / 134.0;
	if ((cx * ratio) > (double)(cy)) {
		gl_height = cy * 90 / 100;
		gl_width = (int)round(gl_height / ratio);
	}
	else {
		gl_width = cx * 90 /100;
		gl_height = (int)round(gl_width * ratio);
	}
	glViewport((cx - gl_width) / 2, (cy - gl_height) / 2, gl_width, gl_height);
	CView::OnSize(nType, cx, cy);
}