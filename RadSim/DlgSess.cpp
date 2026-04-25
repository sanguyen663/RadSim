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
	ON_BN_CLICKED(IDC_BTN_SERVER, &CDlgSess::OnBnClickedBtnServer)
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

void CDlgSess::OnBnClickedBtnServer()
{
	// TODO: Add your control notification handler code here
	CString strIP, strPort;
	// Giả sử ID của 2 ô nhập liệu bạn đặt là IDC_EDIT_SERVER_IP và IDC_EDIT_SERVER_PORT
	GetDlgItemText(IDC_EDIT_IP, strIP);
	GetDlgItemText(IDC_EDIT_PORT, strPort);

	if (strPort.IsEmpty())
	{
		MessageBox(_T("Vui lòng nhập cổng (Port) để lắng nghe!"), _T("Lỗi"), MB_ICONWARNING);
		return;
	}

	UINT nPort = _ttoi(strPort);

	// 1. LẤY CON TRỎ TỚI MÀN HÌNH CHÍNH (RadSimDlg)
	CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
	if (pMainDlg == NULL) return;

	// 2. TỪ NAY TRỞ ĐI, PHẢI THÊM "pMainDlg->" TRƯỚC m_ListenSocket
	// Đóng socket nếu nó đang mở
	pMainDlg->m_ListenSocket.Close();

	BOOL bCreated = FALSE;

	// Tạo socket
	if (strIP.IsEmpty())
	{
		bCreated = pMainDlg->m_ListenSocket.Create(nPort, SOCK_DGRAM);
	}
	else
	{
		bCreated = pMainDlg->m_ListenSocket.Create(nPort, SOCK_DGRAM, FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE, strIP);
	}

	// Xử lý kết quả
	if (bCreated)
	{
		CString strMsg;
		if (strIP.IsEmpty()) strMsg.Format(_T("Đã mở Server lắng nghe trên mọi IP, Cổng: %d"), nPort);
		else strMsg.Format(_T("Đã mở Server lắng nghe trên IP: %s, Cổng: %d"), strIP, nPort);

		MessageBox(strMsg, _T("Thành công"), MB_OK | MB_ICONINFORMATION);

		// Ghi vào màn hình Monitor (cũng phải gọi qua pMainDlg)
		CString strLog = _T("[SYSTEM] ") + strMsg;
		pMainDlg->AddToMonitor(strLog);
	}
	else
	{
		MessageBox(_T("Không thể mở Server! IP/Port không hợp lệ hoặc đang bị phần mềm khác chiếm dụng."), _T("Lỗi mạng"), MB_ICONERROR);
	}
}
