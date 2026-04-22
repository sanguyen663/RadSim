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
	buffer[1] = 0;  // LEN High byte
					// buffer[2] sẽ được gán giá trị độ dài cuối cùng

					// 2. Cập nhật FSPEC (4 BYTES)
					// Byte 1: F1, F2(0), F3, F4, F5, F6(0), F7(0), FX=1 -> 1011 1001 = 0xB9
	buffer[3] = 0xB9;
	// Byte 2: F8, F9(0), F10, F11, F12(0), F13(0), F14(0), FX=1 -> 1011 0001 = 0xB1
	buffer[4] = 0xB1;
	// Byte 3: F15(0), F16, F17, F18(0), F19(0), F20(0), F21(0), FX=1 -> 0110 0001 = 0x61
	buffer[5] = 0x61;
	// Byte 4: F22(0), F23(0), F24(0), F25(0), F26, FX=0 -> 0000 1000 = 0x08
	buffer[6] = 0x08;

	int offset = 7;

	// FRN 1: I062/010 - Data Source Identifier [2 bytes] [cite: 401]
	buffer[offset++] = 0x94;
	buffer[offset++] = 0x01;

	// FRN 3: I062/070 - Time of Track Information [3 bytes] [cite: 527]
	DWORD dwTime = (GetTickCount() / 1000) * 128;
	buffer[offset++] = (dwTime >> 16) & 0xFF;
	buffer[offset++] = (dwTime >> 8) & 0xFF;
	buffer[offset++] = dwTime & 0xFF;

	// FRN 4: I062/040 - Track Number [2 bytes] [cite: 538]
	buffer[offset++] = (trackData.nTrackNumber >> 8) & 0xFF;
	buffer[offset++] = trackData.nTrackNumber & 0xFF;

	// FRN 5: I062/105 - Calculated Position (WGS-84) [6 bytes] [cite: 546]
	double scalePos = 8388608.0 / 180.0; // LSB = 180/2^23 [cite: 551]
	int32_t lat32 = (int32_t)(trackData.fLat * scalePos);
	int32_t lon32 = (int32_t)(trackData.fLon * scalePos);
	buffer[offset++] = (lat32 >> 16) & 0xFF;
	buffer[offset++] = (lat32 >> 8) & 0xFF;
	buffer[offset++] = lat32 & 0xFF;
	buffer[offset++] = (lon32 >> 16) & 0xFF;
	buffer[offset++] = (lon32 >> 8) & 0xFF;
	buffer[offset++] = lon32 & 0xFF;

	// FRN 8: I062/130 - Calculated Track Altitude [2 bytes] [cite: 554]
	int16_t alt = (int16_t)(trackData.fAltitude / 2.0f); // LSB = 2m [cite: 558]
	buffer[offset++] = (alt >> 8) & 0xFF;
	buffer[offset++] = alt & 0xFF;

	// FRN 10: I062/245 - Target Identification [7 bytes]
	// Byte đầu tiên: STI (00) + bit dự phòng
	buffer[offset++] = 0x00;
	char callsign[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
	for (int i = 0; i < 8 && trackData.szIden[i] != '\0'; i++) {
		callsign[i] = trackData.szIden[i];
	}
	uint64_t encoded = 0;
	for (int i = 0; i < 8; i++) {
		encoded = (encoded << 6) | (callsign[i] & 0x3F);
	}
	buffer[offset++] = (encoded >> 40) & 0xFF;
	buffer[offset++] = (encoded >> 32) & 0xFF;
	buffer[offset++] = (encoded >> 24) & 0xFF;
	buffer[offset++] = (encoded >> 16) & 0xFF;
	buffer[offset++] = (encoded >> 8) & 0xFF;
	buffer[offset++] = encoded & 0xFF;

	// FRN 11: I062/180 - Calculated Track Velocity (Polar) [4 bytes] [cite: 562]
	uint16_t speed = (uint16_t)(trackData.fSpeed * 10.0f); // LSB = 0.1 m/s [cite: 567]
	uint16_t heading = (uint16_t)(trackData.fHeading * (65536.0f / 360.0f));
	buffer[offset++] = (speed >> 8) & 0xFF;
	buffer[offset++] = speed & 0xFF;
	buffer[offset++] = (heading >> 8) & 0xFF;
	buffer[offset++] = heading & 0xFF;

	// FRN 16: I062/080 - Track Status [4 bytes] [cite: 571, 573]
	buffer[offset++] = 0x89; // Main: MON=1, SRC=010 (Radar 3D), FX=1 [cite: 575-576]
	BYTE ext1 = 0x01;
	if (trackData.cStatus == 'N') ext1 |= 0x20; // TSB [cite: 581]
	if (trackData.cStatus == 'D') ext1 |= 0x40; // TSE [cite: 581]
	buffer[offset++] = ext1;
	buffer[offset++] = 0x01; // FX=1 sang Ext 3
	BYTE ext3 = 0x00;
	ext3 |= ((trackData.nType & 0x0F) << 1); // Loại mục tiêu [cite: 587]
	buffer[offset++] = ext3;

	// FRN 17: I062/290 - System Track Update Ages [3 bytes] [cite: 589, 591]
	buffer[offset++] = 0x81; // PSR=1, FX=1 [cite: 593-594]
	buffer[offset++] = 0x00;
	buffer[offset++] = 0x04; // PSR Age (1s) [cite: 597-598]

							 // FRN 26: I062/501 - Track Quality [1 byte] [cite: 599, 602]
	buffer[offset++] = trackData.nQuality & 0xFF;

	// Cập nhật chiều dài LEN
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