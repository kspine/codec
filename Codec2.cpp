#include "Codec2.h"

Codec2::Codec2()
{
	m_recvbuf_pos = 0;
	m_recvbuf_parse_pos = 0;
	m_recvbuf_total_len = 0;
	m_parse_url = false;
	m_parse_header = false;
	m_parse_body = false;

	m_sendbuf_pos = 0;
	m_sendbuf_total_len = 0;
}

Codec2::~Codec2()
{
	list<char *>::iterator iter;
	for(iter = m_recvbuf.begin(); iter != m_recvbuf.end(); ++iter)
		delete *iter;
	for(iter = m_sendbuf.begin(); iter != m_sendbuf.end(); ++iter)
		delete *iter;
}

void Codec2::appendReadData(char *pdata, unsigned int len)
{
	char *p;
	if(m_recvbuf.size() == 0)
	{
		p = new char[MSG_SEGMENT_LENGTH];
		m_recvbuf.push_back(p);	
	}
	else
	{
		p = m_recvbuf.back();
	}
	unsigned int pos = 0;
	unsigned int left = 0;
	do
	{
		left = MSG_SEGMENT_LENGTH - m_recvbuf_pos;
		if(len < left)
			break;
		memcpy(p + m_recvbuf_pos, pdata + pos, left);
		cout << "appendReadDataWhileOnce ....." << endl;
		Printmsg(pdata+pos, left);
		p = new char[MSG_SEGMENT_LENGTH];
		m_recvbuf.push_back(p);	
		m_recvbuf_pos = 0;	
		m_recvbuf_total_len += left;
		len -= left;
		pos += left;
	}while(true);

	memcpy(p + m_recvbuf_pos, pdata + pos, len);
	m_recvbuf_pos += len;
	m_recvbuf_total_len += len;
}

void Codec2::appendWriteData(char *pdata, unsigned int len)
{
	char *p;
	if(m_sendbuf.size() == 0)
	{
		p = new char[MSG_SEGMENT_LENGTH];
		m_sendbuf.push_back(p);	
	}
	else
	{
		p = m_sendbuf.back();
	}

	unsigned int pos = 0;
	unsigned int remaind_len = len;
	unsigned int remaind_seg = 0;
	do
	{
		remaind_seg = MSG_SEGMENT_LENGTH - m_sendbuf_total_len%(MSG_SEGMENT_LENGTH);
		if(remaind_len < remaind_seg)
			break;
		memcpy(p + m_sendbuf_total_len%(MSG_SEGMENT_LENGTH), pdata + pos, remaind_seg);
		m_sendbuf_total_len += remaind_seg;
		p = new char[MSG_SEGMENT_LENGTH];
		m_sendbuf.push_back(p);	
		remaind_len -= remaind_seg;
		pos += remaind_seg;
	}while(true);

	memcpy(p + m_sendbuf_total_len%(MSG_SEGMENT_LENGTH), pdata + pos, remaind_len);
	m_sendbuf_total_len += remaind_len;
}


void Codec2::releaseReadData()
{
	while(m_recvbuf_parse_pos >= MSG_SEGMENT_LENGTH)
	{
		char *p = m_recvbuf.front();
		m_recvbuf.pop_front();
		delete [] p;
		m_recvbuf_parse_pos -= MSG_SEGMENT_LENGTH;
		m_recvbuf_total_len -= MSG_SEGMENT_LENGTH;
	}
}

void Codec2::releaseWriteData()
{
	while(m_sendbuf_pos >= MSG_SEGMENT_LENGTH)
	{
		char *p = m_sendbuf.front();
		m_sendbuf.pop_front();
		delete [] p;
		m_sendbuf_pos -= MSG_SEGMENT_LENGTH;
		m_sendbuf_total_len -= MSG_SEGMENT_LENGTH;
	}
}

void* Codec2::decodeReadMsg()
{
	char *p = m_recvbuf.front();
	unsigned int head_len=sizeof(MsgHeader);
	MsgHeader head;;
	unsigned int parse_pos = m_recvbuf_parse_pos;
	CSMsg* pcmsg = NULL;
	if(parse_pos < m_recvbuf_total_len)
	{
		int my_flag = 0;
		unsigned int left = MSG_SEGMENT_LENGTH-parse_pos%(MSG_SEGMENT_LENGTH);
		list<char *>::iterator iter = m_recvbuf.begin();
		if(left > head_len)
		{
			memcpy(&head,p+parse_pos%(MSG_SEGMENT_LENGTH),head_len);
		}
		else
		{
			my_flag = 1;
			memcpy(&head,p+parse_pos%(MSG_SEGMENT_LENGTH),left);
			if(m_recvbuf.size() < 2)
				return NULL;			
			p = *(++iter);
			memcpy((char*)&head+left,p,head_len-left);
		}	
		parse_pos += head_len;

		if(head.CheckRecvHeader() != 0)
		{
			return NULL;
		}
		unsigned int msg_len = head.m_nSumDataLen-head_len;
		if (msg_len > m_recvbuf_total_len - parse_pos)
		{
			return NULL;
		}

		left = MSG_SEGMENT_LENGTH-parse_pos%(MSG_SEGMENT_LENGTH);
		int my_count = 0;
		int my_size = m_recvbuf.size();
		if (msg_len > left)
		{
			my_count ++;
			char *msg_data = new char[msg_len];
			memcpy(msg_data, p+parse_pos%(MSG_SEGMENT_LENGTH) ,left);
			int left_msg_len = msg_len - left;
			int count = 0;
			while(left_msg_len>=MSG_SEGMENT_LENGTH && iter != m_recvbuf.end() && ++iter != m_recvbuf.end())
			{
				memcpy(msg_data+left+(MSG_SEGMENT_LENGTH)*(count++), *iter,MSG_SEGMENT_LENGTH);
				left_msg_len -= MSG_SEGMENT_LENGTH;
			}		
			if (iter == m_recvbuf.end())//消息体长度不够
			{
				return NULL;
			}
			else if (left_msg_len>0 && ++iter == m_recvbuf.end())
			{
				return NULL;
			}
			else if(left_msg_len>0)
			{
				memcpy(msg_data+left+(MSG_SEGMENT_LENGTH)*count, *iter,left_msg_len);
			}
			else
			{
				;
			}
			pcmsg = new CSMsg(msg_data, msg_len);
		}
		else
		{
			pcmsg = new CSMsg(p+parse_pos%(MSG_SEGMENT_LENGTH), msg_len);
		}
		
		parse_pos += msg_len;
		m_recvbuf_parse_pos = parse_pos;
		releaseReadData();
		return pcmsg;
	}
	return pcmsg;
}

void Codec2::encodeWriteMsg(void *pmsg)
{
	CSMsg* pcmsg = (CSMsg*)pmsg;
	unsigned int head_len=sizeof(MsgHeader);
	MsgHeader head;
	head.InitSend(pcmsg->GetDataLen());
	appendWriteData((char*)&head, head_len);	
	appendWriteData(pcmsg->GetData(false), pcmsg->GetDataLen());	
	delete pcmsg;
}


unsigned int Codec2::retriveWriteData(char *outdata)
{
	if (m_sendbuf_total_len == 0)
	{
		return 0;
	}
	char *p = m_sendbuf.front();
	unsigned int left = MSG_SEGMENT_LENGTH-m_sendbuf_pos%(MSG_SEGMENT_LENGTH);	
	unsigned int msg_len = m_sendbuf_total_len-m_sendbuf_pos;
	list<char *>::iterator iter = m_sendbuf.begin();
	if(left >= msg_len)
	{
		memcpy(outdata, p + m_sendbuf_pos, msg_len);
	}
	else
	{
		memcpy(outdata, p + m_sendbuf_pos, left);

		int left_msg_len = msg_len - left;
		int count = 0;
		while(left_msg_len>=MSG_SEGMENT_LENGTH && ++iter != m_sendbuf.end())
		{
			memcpy(outdata+left+(MSG_SEGMENT_LENGTH)*(count++), *iter,MSG_SEGMENT_LENGTH);
			left_msg_len -= MSG_SEGMENT_LENGTH;
		}		
		if (iter == m_sendbuf.end())//消息体长度不够
		{
			return 0;
		}
		else if(left_msg_len>0 && ++iter != m_sendbuf.end())
		{
			memcpy(outdata+left+(MSG_SEGMENT_LENGTH)*count, *iter,left_msg_len);
		}
		else
		{
			;
		}
	}
	return msg_len;
}

void Codec2::reportSendedDataLength(unsigned int len)
{
	m_sendbuf_pos += len;
	releaseWriteData();
}



