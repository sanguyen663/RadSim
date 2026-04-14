// DlgTrackInfo.cpp : implementation file
//

#include "stdafx.h"
#include "RadSim.h"
#include "DlgTrackInfo.h"
#include "afxdialogex.h"


// CDlgTrackInfo dialog

IMPLEMENT_DYNAMIC(CDlgTrackInfo, CDialogEx)

CDlgTrackInfo::CDlgTrackInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EDIT_DIALOG, pParent)
	, m_strTN(_T(""))
	, m_strLat(_T(""))
	, m_strLon(_T(""))
	, m_strHeading(_T(""))
	, m_strSpeed(_T(""))
	, m_strAlti(_T(""))
	, m_strIden(_T(""))
{

}

CDlgTrackInfo::~CDlgTrackInfo()
{
}

void CDlgTrackInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TN, m_strTN);
	DDX_Text(pDX, IDC_EDIT_LAT, m_strLat);
	DDX_Text(pDX, IDC_EDIT_LON, m_strLon);
	DDX_Text(pDX, IDC_EDIT_HEADING, m_strHeading);
	DDX_Text(pDX, IDC_EDIT_SPEED, m_strSpeed);
	DDX_Text(pDX, IDC_EDIT_ALTI, m_strAlti);
	DDX_Text(pDX, IDC_EDIT_IDEN, m_strIden);
}


BEGIN_MESSAGE_MAP(CDlgTrackInfo, CDialogEx)
END_MESSAGE_MAP()


// CDlgTrackInfo message handlers
