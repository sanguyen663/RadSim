
// RadSimDlg.h : header file
//

#pragma once
#include <vector>
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
	CRadSimDlg(CWnd* pParent = NULL);	// standard constructor
	// 1. Biến quản lý cổng lắng nghe 10000
	CC_Listen m_ListenSocket;

	// 2. Mảng lưu trữ danh sách các phiên kết nối (Trung tâm)
	std::vector<C2_Session*> m_listSessions;

	// 3. Biến đếm để tạo cổng mới (Bắt đầu từ 1, cổng sẽ là 10001, 10002...)
	int m_nSessionCounter = 1;

	afx_msg void OnDestroy();

	// nhận log từ Socket
	void AddToMonitor(CString strLog);
	std::vector<AsterixTrack> m_listRealTracks;
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
