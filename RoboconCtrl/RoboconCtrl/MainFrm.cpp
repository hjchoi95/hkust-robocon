
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "RoboconCtrl.h"
#include "RobotMCtrl.h"

#include "MainFrm.h"
#include <sstream>
#include <locale>
#include <codecvt>
#include <string>
#include <utility>
#include <algorithm>
#include <math.h>
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CMainFrame::OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CMainFrame::OnUpdateFileNew)
	ON_COMMAND(ID_EDIT_COPY, &CMainFrame::OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, &CMainFrame::OnEditClear)
	ON_COMMAND(ID_EDIT_PASTE, &CMainFrame::OnEditPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CMainFrame::OnEditSelectAll)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OpenConnection)
	ON_COMMAND(ID_FILE_UPDATE, &CMainFrame::CloseConnection)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_MESSAGE(WM_SEND_STRING, &CMainFrame::WriteString)
	ON_WM_KEYDOWN()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() : send_msg(), keys_pressed()
{
	// TODO: add member initialization code here
}

SerialIO* CMainFrame::serial = NULL;

CMainFrame::~CMainFrame()
{
	if (serial) {
		delete serial;
		serial = NULL;

		HANDLE handles[2] = { 0 };
		if (threads[0] != NULL) {
			handles[0] = threads[0]->m_hThread;
		}
		if (threads[1] != NULL) {
			handles[1] = threads[1]->m_hThread;
		}
		WaitForMultipleObjects(2, handles, TRUE, 2000);
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);
	m_wndInput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndInput);


	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_UPDATE);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	// Create input window
	CString strInputWnd;
	bNameValid = strInputWnd.LoadString(IDS_INPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndInput.Create(strInputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_INPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	HICON hInputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_INPUT_WND_HC : IDI_INPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndInput.SetIcon(hInputBarIcon, FALSE);
}

void CMainFrame::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
void CMainFrame::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

// Settings Getters (I have no idea how to make this better)

std::vector<std::basic_string<TCHAR>> CMainFrame::GetSettings()
{
	return m_wndProperties.GetSettings();
}

// Passing print functions to output window

void CMainFrame::print_to_output(std::basic_string<TCHAR> string_to_print)
{
	m_wndOutput.PrintString(string_to_print);
}
void CMainFrame::print_from_serial(std::basic_string<TCHAR> string_to_print, int ioconfig)
{
	switch (ioconfig) {
	case 1:
		m_wndOutput.ReadFromSerial(_T("Write: ") + string_to_print); break;
	case 0:
	default:
		m_wndOutput.ReadFromSerial(_T("Read: ") + string_to_print); break;
	}
	
}

void CMainFrame::print_from_serial(std::string string_to_print, int ioconfig)
{
	if (readmode == 1) {
		std::pair<std::vector<int>, BOOL> data = RobotMCtrl()(string_to_print);
		if (data.second) {
			GetActiveView()->SendMessage(WM_SEND_STRING, 0, (LPARAM)&(data.first));
			std::basic_ostringstream<TCHAR> oss;
			oss << _T("Received Coordinate: (") << (data.first)[0] << _T(", ") << (data.first)[1] << _T(", ") << (data.first)[2] << _T(")");
			print_from_serial(oss.str().c_str(), ioconfig);
		}

	}
	else {
		print_from_serial(std::wstring(CString(string_to_print.c_str(), string_to_print.size())), ioconfig);
	}
}

// CMainFrame threads

// Read thread

UINT __cdecl CMainFrame::read_thread(LPVOID app_ptr){
	while (serial != NULL && serial->is_connected()){
		if (serial->bytes_to_read()) {
			((CMainFrame*)app_ptr)->print_from_serial(serial->read());
		}
		Sleep(10);
	}
	return 0;
}

// Write thread

UINT __cdecl CMainFrame::write_thread(LPVOID app_ptr){
	while (serial != NULL && serial->is_connected()){
		if (!((CMainFrame*)app_ptr)->keys_pressed.empty()){
			((CMainFrame*)app_ptr)->SendMessage(WM_SEND_STRING, 0, 0);
			Sleep(10);
		}
	}
	return 0;
}

// CMainFrame message handlers

BOOL CMainFrame::PreTranslateMessage(MSG* msg)
{
	std::vector<std::basic_string<TCHAR>> settings = m_wndProperties.GetSettings();
	readmode = stoi(settings[5]);
	if (stoi(settings[3]) == 2 && GetFocus() != NULL){
		keys_pressed.clear();
		if (GetAsyncKeyState(0x51)) { // Q Key
			keys_pressed += std::wstring(_T("q"));
		}
		if (GetAsyncKeyState(0x57)) { // W Key
			keys_pressed += std::wstring(_T("w"));
		}
		if (GetAsyncKeyState(0x45)) { // E Key
			keys_pressed += std::wstring(_T("e"));
		}
		if (GetAsyncKeyState(0x41)) { // A Key
			keys_pressed += std::wstring(_T("a"));
		}
		if (GetAsyncKeyState(0x53)) { // S Key
			keys_pressed += std::wstring(_T("s"));
		}
		if (GetAsyncKeyState(0x44)) { // D Key
			keys_pressed += std::wstring(_T("d"));
		}
		if (GetAsyncKeyState(VK_SHIFT)) {
			shift = TRUE;
		}
		else {
			shift = FALSE;
		}
		for (int i = 0; i < 10; i++) {
			if (GetAsyncKeyState(0x30 + i)) {
				TCHAR c[1] = { _T('0') + i };
				keys_pressed += c;
			}
		}
	}
	else {
		keys_pressed.clear();
	}
	return FALSE;
}

void CMainFrame::OpenConnection()
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	if (serial != NULL){
		CloseConnection();
	}
	std::vector<std::basic_string<TCHAR>> settings = m_wndProperties.GetSettings();
	print_to_output(_T("Opening Port: ") + settings[0] + _T(" | Baud rate: ") + settings[1] + _T(" bit/s | Read buffer size: ") + settings[2] + _T(" bytes"));
	try {
		serial = new SerialIO(settings[0], stoi(settings[1]), stoi(settings[2]));

		if (serial->is_connected()) {
			print_to_output(_T("SUCCESS!"));
		}
		threads[0] = AfxBeginThread(CMainFrame::read_thread, this);
		threads[1] = AfxBeginThread(CMainFrame::write_thread, this);
	}
	catch (std::runtime_error r) {
		print_to_output(converter.from_bytes(r.what()));
	}
}

void CMainFrame::CloseConnection()
{
	if (serial) {
		delete serial;
		serial = NULL;
		
		HANDLE handles[2] = { 0 };
		if (threads[0] != NULL) {
			handles[0] = threads[0]->m_hThread;
		}
		if (threads[1] != NULL) {
			handles[1] = threads[1]->m_hThread;
		}
		WaitForMultipleObjects(2, handles, TRUE, 2000);
		threads[0] = NULL;
		threads[1] = NULL;
		print_to_output(_T("Closing Port"));
	}

}

void CMainFrame::OnEditCopy()
{
	CWnd* pCVw = GetFocus();
	if (pCVw != NULL && pCVw != this) {
		if (!(pCVw->PostMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_COPY, 0), 0))) {
			OutputDebugString(_T("ERROR: cannot post copy message!"));
		}
	}
}

void CMainFrame::OnEditClear()
{
	if (!m_wndInput.m_wndInputBox.PostMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_CLEAR, 0), 0)) {
		OutputDebugString(_T("ERROR: cannot post clear message to input window!"));
	}
	if (!m_wndOutput.m_wndOutputBuild.PostMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_CLEAR, 0), 0)) {
		OutputDebugString(_T("ERROR: cannot post clear message to output window!"));
	}
	if (!m_wndOutput.m_wndOutputRead.PostMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_CLEAR, 0), 0)) {
		OutputDebugString(_T("ERROR: cannot post clear message to output window!"));
	}
}

void CMainFrame::OnEditPaste()
{
	CWnd* pCVw = GetFocus();
	if (pCVw != NULL && pCVw != this) {
		if (!(pCVw->PostMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_PASTE, 0), 0))) {
			OutputDebugString(_T("ERROR: cannot post paste message!"));
		}
	}
}

void CMainFrame::OnEditSelectAll()
{
	CWnd* pCVw = GetFocus();
	if (pCVw != NULL && pCVw != this) {
		if (!(pCVw->PostMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_SELECT_ALL, 0), 0))) {
			OutputDebugString(_T("ERROR: cannot post select all message!"));
		}
	}
}

LRESULT CMainFrame::WriteString(WPARAM w, LPARAM l)
{
	if (serial == NULL) {
		OpenConnection();
	}
	if (serial != NULL && serial->is_connected()){
		switch (w) {
		case 0: // manual mode printing
		{
			if (keys_pressed.find_first_of(_T("qweasd")) != std::basic_string<TCHAR>::npos) {
				int x = 0;
				int y = 0;
				int w = 0;
				if (keys_pressed.find(_T('w')) != std::basic_string<TCHAR>::npos) {
					y += 100;
				}
				if (keys_pressed.find(_T('s')) != std::basic_string<TCHAR>::npos) {
					y -= 100;
				}
				if (keys_pressed.find(_T('d')) != std::basic_string<TCHAR>::npos) {
					x += 100;
				}
				if (keys_pressed.find(_T('a')) != std::basic_string<TCHAR>::npos) {
					x -= 100;
				}
				if (keys_pressed.find(_T('q')) != std::basic_string<TCHAR>::npos) {
					w -= 100;
				}
				if (keys_pressed.find(_T('e')) != std::basic_string<TCHAR>::npos) {
					w += 100;
				}
				if (shift) {
					x = x / 2;
					y = y / 2;
					w = w / 2;
				}
				serial->write(RobotMCtrl()(x, y, w));
				if (x != 0 && y != 0) {
					double scale = sqrt(100 * 100 + 100 * 100) / 100.0;
					x = (int)((double)x / scale);
					y = (int)((double)y / scale);
				}
				std::basic_ostringstream<TCHAR> oss;
				oss << _T("X: ") << x << _T(" Y: ") << y << _T(" w: ") << w;
				print_from_serial(oss.str(), 1);
			}
			// speed printing
			std::size_t found = keys_pressed.find_last_of(_T("0123456789"));
			if (found != std::basic_string<TCHAR>::npos) {
				serial->write(RobotMCtrl()(keys_pressed[found] - '0'));
				std::basic_ostringstream<TCHAR> oss;
				oss << _T("Speed: ") << keys_pressed[found];
				print_from_serial(oss.str(), 1);
			}
			break;
		}
		case 1: // string printing
		{
			CString* string = reinterpret_cast<CString*>(l);
			serial->write(std::string(CT2CA(*string)));
			print_from_serial(std::basic_string<TCHAR>(*string), 1);
			break;
		}
		case 2: // auto mode printing
		{
			std::vector<short> data = *reinterpret_cast<std::vector<short>*>(l);
			serial->write(RobotMCtrl()(data[0], data[1], (unsigned short)data[2]));
			std::basic_ostringstream<TCHAR> oss;
			oss << _T("Sent Position: (") << (short)data[0] << _T(", ") << (short)data[1] << _T(", ") << (short)data[2] << _T(")");
			print_from_serial(oss.str(), 1);
			break;
		}
		default:
			break;
		}
	}
	else {
		OutputDebugString(_T("Port cannot be opened, cannot write"));
		return 1;
	}
	return 0;
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}
