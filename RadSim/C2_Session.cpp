#include "stdafx.h"
#include "C2_Session.h"
#include "RadSimDlg.h"

C2_Session::C2_Session()
{
}


C2_Session::~C2_Session()
{
}

BOOL C2_Session::InitSession(UINT myPort, CString centerIP, UINT centerPort)
{
	m_nMyPort = myPort;
	m_strCenterIP = centerIP;
	m_nCenterPort = centerPort;
	m_dwLastHeartbeat = GetTickCount();
	// Mở cổng UDP riêng cho phiên này
	return Create(m_nMyPort, SOCK_DGRAM);
}

void C2_Session::SendTrackData(AsterixTrack trackData)
{
	// SendTo sẽ đẩy toàn bộ kích thước của struct qua mạng tới IP và Port của Trung tâm
	int nBytesSent = SendTo(&trackData, sizeof(AsterixTrack), m_nCenterPort, m_strCenterIP);

	if (nBytesSent != SOCKET_ERROR)
	{
		BYTE* pData = (BYTE*)&trackData;
		CString strHex = _T("");
		CString strTemp;

		// Vòng lặp đọc từng byte trong struct và chuyển thành dạng Hex
		for (int i = 0; i < sizeof(AsterixTrack); i++)
		{
			// %02X nghĩa là in ra số Hex viết hoa, luôn có 2 chữ số (VD: 0A, F1)
			strTemp.Format(_T("%02X "), pData[i]);
			strHex += strTemp;
		}

		// Tạo câu thông báo hoàn chỉnh (Giống hệt bản vẽ của bạn)
		CString strLog;
		strLog.Format(_T("[TX to %s] %s"), m_strCenterIP, strHex);

		// Gọi cửa sổ chính để đẩy log sang Monitor
		CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
		if (pMainDlg != NULL)
		{
			pMainDlg->AddToMonitor(strLog);
		}
	}
}

void C2_Session::OnReceive(int nErrorCode)
{
	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	CString ip; UINT port;

	int nBytes = ReceiveFrom(buffer, 256, ip, port);
	if (nBytes > 0)
	{
		CString strMsg(buffer);
		if (strMsg == _T("[I_AM_ALIVE]"))
		{
			// Cập nhật lại thời gian sống
			m_dwLastHeartbeat = GetTickCount();
		}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}