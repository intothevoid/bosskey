
// BossKeeDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <map>

using namespace std;

// CBossKeeDlg dialog
class CBossKeeDlg : public CDialogEx
{
// Construction
public:
	CBossKeeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BOSSKEE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	//Main enumertation function
	static BOOL CALLBACK EnumWindowsCallback(HWND hwndWindow, LPARAM lpParam);

	//Hotkey handler
	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);

	//Custom User Message
	LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// Main listbox to populate the Window titles
	CListBox m_lstMain;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnClose();
	CButton m_cmdApply;
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton4();

private:
	//Refresh main list
	void RefreshList();
public:
	afx_msg void OnBnClickedButton2();
};
