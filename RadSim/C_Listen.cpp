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
				// --- THÊM HỘP THOẠI XÁC NHẬN Ở ĐÂY ---
				CString strPrompt;
				strPrompt.Format(_T("Có yêu cầu kết nối từ Trung tâm IP: %s, Cổng: %d.\nBạn có muốn chấp nhận không?"), strIP, nPort);

				int nResponse = MessageBox(pMainDlg->GetSafeHwnd(), strPrompt, _T("Xác nhận kết nối"), MB_YESNO | MB_ICONQUESTION);

				if (nResponse == IDYES)
				{
					// 3. Tính toán cổng mới (Ví dụ: 10000 + 1 = 10001)
					UINT newPort = 10000 + pMainDlg->m_nSessionCounter;

					// 4. Tạo phiên kết nối mới và lưu vào danh sách
					C2_Session* pNewSession = new C2_Session();
					if (pNewSession->InitSession(newPort, strIP, nPort))
					{
						// ... (phần code cấp cổng và gửi ACCEPT giữ nguyên như cũ) ...
						pMainDlg->m_listSessions.push_back(pNewSession);
						pMainDlg->m_nSessionCounter++;

						CString strReply;
						strReply.Format(_T("[ACCEPT:%d]"), newPort);
						char replyBuf[256];
						strcpy_s(replyBuf, CT2A(strReply));
						SendTo(replyBuf, strlen(replyBuf), nPort, strIP);

						CString strLog;
						strLog.Format(_T("[SYSTEM] ĐÃ CHẤP NHẬN kết nối từ %s:%d. Cấp cổng %d"), strIP, nPort, newPort);
						pMainDlg->AddToMonitor(strLog);

						if (pMainDlg->m_dlgSess.GetSafeHwnd() != NULL)
						{
							pMainDlg->m_dlgSess.UpdateSessList();
						}
					}
					else
					{
						delete pNewSession;
					}
				}
				else
				{
					// Nếu chọn NO (Từ chối)
					CString strLog;
					strLog.Format(_T("[SYSTEM] ĐÃ TỪ CHỐI kết nối từ %s:%d"), strIP, nPort);
					pMainDlg->AddToMonitor(strLog);
				}
			}
		}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}