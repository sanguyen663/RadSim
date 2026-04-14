#pragma once
#include "afxsock.h"
class CC_Listen : public CAsyncSocket
{
public:
	CC_Listen();
	~CC_Listen();
	virtual void OnReceive(int nErrorCode);
};

