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

/* ��ϢID */
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
	u8         type;       // content type��jsonЭ���ʽ
	u16        msg_id;     // message id
	u16        msg_len;    // content length����Ϣ�ı߽磬TCPճ��
	u32        file_id;    // �ļ���� ����֪�����͵����ĸ��ļ�
	u32        file_len;   //�ļ����ȣ�����λ�ֽڣ� 
	u32        all_pack;   //���ݰ��ܸ���
	u32        seq;        // �������к� ���ڼ������ݰ���
	u32        md5[8];     //md5�ַ�����32���ַ�
	u32        reserve;    // reserve bytes

};


/*****
����Internet���ʱ��ͬ,��ΪInternet�ϵ�·�������ܻὫMTU��Ϊ��ͬ��ֵ.
������Ǽٶ�MTUΪ1500���������ݵ�,��;����ĳ�������MTUֵС��1500�ֽ�,��ôϵͳ����ʹ��һϵ�еĻ�
��������MTUֵ,ʹ���ݱ��ܹ�˳������Ŀ�ĵ�,�����ͻ�����಻��Ҫ�Ĳ���.

Ŀǰ�������·���豸��MTU��Ϊ1500

����Internet�ϵı�׼MTUֵΪ576�ֽ�,�����ҽ����ڽ���Internet��UDP���ʱ.
��ý�UDP�����ݳ��ȿؼ���548�ֽ�(576-8 (UDPͷ)-20(IPͷ))����.
��ý�TCP�����ݳ��ȿؼ���536�ֽ�(576-20(TCPͷ)-20(IPͷ))����.

��������һƪ����˵����Ӧ���и�PPP�İ�ͷ��β�Ŀ�����8Bytes),�Ǿ�Ϊ1492��

UDP ���Ĵ�С��Ӧ���� 1492 - IPͷ(20) - UDPͷ(8) = 1464(BYTES)
TCP ���Ĵ�С��Ӧ���� 1492 - IPͷ(20) - TCPͷ(20) = 1452(BYTES)
*****/

/*protocol_package_t ���ֽ�Ŀǰ����Ϊ536  Э��ͷΪ28 ĿǰЭ����Ϊ  508 */
class protocol_package_t
{
public:
	protocol_package_t()
	{
		memset(body, 0, tcp_package_size - sizeof(protocol_head_t));  //tcp_package_size - sizeof(protocol_head_t)
	}

	protocol_head_t header;   //60�ֽ�
	u8 body[tcp_package_size - sizeof(protocol_head_t)];  //536-60 = 476  tcp_package_size-sizeof(protocol_head_t)
};
