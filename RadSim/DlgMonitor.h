#pragma once
#include "afxwin.h"


// CDlgMonitor dialog

class CDlgMonitor : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMonitor)

public:
	CDlgMonitor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMonitor();
	void AddLog(CString strLog);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MONITOR_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool m_bIsPaused = false;
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listLog;
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnBnClickedBtnStart();
};
