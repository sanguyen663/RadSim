#pragma once
#include "afxwin.h"


// CDlgTrackInfo dialog

class CDlgTrackInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTrackInfo)

public:
	CDlgTrackInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTrackInfo();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	CString m_strTN;
	CString m_strLat;
	CString m_strLon;
	CString m_strHeading;
	CString m_strSpeed;
	CString m_strAlti;
	CString m_strIden;
};
