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
	// TẠO GÓI TIN THEO CHUẨN ASTERIX CAT 062 (Dữ liệu Quỹ đạo Hệ thống)
	// Độ dài dự kiến: CAT(1) + LEN(2) + FSPEC(2) + Dữ liệu(15) = 20 bytes
	BYTE buffer[20];
	memset(buffer, 0, 20);

	// 1. Khối tiêu đề Khối dữ liệu
	buffer[0] = 62; // CAT = 062
	buffer[1] = 0;  // LEN (Byte cao)
	buffer[2] = 20; // LEN (Byte thấp) - Tổng 20 bytes

					// 2. FSPEC (Bản đồ trường dữ liệu)
					// Byte 1: Chọn FRN 1, 3, 4, 5 và có byte mở rộng -> 1011 1001 -> 0xB9
					// Byte 2: Chọn FRN 8 (Độ cao) -> 1000 0000 -> 0x80
	buffer[3] = 0xB9;
	buffer[4] = 0x80;

	int offset = 5;

	// 3. FRN 1: I062/010 Nguồn dữ liệu (2 bytes - SAC/SIC)
	buffer[offset++] = 0x01; // Giả sử SAC = 1
	buffer[offset++] = 0x01; // Giả sử SIC = 1 (Số hiệu PUR)

							 // 4. FRN 3: I062/070 Thời gian thông tin (3 bytes)
	DWORD dwTime = GetTickCount() / 1000; // Thay bằng thời gian thực của track
	buffer[offset++] = (dwTime >> 16) & 0xFF;
	buffer[offset++] = (dwTime >> 8) & 0xFF;
	buffer[offset++] = dwTime & 0xFF;

	// 5. FRN 4: I062/040 Số hiệu quỹ đạo (2 bytes)
	// (Lưu ý: Bạn cần thay ".nTrackID" bằng tên biến ID thật trong struct AsterixTrack của bạn)
	int trackID = 1; // trackData.nTrackID; 
	buffer[offset++] = (trackID >> 8) & 0xFF;
	buffer[offset++] = trackID & 0xFF;

	// 6. FRN 5: I062/105 Vị trí WGS-84 (6 bytes: 3 byte Vĩ độ, 3 byte Kinh độ)
	// (Ở đây điền mã mẫu, trong thực tế bạn sẽ áp dụng công thức 180/2^25 để đổi tọa độ sang int)
	buffer[offset++] = 0x00; // Vĩ độ 1
	buffer[offset++] = 0x00; // Vĩ độ 2
	buffer[offset++] = 0x00; // Vĩ độ 3
	buffer[offset++] = 0x00; // Kinh độ 1
	buffer[offset++] = 0x00; // Kinh độ 2
	buffer[offset++] = 0x00; // Kinh độ 3

							 // 7. FRN 8: I062/130 Độ cao tính toán (2 bytes)
	int altitude = 1000; // trackData.nAltitude; 
	buffer[offset++] = (altitude >> 8) & 0xFF;
	buffer[offset++] = altitude & 0xFF;

	// GỬI QUA MẠNG
	int nBytesSent = SendTo(buffer, 20, m_nCenterPort, m_strCenterIP);

	// In Log ra màn hình Monitor (Chuyển byte sang HEX)
	if (nBytesSent != SOCKET_ERROR)
	{
		CString strHex = _T("");
		CString strTemp;
		for (int i = 0; i < 20; i++)
		{
			strTemp.Format(_T("%02X "), buffer[i]);
			strHex += strTemp;
		}

		CString strLog;
		strLog.Format(_T("[TX to %s] %s"), m_strCenterIP, strHex);

		CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
		if (pMainDlg != NULL) pMainDlg->AddToMonitor(strLog);
	}// SendTo sẽ đẩy toàn bộ kích thước của struct qua mạng tới IP và Port của Trung tâm
	nBytesSent = SendTo(&trackData, sizeof(AsterixTrack), m_nCenterPort, m_strCenterIP);

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
		if (strMsg.Find(_T("[I_AM_ALIVE]")) != -1)
		{
			// Cập nhật lại thời gian sống
			m_dwLastHeartbeat = GetTickCount();
		}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}