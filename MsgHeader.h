/*************************************************************************
	> File Name: MsgHeader.h
	> Author: 
	> Mail: 
	> Created Time: 2017年01月06日 星期五 18时03分02秒
 ************************************************************************/

#ifndef _MSGHEADER_H
#define _MSGHEADER_H

class MsgHeader
{
public:
	MsgHeader()
	{
		m_nCheck1 = 0;
		m_nSumDataLen = 0;
		m_nCheck2 = 0;
	}
	~MsgHeader()
	{

	}

	void InitSend(int nSendLen)
	{
		m_nCheck1 = 33;	
		m_nCheck2 = 99;
		m_nSumDataLen = nSendLen + sizeof(MsgHeader);
	}
	int CheckRecvHeader()
	{
		if(m_nCheck1 == 33 && m_nCheck2 == 99 && m_nSumDataLen > sizeof(MsgHeader) )
		{
			return 0;
		}
		return -1;
	}
	int     		m_nCheck1;      //消息头位置校验(这个值必须为33才算正确);	
	unsigned int    m_nSumDataLen;  //当前消息的总长度(消息头 + 消息体)
	int     		m_nCheck2;      //消息头位置校验(这个值必须为99才算正确);
};

#endif
