#include "Codec.h"

Codec::Codec()
{
}

Codec::~Codec()
{
}

void Codec::appendReadData(char *pdata, unsigned int len)
{
}

void Codec::releaseReadData()
{
}

void* Codec::decodeReadMsg()
{
	return NULL;
}


void Codec::encodeWriteMsg(void *pmsg)
{
}


unsigned int Codec::retriveWriteData(char *outdata)
{
	return 0;
}

void Codec::reportSendedDataLength(unsigned int len)
{
}


