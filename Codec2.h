#pragma once

#include <list>
#include <map>
#include <string.h>
#include <string>
#include <stdio.h>
#include "Codec.h"
#include "SMsg.h"
#include "MsgHead.h"

#define MAX_HEADER_LINE_LENGTH 1024

using namespace std;


class Codec2:public Codec
{
public:
	Codec2();
	~Codec2();
	void appendReadData(char *pdata, unsigned int len);
	void appendWriteData(char *pdata, unsigned int len);
	void releaseReadData();
	void releaseWriteData();
	void* decodeReadMsg();
	void encodeWriteMsg(void *pmsg);
	unsigned int retriveWriteData(char *outdata);
	void reportSendedDataLength(unsigned int len);

protected:
	
	list<char *> m_recvbuf;
	unsigned int m_recvbuf_pos;
	unsigned int m_recvbuf_total_len;
	unsigned int m_recvbuf_parse_pos;

	list<char *> m_sendbuf;
	unsigned int m_sendbuf_pos;
	unsigned int m_sendbuf_total_len;

	bool m_parse_url;
	bool m_parse_header;
	bool m_parse_body;
};
