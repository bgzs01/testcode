#pragma once

#include "TransferFile.h"

class TransferFileServer : virtual public TransferFileIf {
public:
	TransferFileServer();

	bool UploadFile(const std::string& file_name, const std::string& file_data, const int64_t file_length);
	bool isExistFile(const std::string& file_name);
	void DownloadFile(std::string& _return, const std::string& file_name);

};