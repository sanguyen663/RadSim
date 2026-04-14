#pragma once
#include "afxcmn.h"


// CDlgTrack dialog

class CDlgTrack : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTrack)

public:
	CDlgTrack(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTrack();
	virtual BOOL OnInitDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRACK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListTrack;
	afx_msg void OnBnClickedBtnEdit();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnAuto();
	bool m_bIsAutoRunning = false;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


