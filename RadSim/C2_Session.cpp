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
	// TẠO GÓI TIN THEO CHUẨN ASTERIX CAT 062 VỚI 8 TRƯỜNG
	BYTE buffer[31]; // Tăng lên 31 bytes vì I062/080 giờ chiếm 2 bytes
	memset(buffer, 0, 31);

	// 1. Khối Header của ASTERIX
	buffer[0] = 62; // CAT = 062
	buffer[1] = 0;
	buffer[2] = 31; // LEN: 31 bytes

					// 2. Khối FSPEC (3 bytes)
	buffer[3] = 0x9B;
	buffer[4] = 0x0F;
	buffer[5] = 0x10;

	int offset = 6;

	// 3. FRN 1: I062/010 (Data Source Identifier)
	buffer[offset++] = 0x94; // SAC: Việt Nam
	buffer[offset++] = 0x01; // SIC

	// 4. FRN 4: I062/070 (Time of Track Information)
	DWORD dwTime = (GetTickCount() / 1000) * 128;
	buffer[offset++] = (dwTime >> 16) & 0xFF;
	buffer[offset++] = (dwTime >> 8) & 0xFF;
	buffer[offset++] = dwTime & 0xFF;

	// 5. FRN 5: I062/105 (Position in WGS-84) 
	double scalePos = 33554432.0 / 180.0;
	int32_t lat32 = (int32_t)(trackData.fLat * scalePos);
	int32_t lon32 = (int32_t)(trackData.fLon * scalePos);

	buffer[offset++] = (lat32 >> 24) & 0xFF;
	buffer[offset++] = (lat32 >> 16) & 0xFF;
	buffer[offset++] = (lat32 >> 8) & 0xFF;
	buffer[offset++] = lat32 & 0xFF;

	buffer[offset++] = (lon32 >> 24) & 0xFF;
	buffer[offset++] = (lon32 >> 16) & 0xFF;
	buffer[offset++] = (lon32 >> 8) & 0xFF;
	buffer[offset++] = lon32 & 0xFF;

	// 6. FRN 7: I062/185 (Calculated Track Velocity)
	int16_t vx = (int16_t)(trackData.fSpeed * 4.0f);
	int16_t vy = 0x0000;
	buffer[offset++] = (vx >> 8) & 0xFF;
	buffer[offset++] = vx & 0xFF;
	buffer[offset++] = (vy >> 8) & 0xFF;
	buffer[offset++] = vy & 0xFF;

	// 7. FRN 12: I062/040 (Track Number) 
	buffer[offset++] = (trackData.nTrackNumber >> 8) & 0xFF;
	buffer[offset++] = trackData.nTrackNumber & 0xFF;

	// 8. FRN 13: I062/080 (Track Status) - ĐÃ TÍCH HỢP cStatus
	buffer[offset++] = 0x01; // Octet 1: Mặc định tất cả bằng 0, bật bit cuối (FX=1) để mở Octet 2

	if (trackData.cStatus == 'N') {
		buffer[offset++] = 0x20; // Octet 2: Bật bit TSB (Track Service Begin) - 0010 0000
	}
	else if (trackData.cStatus == 'D') {
		buffer[offset++] = 0x40; // Octet 2: Bật bit TSE (Track Service End) - 0100 0000
	}
	else {
		buffer[offset++] = 0x00; // Octet 2: Quỹ đạo cập nhật bình thường ('U')
	}

	// 9. FRN 14: I062/290 (System Track Update Ages)
	buffer[offset++] = 0x80; // Map: Chọn Subfield 1 (Track Age)
	buffer[offset++] = 0x04; // Track Age: 1 giây (LSB = 1/4s)

							 // 10. FRN 18: I062/130 (Calculated Geometric Altitude)
	int16_t alt = (int16_t)(trackData.fAltitude / 6.25f);
	buffer[offset++] = (alt >> 8) & 0xFF;
	buffer[offset++] = alt & 0xFF;

	// --- GỬI GÓI TIN QUA UDP ---
	int nBytesSent = SendTo(buffer, 31, m_nCenterPort, m_strCenterIP);

	if (nBytesSent != SOCKET_ERROR)
	{
		CString strHex = _T("");
		CString strTemp;
		for (int i = 0; i < 36; i++)
		{
			strTemp.Format(_T("%02X "), buffer[i]);
			strHex += strTemp;
		}

		CString strLog;
		strLog.Format(_T("[TX ASTERIX to %s] %s"), m_strCenterIP, strHex);

		CRadSimDlg* pMainDlg = (CRadSimDlg*)AfxGetMainWnd();
		if (pMainDlg != NULL) pMainDlg->AddToMonitor(strLog);
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