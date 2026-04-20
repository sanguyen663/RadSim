#pragma once
#include "afxsock.h"

struct AsterixTrack {
	int nTrackNumber;      // Số hiệu quỹ đạo (TN)
	float fLat;            // Vĩ độ
	float fLon;            // Kinh độ
	float fSpeed;          // Vận tốc
	float fHeading;        // Hướng đi
	float fAltitude;       // Độ cao
	int nType;             // Kiểu loại
	char szIden[8];        // Nhận dạng (Ví dụ: "VN01")
	char cStatus;          // Trạng thái: 'N' (New), 'U' (Upd), 'D' (Del)
	int nQuality;		   // Chất lượng
};

class C2_Session :
	public CAsyncSocket
{
public:
	C2_Session();
	~C2_Session();
	//Lưu thông tin kết nối
	CString m_strCenterIP;
	UINT m_nCenterPort;
	UINT m_nMyPort; // Cổng của phiên này (vd: 10001)

					// Hàm để khởi tạo cổng riêng cho phiên
	BOOL InitSession(UINT myPort, CString centerIP, UINT centerPort);
	void SendTrackData(AsterixTrack trackData);
	DWORD m_dwLastHeartbeat;
	virtual void OnReceive(int nErrorCode);
};

