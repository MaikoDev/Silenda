#pragma once

const inline void byte_cpy(const char* input, const size_t cpyLength, std::string& dest)
{
	std::string buffer(cpyLength, '0');
	for (unsigned int i = 0; i < cpyLength; i++)
		buffer[i] = input[i];

	dest = buffer;
}