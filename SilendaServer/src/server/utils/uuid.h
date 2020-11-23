#pragma once

#define UUID_DIGIT_SIZE 36

const inline unsigned char random_char()
{
	std::random_device rand_dev;
	std::mt19937 generator(rand_dev());
	std::uniform_int_distribution<> distr(0, 255);
	return static_cast<unsigned char>(distr(generator));
}

const inline std::string generate_hex(const unsigned int len)
{
	std::stringstream ss;
	for (auto i = 0; i < len; i++)
	{
		const auto rc = random_char();
		std::stringstream hexstream;
		hexstream << std::hex << int(rc);
		auto hex = hexstream.str();
		ss << (hex.length() < 2 ? '0' + hex : hex);
	}
	return ss.str();
}

const inline std::string generate_uuid()
{
	char* buffer = (char*)calloc(UUID_DIGIT_SIZE + 1, sizeof(char));

	strcat_s(buffer, UUID_DIGIT_SIZE + 1, generate_hex(4).c_str());
	strcat_s(buffer, UUID_DIGIT_SIZE + 1, "-");
	strcat_s(buffer, UUID_DIGIT_SIZE + 1, generate_hex(2).c_str());
	strcat_s(buffer, UUID_DIGIT_SIZE + 1, "-");
	strcat_s(buffer, UUID_DIGIT_SIZE + 1, generate_hex(2).c_str());
	strcat_s(buffer, UUID_DIGIT_SIZE + 1, "-");
	strcat_s(buffer, UUID_DIGIT_SIZE + 1, generate_hex(2).c_str());
	strcat_s(buffer, UUID_DIGIT_SIZE + 1, "-");
	strcat_s(buffer, UUID_DIGIT_SIZE + 1, generate_hex(6).c_str());

	std::string retString = buffer;
	free(buffer);

	return retString;
}