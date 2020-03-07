service TransferFile{
	bool UploadFile(1: string file_name, 2: binary file_data, 3: i64 file_length),
	bool isExistFile(1: string file_name),
	binary DownloadFile(1: string file_name)
}
