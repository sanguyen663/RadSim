#pragma once
#include "afxcmn.h"


// CDlgSess dialog

class CDlgSess : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSess)

public:
	CDlgSess(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSess();
	virtual BOOL OnInitDialog();
	void UpdateSessList();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SESS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listSess;
	afx_msg void OnBnClickedBtnKick();
	afx_msg void OnBnClickedBtnServer();
};
