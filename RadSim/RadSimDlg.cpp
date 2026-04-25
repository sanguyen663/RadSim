
// RadSimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RadSim.h"
#include "RadSimDlg.h"
#include "afxdialogex.h"
#include "DlgMonitor.h"
#include "DlgSess.h"
#include "DlgTrack.h"
#include "C_Listen.h"
#include "C2_Session.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRadSimDlg dialog



CRadSimDlg::CRadSimDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RADSIM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRadSimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRadSimDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_TRACK, &CRadSimDlg::OnBnClickedBtnTrack)
	ON_BN_CLICKED(IDC_BTN_SESS, &CRadSimDlg::OnBnClickedBtnSess)
	ON_BN_CLICKED(IDC_BTN_MONITOR, &CRadSimDlg::OnBnClickedBtnMonitor)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRadSimDlg message handlers

BOOL CRadSimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString strInfo;
	strInfo.Format(_T("Số mục tiêu: %d | Số kết nối đang hoạt động: %d"), 15, 2);
	SetDlgItemText(IDC_STATIC_INFO2, strInfo);

	// Khởi động cổng lắng nghe 10000 (UDP)
	if (m_ListenSocket.Create(10000, SOCK_DGRAM))
	{
		// Cập nhật lên giao diện (Nếu bạn có một Static Text ID là IDC_STATIC_STATUS)
		// SetDlgItemText(IDC_STATIC_STATUS, _T("Trạng thái: [ Đang lắng nghe cổng UDP 10000 ]"));
	}
	else
	{
		// Báo lỗi nếu cổng 10000 bị phần mềm khác chiếm dụng
		MessageBox(_T("Không thể mở cổng 10000. Cổng đang được sử dụng!"), _T("Lỗi mạng"), MB_ICONERROR);
	}

	GenerateMockData();

	// Khởi tạo Timer số 1, chạy mỗi 1000 mili-giây (1 giây)
	SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRadSimDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRadSimDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRadSimDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRadSimDlg::OnBnClickedBtnTrack()
{
	// TODO: Add your control notification handler code here
	if (m_dlgTrack.GetSafeHwnd() == NULL)
	{
		// Nếu chưa, thì khởi tạo nó thành dạng Modeless
		m_dlgTrack.Create(IDD_TRACK_DIALOG, this);
	}
	m_dlgTrack.ShowWindow(SW_SHOW);
	m_dlgTrack.SetActiveWindow();
}


void CRadSimDlg::OnBnClickedBtnSess()
{
	// TODO: Add your control notification handler code here
	if (m_dlgSess.GetSafeHwnd() == NULL)
	{
		m_dlgSess.Create(IDD_SESS_DIALOG, this);
	}
	m_dlgSess.ShowWindow(SW_SHOW);
	m_dlgSess.SetActiveWindow();
}


void CRadSimDlg::OnBnClickedBtnMonitor()
{
	// TODO: Add your control notification handler code here
	if (m_dlgMonitor.GetSafeHwnd() == NULL)
	{
		m_dlgMonitor.Create(IDD_MONITOR_DIALOG, this);
	}
	m_dlgMonitor.ShowWindow(SW_SHOW);
	m_dlgMonitor.SetActiveWindow();
}

void CRadSimDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// 1. Đóng cổng lắng nghe
	m_ListenSocket.Close();

	// 2. Đóng và xóa tất cả các phiên kết nối đang mở
	for (size_t i = 0; i < m_listSessions.size(); i++)
	{
		if (m_listSessions[i] != NULL)
		{
			m_listSessions[i]->Close();
			delete m_listSessions[i];
		}
	}
	m_listSessions.clear();
}

void CRadSimDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1) // Đúng Timer số 1 mà ta đã cài đặt
	{
		DWORD dwCurrentTime = GetTickCount();

		// Quét ngược danh sách để dọn dẹp các session bị Timeout (> 15 giây)
		for (int i = m_listSessions.size() - 1; i >= 0; i--)
		{
			if (m_listSessions[i] != NULL)
			{
				if (dwCurrentTime - m_listSessions[i]->m_dwLastHeartbeat > 15000)
				{
					// Quá 15 giây không thấy nhịp tim -> Xóa sổ
					CString strLog;
					strLog.Format(_T("[SYSTEM] Center %s TIMEOUT. Ngắt kết nối."), m_listSessions[i]->m_strCenterIP);
					AddToMonitor(strLog);

					m_listSessions[i]->Close();
					delete m_listSessions[i];
					m_listSessions.erase(m_listSessions.begin() + i);
				}
			}
		}
		// Nếu chưa có Trung tâm nào kết nối thì không làm gì cả
		if (m_listSessions.empty()) return;

		for (auto it = m_listRealTracks.begin(); it != m_listRealTracks.end(); )
		{
			bool bShouldSend = false;
			DWORD dwLastSent = m_mapLastSentTime[it->nTrackNumber];

			// Gửi ngay lập tức nếu là mục tiêu Mới (N) hoặc Bị xóa (D)
			if (it->cStatus == 'N' || it->cStatus == 'D') {
				bShouldSend = true;
			}
			// Hoặc nếu đã trôi qua >= 15 giây kể từ lần gửi cuối
			else if (dwCurrentTime - dwLastSent >= 15000) {
				bShouldSend = true;
			}

			if (bShouldSend)
			{
				for (size_t i = 0; i < m_listSessions.size(); i++)
				{
					if (m_listSessions[i] != NULL)
						m_listSessions[i]->SendTrackData(*it);
				}
				// Cập nhật lại thời gian gửi
				m_mapLastSentTime[it->nTrackNumber] = dwCurrentTime;

				// Chuyển trạng thái: Gửi 'N' xong thì thành 'U'
				if (it->cStatus == 'N') {
					it->cStatus = 'U';
					++it; // Đi tiếp tới track sau
				}
				// Gửi 'D' xong thì xóa hoàn toàn khỏi hệ thống
				else if (it->cStatus == 'D') {
					m_mapLastSentTime.erase(it->nTrackNumber);
					it = m_listRealTracks.erase(it); // Xóa và nhận iterator mới
				}
				else {
					++it;
				}
			}
			else {
				++it;
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CRadSimDlg::AddToMonitor(CString strLog)
{
	if (m_dlgMonitor.GetSafeHwnd() != NULL)
	{
		m_dlgMonitor.AddLog(strLog);
	}
}

void CRadSimDlg::GenerateMockData()
{
	m_listRealTracks.clear();
	m_mapLastSentTime.clear();
	srand((unsigned)time(NULL));
	const char* callsignPrefixes[] = { "VN", "VJ", "QH", "BL", "VU" };

	// Vòng lặp tạo 300 mục tiêu
	for (int i = 1; i <= 75; i++)
	{
		AsterixTrack track;
		memset(&track, 0, sizeof(AsterixTrack));
		track.nTrackNumber = i;

		// 1. Random Tọa độ bay quanh khu vực Hà Nội (Mở rộng phạm vi rộng hơn)
		// rand() % 2000 / 1000.0f - 1.0f tạo ra giá trị từ -1.0 đến +1.0 độ
		track.fLat = 21.0f + ((rand() % 2000) / 1000.0f - 1.0f);
		track.fLon = 105.8f + ((rand() % 2000) / 1000.0f - 1.0f);

		// 2. Random Vận tốc (100 - 300 m/s) và Hướng bay ban đầu (0 - 359 độ)
		track.fSpeed = 100.0f + (rand() % 201);
		track.fHeading = (float)(rand() % 360);

		// 3. Random Độ cao (1000 - 10000 mét)
		track.fAltitude = 1000.0f + (rand() % 9001);

		// 4. Random Kiểu loại (1 - 15 theo chuẩn Asterix I062/080)
		// VD: 3: Tiêm kích, 4: Trực thăng, 11: AWACS, 14: Dân sự, 15: UAV...
		track.nType = 1 + (rand() % 15);

		// 5. Random Ký hiệu (szIden): Bốc ngẫu nhiên 1 tiền tố hãng bay + 3 số ngẫu nhiên
		int prefixIdx = rand() % 5;
		int flightNum = 100 + (rand() % 900); // Từ 100 đến 999
		CStringA strIdenA; // Dùng CStringA để format chuỗi ASCII (an toàn cho Asterix)
		strIdenA.Format("%s%d", callsignPrefixes[prefixIdx], flightNum);
		strcpy_s(track.szIden, sizeof(track.szIden), strIdenA);

		// 6. Random Chất lượng quỹ đạo (0 - 10, ta random từ 5-10 cho thực tế)
		track.nQuality = 5 + (rand() % 6);

		track.cStatus = 'N'; // Trạng thái New

		// Đẩy vào mảng thực tế của hệ thống
		m_listRealTracks.push_back(track);\
		m_mapLastSentTime[i] = GetTickCount() - (rand() % 15000);
	}

	// Cập nhật lên giao diện
	CString strInfo;
	strInfo.Format(_T("Số mục tiêu: %d | Số kết nối đang hoạt động: %d"),
		(int)m_listRealTracks.size(), (int)m_listSessions.size());
	SetDlgItemText(IDC_STATIC_INFO2, strInfo);
}