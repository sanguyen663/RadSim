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
	BYTE buffer[64];
	memset(buffer, 0, 64);

	// 1. Khối Header của ASTERIX 
	buffer[0] = 62; // CAT = 062
	buffer[1] = 0;

	// 2. Khối FSPEC (KÉO DÀI THÀNH 4 BYTES)
	buffer[3] = 0x99; // FRN 1, 4, 5, FX=1
	buffer[4] = 0x1F; // FRN 11, 12, 13, 14, FX=1
	buffer[5] = 0x11; // FRN 18, FX=1 (Mở byte số 4)
	buffer[6] = 0x28; // FRN 24, FX=0 (Đóng FSPEC)

	int offset = 7;

	// 3. FRN 1: I062/010 (Data Source)
	buffer[offset++] = 0x94;
	buffer[offset++] = 0x01;

	// 4. FRN 4: I062/070 (Time)
	DWORD dwTime = (GetTickCount() / 1000) * 128;
	buffer[offset++] = (dwTime >> 16) & 0xFF;
	buffer[offset++] = (dwTime >> 8) & 0xFF;
	buffer[offset++] = dwTime & 0xFF;

	// 5. FRN 5: I062/105 (Position)
	double scalePos = 33554432.0 / 180.0;
	int32_t lat32 = (int32_t)(trackData.fLat * scalePos);
	int32_t lon32 = (int32_t)(trackData.fLon * scalePos);
	buffer[offset++] = (lat32 >> 24) & 0xFF; buffer[offset++] = (lat32 >> 16) & 0xFF;
	buffer[offset++] = (lat32 >> 8) & 0xFF;  buffer[offset++] = lat32 & 0xFF;
	buffer[offset++] = (lon32 >> 24) & 0xFF; buffer[offset++] = (lon32 >> 16) & 0xFF;
	buffer[offset++] = (lon32 >> 8) & 0xFF;  buffer[offset++] = lon32 & 0xFF;

	// 6. FRN 11: I062/180 (Velocity Polar)
	uint16_t speed = (uint16_t)(trackData.fSpeed * 10.0f);
	uint16_t heading = (uint16_t)(trackData.fHeading * (65536.0f / 360.0f));
	buffer[offset++] = (speed >> 8) & 0xFF;   buffer[offset++] = speed & 0xFF;
	buffer[offset++] = (heading >> 8) & 0xFF; buffer[offset++] = heading & 0xFF;

	// 7. FRN 12: I062/040 (Track Number) 
	buffer[offset++] = (trackData.nTrackNumber >> 8) & 0xFF;
	buffer[offset++] = trackData.nTrackNumber & 0xFF;

	// 8. FRN 13: I062/080 (Track Status + Type)
	buffer[offset++] = 0x01;
	BYTE octet2 = 0x01;
	if (trackData.cStatus == 'N') octet2 |= 0x20;
	else if (trackData.cStatus == 'D') octet2 |= 0x40;
	buffer[offset++] = octet2;
	buffer[offset++] = 0x01;
	BYTE octet4 = 0x00;
	octet4 |= ((trackData.nType & 0x0F) << 1);
	buffer[offset++] = octet4;

	// 9. FRN 14: I062/290 (Update Ages)
	buffer[offset++] = 0x80;
	buffer[offset++] = 0x04;

	// 10. FRN 18: I062/130 (Altitude)
	int16_t alt = (int16_t)(trackData.fAltitude / 6.25f);
	buffer[offset++] = (alt >> 8) & 0xFF;
	buffer[offset++] = alt & 0xFF;

	// 11. FRN 24: I062/245 (Target Identification) --- THÊM MỚI Ở ĐÂY
	char callsign[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
	for (int i = 0; i < 8 && trackData.szIden[i] != '\0'; i++) {
		callsign[i] = trackData.szIden[i];
	}

	// Mã hóa 8 ký tự ASCII sang dạng 6-bit (tổng cộng 48 bits = 6 bytes)
	uint64_t encoded = 0;
	for (int i = 0; i < 8; i++) {
		encoded = (encoded << 6) | (callsign[i] & 0x3F);
	}

	buffer[offset++] = 0x00; // STI = 0 (Callsign default)
	buffer[offset++] = (encoded >> 40) & 0xFF;
	buffer[offset++] = (encoded >> 32) & 0xFF;
	buffer[offset++] = (encoded >> 24) & 0xFF;
	buffer[offset++] = (encoded >> 16) & 0xFF;
	buffer[offset++] = (encoded >> 8) & 0xFF;
	buffer[offset++] = encoded & 0xFF;

	// 12. FRN 26: I062/501 (Track Quality)
	buffer[offset++] = trackData.nQuality & 0xFF;

	// Cập nhật lại tổng chiều dài
	buffer[2] = offset;

	// --- GỬI GÓI TIN ---
	int nBytesSent = SendTo(buffer, offset, m_nCenterPort, m_strCenterIP);
	if (nBytesSent != SOCKET_ERROR) {
		CString strHex = _T(""), strTemp;
		for (int i = 0; i < offset; i++) {
			strTemp.Format(_T("%02X "), buffer[i]);
			strHex += strTemp;
		}
		CString strLog;
		strLog.Format(_T("[TX to %s] %s"), m_strCenterIP, strHex);
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