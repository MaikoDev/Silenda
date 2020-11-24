#pragma once

namespace SilendaServer
{
	class FIO
	{
	public:
		static FIO* GetInstance();
		~FIO() {};

		const bool readFromFile(const std::string& fileDir, std::string& dest);
		const void writeToFile(const std::string& fileDir, const std::string& content);

		template<class T>
		const bool readObjFromFile(const std::string& fileDir, T& obj)
		{
			std::string str;
			if (!this->readFromFile(fileDir, str))
				return 0;

			nlohmann::json j = nlohmann::json::parse(str);
			obj = j;
		}

		template<class T>
		const void writeObjToFile(const std::string& fileDir, const T& obj)
		{
			nlohmann::json j = obj;
			std::string str = j.dump(4);

			this->writeToFile(fileDir, str);
		}
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