// DlgSess.cpp : implementation file
//

#include "stdafx.h"
#include "RadSim.h"
#include "RadSimDlg.h"
#include "DlgSess.h"
#include "afxdialogex.h"


// CDlgSess dialog

IMPLEMENT_DYNAMIC(CDlgSess, CDialogEx)

CDlgSess::CDlgSess(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SESS_DIALOG, pParent)
{

}

CDlgSess::~CDlgSess()
{
}

void CDlgSess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SESS, m_listSess);
}


BEGIN_MESSAGE_MAP(CDlgSess, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_KICK, &CDlgSess::OnBnClickedBtnKick)
END_MESSAGE_MAP()


// CDlgSess message handlers
BOOL CDlgSess::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_listSess.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// 2. Tạo các cột (Cột 0 -> Cột 5)
	m_listSess.InsertColumn(0, _T("UserID"), LVCFMT_CENTER, 50);
	m_listSess.InsertColumn(1, _T("IP Address"), LVCFMT_CENTER, 100);
	m_listSess.InsertColumn(2, _T("Port"), LVCFMT_CENTER, 50);
	m_listSess.InsertColumn(3, _T("Init Time"), LVCFMT_CENTER, 80);
	m_listSess.InsertColumn(4, _T("Phiên"), LVCFMT_CENTER, 70);
	m_listSess.InsertColumn(5, _T("Trạng thái"), LVCFMT_CENTER, 90);

	// 3. Đổ dữ liệu mẫu
	// --- Dòng 1: Trung tâm 1 ---
	int nItem1 = m_listSess.InsertItem(0, _T("TT_01"));
	m_listSess.SetItemText(nItem1, 1, _T("192.168.1.10"));
	m_listSess.SetItemText(nItem1, 2, _T("10001"));
	m_listSess.SetItemText(nItem1, 3, _T("08:00:15"));
	m_listSess.SetItemText(nItem1, 4, _T("10001"));	
	m_listSess.SetItemText(nItem1, 5, _T("Đang truyền"));

	// --- Dòng 2: Trung tâm 2 ---
	int nItem2 = m_listSess.InsertItem(1, _T("TT_02"));
	m_listSess.SetItemText(nItem2, 1, _T("192.168.1.15"));
	m_listSess.SetItemText(nItem2, 2, _T("10002"));
	m_listSess.SetItemText(nItem2, 3, _T("08:05:22"));
	m_listSess.SetItemText(nItem2, 4, _T("10002"));
	m_listSess.SetItemText(nItem2, 5, _T("Đang chờ"));

	return TRUE;
}


void CDlgSess::OnBnClickedBtnKick()
{
	// TODO: Add your control notification handler code here
	// 1. Lấy vị trí dòng đang được chọn trên List Control
	int nIndex = m_listSess.GetSelectionMark();

	// Nếu chưa chọn dòng nào
	if (nIndex == -1)
	{
		MessageBox(_T("Vui lòng chọn một phiên kết nối trong danh sách để ngắt!"), _T("Thông báo"), MB_ICONWARNING);
		return;
	}

	// 2. Lấy thông tin IP và Cổng Phiên (Cột số 4) để hiển thị thông báo
	CString strIP = m_listSess.GetItemText(nIndex, 1);
	CString strSessionPort = m_listSess.GetItemText(nIndex, 4);

	CString strConfirm;
	strConfirm.Format(_T("Bạn có chắc chắn muốn ngắt kết nối Trung tâm [%s] (Phiên: %s) không?"), strIP, strSessionPort);

	if (IDYES != MessageBox(strConfirm, _T("Xác nhận Ngắt kết nối"), MB_YESNO | MB_ICONQUESTION))
	{
		return;
	}

	// Đổi cổng từ chuỗi sang số nguyên để so sánh
	UINT nTargetPort = _ttoi(strSessionPort);

	// 3. Xóa Session trong dữ liệu mạng thực tế của Main Dialog
	CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
	if (pMainDlg != NULL)
	{
		for (auto it = pMainDlg->m_listSessions.begin(); it != pMainDlg->m_listSessions.end(); ++it)
		{
			// Tìm đúng Session có cổng (m_nMyPort) khớp với dòng đang chọn
			if ((*it) != NULL && (*it)->m_nMyPort == nTargetPort)
			{
				// Ghi log ra màn hình Giám sát (Hex Monitor)
				CString strLog;
				strLog.Format(_T("[SYSTEM] Đã ngắt kết nối (KICK) Trung tâm %s"), strIP);
				pMainDlg->AddToMonitor(strLog);

				// Đóng cổng UDP và xóa khỏi bộ nhớ
				(*it)->Close();
				delete (*it);
				pMainDlg->m_listSessions.erase(it);

				break; // Đã tìm thấy và xóa xong, thoát vòng lặp
			}
		}

		// Cập nhật lại số lượng kết nối đang hoạt động ở màn hình chính
		CString strInfo;
		strInfo.Format(_T("Số mục tiêu: %d | Số kết nối đang hoạt động: %d"),
			(int)pMainDlg->m_listRealTracks.size(), (int)pMainDlg->m_listSessions.size());
		pMainDlg->SetDlgItemText(IDC_STATIC_INFO2, strInfo);
	}

	// 4. Xóa dòng đó khỏi bảng hiển thị
	m_listSess.DeleteItem(nIndex);
}

void CDlgSess::UpdateSessList()
{
	CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
	if (pMainDlg == NULL) return;

	m_listSess.DeleteAllItems();

	// Duyệt qua tất cả các phiên hiện có
	for (size_t i = 0; i < pMainDlg->m_listSessions.size(); i++)
	{
		C2_Session* pSess = pMainDlg->m_listSessions[i];
		CString strIndex, strIP, strCenterPort, strMyPort;

		strIndex.Format(_T("%d"), i + 1);
		strIP = pSess->m_strCenterIP;
		strCenterPort.Format(_T("%d"), pSess->m_nCenterPort);
		strMyPort.Format(_T("%d"), pSess->m_nMyPort);

		int nItem = m_listSess.InsertItem((int)i, strIndex);
		m_listSess.SetItemText(nItem, 1, strIP);          // Cột IP C2Center
		m_listSess.SetItemText(nItem, 2, strMyPort);      // Cột Cổng RadSim Mở (10001..)
		m_listSess.SetItemText(nItem, 3, strCenterPort);  // Cột Cổng C2Center Gửi
		m_listSess.SetItemText(nItem, 4, _T("Đang phát"));// Trạng thái
	}
}