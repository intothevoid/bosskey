
// BossKeeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BossKee.h"
#include "BossKeeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Constants
const int HOTKEY_ID = 100;

//Global mappings of window titles and handles
map<CString,HWND> g_mapMain;

//Global window name to set
CString g_csWindowTitle = _T("");

//Global hide/unhide flag
BOOL g_bHide = 0;

//User message
#define WM_MYMESSAGE (WM_USER + 1)

//User notification structure
NOTIFYICONDATA nid;

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


// CBossKeeDlg dialog




CBossKeeDlg::CBossKeeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBossKeeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBossKeeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstMain);
	DDX_Control(pDX, IDC_BUTTON3, m_cmdApply);
}

BEGIN_MESSAGE_MAP(CBossKeeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON3, &CBossKeeDlg::OnBnClickedButton3)

	//Hotkey Handler
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(WM_MYMESSAGE, OnMyMessage)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LIST1, &CBossKeeDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON4, &CBossKeeDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, &CBossKeeDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CBossKeeDlg message handlers

BOOL CBossKeeDlg::OnInitDialog()
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

	// TODO: Add extra initialization here

	//Clear main handle title map
	g_mapMain.clear();

	//Intial Apply button disabled
	m_cmdApply.EnableWindow(FALSE);

	//Setup our hotkey
	if(!RegisterHotKey(m_hWnd, HOTKEY_ID, MOD_CONTROL, VK_SPACE))
	{
		DWORD dwError = ::GetLastError();
		TRACE(_T("RegisterHotKeys() Failed Code: %ld"),dwError);
		MessageBox(_T("BossKee: Failed to register hotkey!"));
	}

	//Populate list
	RefreshList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBossKeeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBossKeeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBossKeeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CALLBACK CBossKeeDlg::EnumWindowsCallback(HWND hwndWindow, LPARAM lpParam)
{
	TCHAR lpszBuff[1024];
	memset(lpszBuff,0,1024);
	int nCnt  = 0;

	::GetWindowText(hwndWindow,lpszBuff,(int)(_tcslen(lpszBuff) - 1));

	CBossKeeDlg *myObj = (CBossKeeDlg *)lpParam;

	//Only add valid window titles
	if(0 != _tcscmp(lpszBuff,_T("")))
	{
		myObj->m_lstMain.AddString(lpszBuff);
		g_mapMain[lpszBuff] = hwndWindow;
		nCnt = g_mapMain.size();
	}

	//To continue enumeration, we return TRUE
	return TRUE;
}

void CBossKeeDlg::OnBnClickedButton3()
{
	//Get the current selected Window title from list
	//and save it to our global location
	int nSel = m_lstMain.GetCurSel();
	m_lstMain.GetText(nSel,g_csWindowTitle);

	//Disable command button
	m_cmdApply.EnableWindow(FALSE);
}

LRESULT CBossKeeDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	HWND hndWindow = NULL;
	hndWindow = g_mapMain[g_csWindowTitle];

	//Window is visible
	if(0 == g_bHide)
	{
		::ShowWindow(hndWindow,SW_HIDE);
		
		//Window is now hidden
		g_bHide = 1;
	}
	else
	{
		//Window is not visible
		::ShowWindow(hndWindow,SW_SHOWNORMAL);
		
		//Window is now visible
		g_bHide = 0;
	}

	return TRUE;
}


void CBossKeeDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	
	//Unregister hotkey
	if(!UnregisterHotKey(m_hWnd, HOTKEY_ID))
	{
		DWORD dwError = ::GetLastError();
		TRACE(_T("UnRegisterHotKeys() Failed Code: %ld"),dwError);
		MessageBox(_T("BossKee: Failed to unregister hotkey!"));
	}

	CDialogEx::OnClose();
}


void CBossKeeDlg::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here

	//Enable apply button
	m_cmdApply.EnableWindow();
}


void CBossKeeDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here

	//Refresh the list
	RefreshList();
}


void CBossKeeDlg::RefreshList()
{
	//Clear list
	m_lstMain.ResetContent();

	//Clear our map
	g_mapMain.clear();

	//Start the enumeration
	BOOL bRetVal = FALSE;
	bRetVal = EnumWindows(EnumWindowsCallback,(LPARAM)this);
	if(FALSE == bRetVal)
	{
		DWORD dwError = ::GetLastError();
		TRACE(_T("Enumwindows Failed Code: %ld"),dwError);
		MessageBox(_T("BossKee: Window Enumeration failed!"));
	}
}

void CBossKeeDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	
	//Minimize to System Tray
	//Show the notification icon
	ZeroMemory(&nid,sizeof(nid));
	nid.cbSize				=	sizeof(NOTIFYICONDATA);
	nid.hWnd				=	m_hWnd;
	nid.uID					=	0;
	nid.uFlags				=	NIF_ICON | NIF_MESSAGE | NIF_TIP;

	nid.uCallbackMessage	                =	WM_USER+1;
	nid.hIcon				=	LoadIcon(AfxGetInstanceHandle() ,MAKEINTRESOURCE(IDI_ICON2));
	_tcscpy(nid.szTip, _T("BossKey - I have your back!\n\nClick me to restore."));

	Shell_NotifyIcon(NIM_ADD,&nid);
	
	//Hide me
	ShowWindow(SW_HIDE);
}

LRESULT CBossKeeDlg::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
		case WM_LBUTTONUP:
			ShowWindow(SW_SHOWNORMAL);
			Shell_NotifyIcon(NIM_DELETE,&nid);
			return true;
	}

	return TRUE;
}