#include "pch.h"

#include "fio.h"

namespace Silenda
{
	FIO* FIO::m_Instance = nullptr;

	FIO* FIO::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new FIO();
		return m_Instance;
	}

	const bool FIO::readFromFile(const std::string& fileDir, std::string& dest)
	{
		fopen_s(&m_File, fileDir.c_str(), "r");

		if (m_File == NULL)
			return 0;
		else
		{
			// Get length of file contents 
			fseek(m_File, 0, SEEK_END);
			m_FileSize = ftell(m_File);
			rewind(m_File);

			// Create buffer to size of length
			m_FileBuffer = (char*)calloc(m_FileSize + 1, sizeof(char));

			// Copy file content to buffer
			m_ReadSize = fread(m_FileBuffer, sizeof(char), m_FileSize, m_File);
			if (m_ReadSize != m_FileSize)
			{
				free(m_FileBuffer);
				m_FileBuffer = nullptr;

				fclose(m_File);
				m_File = nullptr;

				return 0;
			}

			dest = m_FileBuffer;

			free(m_FileBuffer);
			m_FileBuffer = nullptr;

			fclose(m_File);
			m_File = nullptr;

			return 1;
		}
	}

	const void FIO::writeToFile(const std::string& fileDir, const std::string& content)
	{
		fopen_s(&m_File, fileDir.c_str(), "w");
		if (m_File != NULL)
		{
			fwrite(content.c_str(), sizeof(char), content.size(), m_File);
			fclose(m_File);
			m_File = nullptr;
		}
	}
}