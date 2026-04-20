// DlgMonitor.cpp : implementation file
//

#include "stdafx.h"
#include "RadSim.h"
#include "DlgMonitor.h"
#include "afxdialogex.h"


// CDlgMonitor dialog

IMPLEMENT_DYNAMIC(CDlgMonitor, CDialogEx)

CDlgMonitor::CDlgMonitor(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MONITOR_DIALOG, pParent)
{

}

CDlgMonitor::~CDlgMonitor()
{
}

void CDlgMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_listLog);
}


BEGIN_MESSAGE_MAP(CDlgMonitor, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CDlgMonitor::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CDlgMonitor::OnBnClickedBtnPause)
	ON_BN_CLICKED(IDC_BTN_START, &CDlgMonitor::OnBnClickedBtnStart)
END_MESSAGE_MAP()


// CDlgMonitor message handlers
void CDlgMonitor::AddLog(CString strLog)
{
	// 1. Lấy thời gian hiện tại
	CTime currentTime = CTime::GetCurrentTime();
	CString strTime = currentTime.Format(_T("[%H:%M:%S] "));

	// 2. Thiết lập độ dài tối đa cho mỗi dòng (có thể tùy chỉnh)
	int nMaxLength = 85;
	int nLength = strLog.GetLength();

	if (m_listLog.GetSafeHwnd() == NULL) return;

	// TRƯỜNG HỢP 1: Chuỗi ngắn -> In trên 1 dòng
	if (nLength + strTime.GetLength() <= nMaxLength)
	{
		m_listLog.AddString(strTime + strLog);
	}
	// TRƯỜNG HỢP 2: Chuỗi dài -> Cắt thành nhiều dòng
	else
	{
		int nOffset = 0;
		bool bFirstLine = true;

		while (nOffset < nLength)
		{
			if (bFirstLine)
			{
				// Dòng đầu tiên: Bị chiếm mất 11 ký tự bởi chuỗi thời gian
				int nChunkSize = nMaxLength - strTime.GetLength();
				CString strChunk = strLog.Mid(nOffset, nChunkSize);

				m_listLog.AddString(strTime + strChunk);
				nOffset += nChunkSize;
				bFirstLine = false;
			}
			else
			{
				// Các dòng tiếp theo: Bỏ thụt lề, lấy trọn vẹn chiều rộng màn hình (nMaxLength)
				CString strChunk = strLog.Mid(nOffset, nMaxLength);

				m_listLog.AddString(strChunk); // Ghi thẳng, không thêm khoảng trắng
				nOffset += nMaxLength;
			}
		}
	}

	// 3. Tự động cuộn xuống dòng mới nhất
	int nCount = m_listLog.GetCount();
	if (nCount > 0)
	{
		m_listLog.SetTopIndex(nCount - 1);
	}
}

void CDlgMonitor::OnBnClickedBtnClear()
{
	// TODO: Add your control notification handler code here
	m_listLog.ResetContent();
}


void CDlgMonitor::OnBnClickedBtnPause()
{
	// TODO: Add your control notification handler code here
	// 1. Đảo ngược trạng thái hiện tại (Đang chạy -> Dừng, Đang dừng -> Chạy)
	m_bIsPaused = !m_bIsPaused;

	// 2. Lấy con trỏ đến nút Pause trên giao diện
	CButton* pBtnPause = (CButton*)GetDlgItem(IDC_BTN_PAUSE);

	// 3. Đổi chữ trên nút để người dùng biết trạng thái tiếp theo
	if (m_bIsPaused)
	{
		pBtnPause->SetWindowText(_T("Tiếp tục (Resume)"));
	}
	else
	{
		pBtnPause->SetWindowText(_T("Tạm dừng (Pause)"));
	}
}


void CDlgMonitor::OnBnClickedBtnStart()
{
	// TODO: Add your control notification handler code here
	// 1. Ép trạng thái về chế độ ĐANG CHẠY
	m_bIsPaused = false;

	// 2. Đặt lại chữ cho nút Pause (phòng trường hợp người dùng đang bấm Pause trước đó)
	CButton* pBtnPause = (CButton*)GetDlgItem(IDC_BTN_PAUSE);
	if (pBtnPause != NULL)
	{
		pBtnPause->SetWindowText(_T("Tạm dừng (Pause)"));
	}

	// 3. (Tùy chọn) Xóa sạch log cũ để giám sát lại từ đầu
	m_listLog.ResetContent();

	// 4. In một dòng thông báo hệ thống cho đẹp mắt
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format(_T("[%H:%M:%S] "));
	m_listLog.AddString(strTime + _T("[SYSTEM] ===== BẮT ĐẦU GIÁM SÁT DỮ LIỆU ====="));
}
