// DlgTrack.cpp : implementation file
//

#include "stdafx.h"
#include "RadSim.h"
#include "RadSimDlg.h"
#include "DlgTrack.h"
#include "DlgTrackInfo.h"
#include "afxdialogex.h"
#include <math.h>


// CDlgTrack dialog

IMPLEMENT_DYNAMIC(CDlgTrack, CDialogEx)

CDlgTrack::CDlgTrack(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRACK_DIALOG, pParent)
{

}

CDlgTrack::~CDlgTrack()
{
}

void CDlgTrack::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TRACK, m_ListTrack);
}


BEGIN_MESSAGE_MAP(CDlgTrack, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_EDIT, &CDlgTrack::OnBnClickedBtnEdit)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CDlgTrack::OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_AUTO, &CDlgTrack::OnBnClickedBtnAuto)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgTrack message handlers
BOOL CDlgTrack::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 1. Cài đặt style cho bảng: Có lưới (Gridlines) và chọn cả dòng (FullRowSelect)
	m_ListTrack.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 2. Tạo các cột
	// Cú pháp: InsertColumn(Vị trí, Tên cột, Căn lề, Độ rộng cột)
	m_ListTrack.InsertColumn(0, _T("TN"), LVCFMT_CENTER, 25);
	m_ListTrack.InsertColumn(1, _T("Vĩ độ - Kinh độ"), LVCFMT_CENTER, 120);
	m_ListTrack.InsertColumn(2, _T("Hướng - Tốc độ"), LVCFMT_CENTER, 85);
	m_ListTrack.InsertColumn(3, _T("Độ cao"), LVCFMT_CENTER, 60);
	m_ListTrack.InsertColumn(4, _T("Loại"), LVCFMT_CENTER, 40);
	m_ListTrack.InsertColumn(5, _T("Iden"), LVCFMT_CENTER, 50);
	m_ListTrack.InsertColumn(6, _T("TQ"), LVCFMT_CENTER, 40);
	m_ListTrack.InsertColumn(7, _T("Trạng thái"), LVCFMT_CENTER, 50);

	CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
	if (pMainDlg != NULL)
	{
		for (size_t i = 0; i < pMainDlg->m_listRealTracks.size(); i++)
		{
			// Lấy quỹ đạo thứ i
			AsterixTrack track = pMainDlg->m_listRealTracks[i];

			// Format dữ liệu thành chuỗi
			CString strTN, strPos, strHdgSpd, strAlti, strType, strIden, strStatus, strTQ;
			strTN.Format(_T("%02d"), track.nTrackNumber);
			strPos.Format(_T("%.4f - %.4f"), track.fLat, track.fLon);
			strHdgSpd.Format(_T("%.0f - %.0f"), track.fHeading, track.fSpeed);
			strAlti.Format(_T("%.0f"), track.fAltitude);
			strType.Format(_T("%d"), track.nType);
			strIden = CA2T(track.szIden); 
			strTQ.Format(_T("%d"), track.nQuality);
			strStatus = _T("New");

			// Thêm vào bảng (List Control)
			int nItem = m_ListTrack.InsertItem(i, strTN);
			m_ListTrack.SetItemText(nItem, 1, strPos);
			m_ListTrack.SetItemText(nItem, 2, strHdgSpd);
			m_ListTrack.SetItemText(nItem, 3, strAlti);
			m_ListTrack.SetItemText(nItem, 4, strType);
			m_ListTrack.SetItemText(nItem, 5, strIden);
			m_ListTrack.SetItemText(nItem, 6, strTQ);
			m_ListTrack.SetItemText(nItem, 7, strStatus);
		}
	}

	return TRUE;
}

void CDlgTrack::OnBnClickedBtnEdit()
{
	// TODO: Add your control notification handler code here	
	CDlgTrackInfo dlg;
	if (dlg.DoModal() == IDOK)
	{
		CString strTN = dlg.m_strTN;
		CString strLat = dlg.m_strLat;
		CString strLon = dlg.m_strLon;
		CString strHeading = dlg.m_strHeading;
		CString strSpeed = dlg.m_strSpeed;
		CString strAlti = dlg.m_strAlti;
		CString strIden = dlg.m_strIden;

		int nIndex = m_ListTrack.GetItemCount();
		int nItem = m_ListTrack.InsertItem(nIndex, strTN);

		m_ListTrack.SetItemText(nItem, 1, strLat + _T(" - ") + strLon);
		m_ListTrack.SetItemText(nItem, 2, strHeading + _T(" - ") + strSpeed); // Đã ghép thêm Speed cho đúng cột
		m_ListTrack.SetItemText(nItem, 3, strAlti);
		m_ListTrack.SetItemText(nItem, 4, _T("1")); // Cột Loại
		m_ListTrack.SetItemText(nItem, 5, strIden);
		m_ListTrack.SetItemText(nItem, 7, _T("New")); // Cột Trạng thái

		CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
		if (pMainDlg != NULL)
		{
			AsterixTrack newTrack;
			newTrack.nTrackNumber = _ttoi(strTN);
			newTrack.fLat = (float)_ttof(strLat);
			newTrack.fLon = (float)_ttof(strLon);
			newTrack.fHeading = (float)_ttof(strHeading);
			newTrack.fSpeed = (float)_ttof(strSpeed);
			newTrack.fAltitude = (float)_ttof(strAlti);
			newTrack.nType = 1;

			// Chuyển từ CString sang mảng char[] cho biến Iden
			strcpy_s(newTrack.szIden, sizeof(newTrack.szIden), CT2A(strIden));

			newTrack.cStatus = 'N'; // New

			pMainDlg->m_listRealTracks.push_back(newTrack);
		} 
	}
}


void CDlgTrack::OnBnClickedBtnDelete()
{
	// TODO: Add your control notification handler code here
	// 1. Lấy vị trí dòng đang được chọn trên List Control
	int nIndex = m_ListTrack.GetSelectionMark();

	// Kiểm tra nếu người dùng chưa chọn dòng nào
	if (nIndex == -1)
	{
		MessageBox(_T("Vui lòng chọn một quỹ đạo trong danh sách để xóa!"), _T("Thông báo"), MB_ICONWARNING);
		return;
	}

	// 2. Xác nhận lại việc xóa (để tránh bấm nhầm)
	if (IDYES != MessageBox(_T("Bạn có chắc chắn muốn xóa quỹ đạo này không?"), _T("Xác nhận"), MB_YESNO | MB_ICONQUESTION))
	{
		return;
	}

	// 3. Lấy số hiệu quỹ đạo (TN) từ cột đầu tiên để tìm trong dữ liệu gốc
	CString strTN = m_ListTrack.GetItemText(nIndex, 0);
	int nTN = _ttoi(strTN); // Chuyển từ chuỗi sang số nguyên

	// 4. Xóa dữ liệu trong mảng thực tế của Main Dialog
	CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
	if (pMainDlg != NULL)
	{
		for (auto it = pMainDlg->m_listRealTracks.begin(); it != pMainDlg->m_listRealTracks.end(); ++it)
		{
			if (it->nTrackNumber == nTN)
			{
				// (Tùy chọn chuyên nghiệp) Gửi một bản tin cuối cùng báo trạng thái 'D' (Delete)
				// để phía Trung tâm xóa mục tiêu này ngay lập tức thay vì đợi timeout
				it->cStatus = 'D';
				for (size_t i = 0; i < pMainDlg->m_listSessions.size(); i++)
				{
					if (pMainDlg->m_listSessions[i] != NULL)
						pMainDlg->m_listSessions[i]->SendTrackData(*it);
				}

				// Xóa khỏi vector dữ liệu
				pMainDlg->m_listRealTracks.erase(it);
				break;
			}
		}

		// Cập nhật lại số lượng mục tiêu hiển thị ở màn hình chính
		CString strInfo;
		strInfo.Format(_T("Số mục tiêu: %d | Số kết nối đang hoạt động: %d"),
			(int)pMainDlg->m_listRealTracks.size(), (int)pMainDlg->m_listSessions.size());
		pMainDlg->SetDlgItemText(IDC_STATIC_INFO2, strInfo);
	}

	// 5. Cuối cùng, xóa dòng hiển thị trên giao diện bảng
	m_ListTrack.DeleteItem(nIndex);
}



void CDlgTrack::OnBnClickedBtnAuto()
{
	// TODO: Add your control notification handler code here
	m_bIsAutoRunning = !m_bIsAutoRunning;

	if (m_bIsAutoRunning)
	{
		SetDlgItemText(IDC_BTN_AUTO, _T("Dừng chạy"));
		SetTimer(2, 1000, NULL); // Bật Timer số 2, quét mỗi 1 giây
	}
	else
	{
		SetDlgItemText(IDC_BTN_AUTO, _T("Bật Tự động chạy"));
		KillTimer(2); // Tắt Timer
	}
}

void CDlgTrack::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 2)
	{
		CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
		if (pMainDlg != NULL)
		{
			// Lặp qua tất cả mục tiêu thật đang có trong hệ thống
			for (size_t i = 0; i < pMainDlg->m_listRealTracks.size(); i++)
			{
				AsterixTrack& track = pMainDlg->m_listRealTracks[i];

				// --- THÊM NHIỄU NGẪU NHIÊN KHI ĐANG BAY ---

				// 30% cơ hội máy bay lượn vòng đổi hướng (Lệch -5 đến +5 độ)
				if (rand() % 10 < 3) {
					track.fHeading += ((rand() % 11) - 5.0f);
					if (track.fHeading >= 360.0f) track.fHeading -= 360.0f;
					if (track.fHeading < 0.0f) track.fHeading += 360.0f;
				}

				// 20% cơ hội tăng/giảm tốc độ (Thay đổi -5 đến +5 m/s)
				if (rand() % 10 < 2) {
					track.fSpeed += ((rand() % 11) - 5.0f);
					// Giới hạn tốc độ không để máy bay lùi hoặc bay quá nhanh
					if (track.fSpeed < 50.0f) track.fSpeed = 50.0f;
					if (track.fSpeed > 400.0f) track.fSpeed = 400.0f;
				}

				// 20% cơ hội thay đổi độ cao (Thay đổi -50 đến +50 mét)
				if (rand() % 10 < 2) {
					track.fAltitude += ((rand() % 101) - 50.0f);
					if (track.fAltitude < 0.0f) track.fAltitude = 0.0f;
				}

				// Đôi khi nhiễu sóng làm Chất lượng quỹ đạo (nQuality) tụt nhẹ
				if (rand() % 100 < 5) { // 5% cơ hội
					track.nQuality = (rand() % 11); // Random lại từ 0 - 10
				}

				// --- TÍNH TOÁN TỌA ĐỘ MỚI ---
				float rad = track.fHeading * (3.14159f / 180.0f);
				track.fLat += (track.fSpeed * cos(rad)) * 0.00005f;
				track.fLon += (track.fSpeed * sin(rad)) * 0.00005f;
				track.cStatus = 'U'; // Đánh dấu là Update

				// --- CẬP NHẬT LẠI GIAO DIỆN BẢNG ---
				CString strNewPos, strNewHdgSpd, strNewAlti, strNewTQ;
				strNewPos.Format(_T("%.4f - %.4f"), track.fLat, track.fLon);
				strNewHdgSpd.Format(_T("%.0f - %.0f"), track.fHeading, track.fSpeed);
				strNewAlti.Format(_T("%.0f"), track.fAltitude);
				strNewTQ.Format(_T("%d"), track.nQuality);

				for (int row = 0; row < m_ListTrack.GetItemCount(); row++)
				{
					if (_ttoi(m_ListTrack.GetItemText(row, 0)) == track.nTrackNumber)
					{
						m_ListTrack.SetItemText(row, 1, strNewPos);
						m_ListTrack.SetItemText(row, 2, strNewHdgSpd); // Update cả Hướng-Tốc độ
						m_ListTrack.SetItemText(row, 3, strNewAlti);   // Update cả Độ cao
						m_ListTrack.SetItemText(row, 6, strNewTQ);
						m_ListTrack.SetItemText(row, 7, _T("Upd"));
						break;
					}
				}
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}