/**
* Author: zhengjianhui
* Desc: A code method to parse the tcp data, and compose the message
* Date: 2016-10-31
* Modify: XXXX-XX-XX XXXXX XXXXXXXXXXXXXXXXXXXXXX
*/

#pragma once

#include <iostream>
#include "Global.h"

class Codec
{
public:
	Codec();
	virtual ~Codec();
	/**
	* Append the receive data from tcp link, prepare for isPackage method to parse message
	* Param1: data buffer
	* Param2: data length
	*/
	virtual void appendReadData(char *pdata, unsigned int len);

	/**
	* Cut the used data and release in read buffer
	*/
	virtual void releaseReadData();


	/**
	* Get the valid message when there is a valid message
	* Return: the message pointer, NULL if there is no message to parse
	*/
	virtual void* decodeReadMsg();

	//////////////////////////////////////////////////////////////////////////
	
	/**
	* Encode the message into data and store it to prepare to send
	* Param1: message pointer
	*/
	virtual void encodeWriteMsg(void *pmsg);
	/**
	* Get the data prepare to send, some data maybe send next time
	* Param1: the receive data buffer
	* Return: the data length to receive, 0 if there is no data left
	*/
	virtual unsigned int retriveWriteData(char *outdata);
	/**
	* The real data length send completely
	*/
	virtual void reportSendedDataLength(unsigned int len);
	


};

