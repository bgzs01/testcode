#pragma once

#include "global_def.h"
#include <string>


//const int tcp_package_size = 536;
//const int udp_package_size = 548;

const int tcp_package_size = 1452;
//const int tcp_package_size = 1024 * 16;
const int udp_package_size = 548;

const u16 protocol_tag = 0xFBFE;

enum protocol_type_t
{
	JSON_PROTOCOL_TYPE = 0,  // json
	PB_PROTOCOL_TYPE = 1,  // Google Protocol Buffer
	FB_PROTOCOL_TYPE = 2,  // FlatBuffers
	BINARY_PROTOCOL_TYPE = 3,  // binary format

	UNKNOW_PROTOCOL_TYPE = 0xFF
};

/* 消息ID */
enum EventID
{
	EEVENTID_COMMON_RSP = 0x00,

	EEVENTID_DOWNLOAD_FILE_REQ = 0x01,
	EEVENTID_DOWNLOAD_FILE_RSP = 0x02,

	EEVENTID_FILE_INFO_REQ = 0x03,
	EEVENTID_FILE_INFO_RSP = 0x04,

	EEVENTID_FILE_DATA_REQ = 0x05,
	EEVENTID_FILE_DATA_RSP = 0x06,

	EEVENTID_FILE_END_REQ = 0x07,
	EEVENTID_FILE_END_RSP = 0x08,

	//EEVENTID_LIST_ACCOUNT_RECORDS_REQ = 0x09,
	//EEVENTID_LIST_ACCOUNT_RECORDS_RSP = 0x10,

	//EEVENTID_UNLOCK_REQ = 0x11,
	//EEVENTID_UNLOCK_RSP = 0x12,

	//EEVENTID_LIST_TRAVELS_REQ = 0x13,
	//EEVENTID_LIST_TRAVELS_RSP = 0x14,

	//EEVENTID_LOCK_REQ = 0x15,
	//EEVENTID_LOCK_RSP = 0x16,

	EEVENTID_UNKOWN = 0xFFFF

};


class protocol_head_t
{
public:
	protocol_head_t()
		:tag(protocol_tag), version(0x01), type(BINARY_PROTOCOL_TYPE),
		msg_id(0x0000), msg_len(0x0000), file_id(0), file_len(0x00),
		all_pack(0x0), seq(0x0), 
		reserve(0x0)
	{
		memset(md5, 0, sizeof(md5));
	}

	std::string getVerStr()
	{
		std::string masVer = std::to_string(version & 0xf0);
		std::string subVer = std::to_string(version & 0x0f);

		std::string result = masVer + "." + subVer;
		return result;
	}

	std::string getMd5Str()
	{
		char buf[32 + 1] = {0};
		memcpy(buf, md5, sizeof(md5));
		std::string md5Str(buf);
		return md5Str;
	}

	void setMd5Str(std::string md5Str)
	{
		memcpy(md5, md5Str.data(), sizeof(md5));
	}

public:
	u16        tag;        // 0xFBFE
	u8         version;    // protocol version, high 4 bit is master version, 
	// low 4 bit is sub version
	u8         type;       // content type，json协议格式
	u16        msg_id;     // message id
	u16        msg_len;    // content length，消息的边界，TCP粘包
	u32        file_id;    // 文件编号 用于知道发送的是哪个文件
	u32        file_len;   //文件长度，（单位字节） 
	u32        all_pack;   //数据包总个数
	u32        seq;        // 发送序列号 （第几个数据包）
	u32        md5[8];     //md5字符串共32个字符
	u32        reserve;    // reserve bytes

};


/*****
进行Internet编程时则不同,因为Internet上的路由器可能会将MTU设为不同的值.
如果我们假定MTU为1500来发送数据的,而途经的某个网络的MTU值小于1500字节,那么系统将会使用一系列的机
制来调整MTU值,使数据报能够顺利到达目的地,这样就会做许多不必要的操作.

目前大多数的路由设备的MTU都为1500

鉴于Internet上的标准MTU值为576字节,所以我建议在进行Internet的UDP编程时.
最好将UDP的数据长度控件在548字节(576-8 (UDP头)-20(IP头))以内.
最好将TCP的数据长度控件在536字节(576-20(TCP头)-20(IP头))以内.

看到另外一篇文章说，还应该有个PPP的包头包尾的开销（8Bytes),那就为1492了

UDP 包的大小就应该是 1492 - IP头(20) - UDP头(8) = 1464(BYTES)
TCP 包的大小就应该是 1492 - IP头(20) - TCP头(20) = 1452(BYTES)
*****/

/*protocol_package_t 总字节目前设置为536  协议头为28 目前协议体为  508 */
class protocol_package_t
{
public:
	protocol_package_t()
	{
		memset(body, 0, tcp_package_size - sizeof(protocol_head_t));  //tcp_package_size - sizeof(protocol_head_t)
	}

	protocol_head_t header;   //60字节
	u8 body[tcp_package_size - sizeof(protocol_head_t)];  //536-60 = 476  tcp_package_size-sizeof(protocol_head_t)
};
