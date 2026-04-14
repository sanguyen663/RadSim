#include "stdafx.h"
#include "C_Listen.h"
#include "RadSim.h"
#include "RadSimDlg.h"

CC_Listen::CC_Listen()
{
}


CC_Listen::~CC_Listen()
{
}

void CC_Listen::OnReceive(int nErrorCode)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer)); // Xóa trắng buffer cho an toàn
	CString strIP;
	UINT nPort;

	// 1. Đọc gói tin nhận được
	int nBytes = ReceiveFrom(buffer, 1024, strIP, nPort);

	if (nBytes > 0)
	{
		CString strMsg(buffer); // Chuyển từ char* sang CString để dễ so sánh

	// 2. Kiểm tra xem có đúng là gói tin xin kết nối không?
		if (strMsg == _T("[REQ_CONNECT]"))
		{
			// Lấy con trỏ của Cửa sổ chính để can thiệp vào dữ liệu của nó
			CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
			if (pMainDlg != NULL)
			{
				// 3. Tính toán cổng mới (Ví dụ: 10000 + 1 = 10001)
				UINT newPort = 10000 + pMainDlg->m_nSessionCounter;

				// 4. Tạo phiên kết nối mới và lưu vào danh sách
				C2_Session* pNewSession = new C2_Session();
				if (pNewSession->InitSession(newPort, strIP, nPort))
				{
					pMainDlg->m_listSessions.push_back(pNewSession);

					// Tăng biến đếm để Trung tâm sau vào sẽ lấy cổng 10002
					pMainDlg->m_nSessionCounter++;

					// 5. Gửi thông báo chấp nhận (ACCEPT) về cho Trung tâm
					CString strReply;
					strReply.Format(_T("[ACCEPT:%d]"), newPort);

					// Đổi từ CString sang char* để gửi qua mạng
					char replyBuf[256];
					strcpy_s(replyBuf, CT2A(strReply));
					SendTo(replyBuf, strlen(replyBuf), nPort, strIP);

					// --- THÊM MỚI ĐỂ HIỂN THỊ LOG LÊN RADSIM ---
					CString strLog;
					strLog.Format(_T("[SYSTEM] Có Trung tâm %s:%d xin kết nối. Đã cấp cổng %d"), strIP, nPort, newPort);
					pMainDlg->AddToMonitor(strLog);

					// Nếu bạn đã viết hàm UpdateSessList() trong DlgSess của RadSim thì gọi ở đây:
					if (pMainDlg->m_dlgSess.GetSafeHwnd() != NULL)
					{
						pMainDlg->m_dlgSess.UpdateSessList(); 
					}
				}
				else
				{
					delete pNewSession; // Mở cổng thất bại thì xóa đi
				}
			}
		}
	}

	CAsyncSocket::OnReceive(nErrorCode);
}