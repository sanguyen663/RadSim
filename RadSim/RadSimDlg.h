
// RadSimDlg.h : header file
//

#pragma once
#include <vector>
#include <map>
#include "C_Listen.h"
#include "C2_Session.h"
#include "DlgTrack.h"
#include "DlgSess.h"
#include "DlgMonitor.h"


// CRadSimDlg dialog
class CRadSimDlg : public CDialogEx
{
// Construction
public:
	CRadSimDlg(CWnd* pParent = NULL);	
	CC_Listen m_ListenSocket;
	std::vector<C2_Session*> m_listSessions;
	int m_nSessionCounter = 1;
	afx_msg void OnDestroy();
	void AddToMonitor(CString strLog);
	std::vector<AsterixTrack> m_listRealTracks;
	std::map<int, DWORD> m_mapLastSentTime;
	void GenerateMockData();
	CDlgMonitor m_dlgMonitor;
	CDlgTrack   m_dlgTrack;
	CDlgSess    m_dlgSess;
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RADSIM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


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
	afx_msg void OnBnClickedBtnTrack();
	afx_msg void OnBnClickedBtnSess();
	afx_msg void OnBnClickedBtnMonitor();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
