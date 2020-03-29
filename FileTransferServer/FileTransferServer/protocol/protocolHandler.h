#pragma once

//#include "protocol.h"
class protocol_package_t;

class ProtocolHandler
{
public:
	void WriteBufferToPackage(protocol_package_t& package);
	void GetBufferFromPackage(protocol_package_t& package);
};