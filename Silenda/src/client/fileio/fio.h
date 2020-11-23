#pragma once

namespace Silenda
{
	class FIO
	{
	public:
		static FIO* GetInstance();
		~FIO() {};

		const bool readFromFile(const std::string& fileDir, std::string& dest);
		const void writeToFile(const std::string& fileDir, const std::string& content);
	private:
		FILE* m_File = nullptr;
		unsigned long long m_FileSize = 0;
		char* m_FileBuffer = nullptr;
		unsigned long long m_ReadSize = 0;
	private:
		FIO() {};

		static FIO* m_Instance;
	};
}