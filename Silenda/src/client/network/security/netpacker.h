#pragma once

#include "cryptopp/rsa.h"
#include "cryptopp/osrng.h"

namespace Silenda
{
	class NetPacker
	{
	public:
		~NetPacker() {};

		static NetPacker* GetInstance();

		const std::string encrypt(const std::string& raw, const std::string& publicKey);
		const std::string decrypt(const std::string& data);

		const std::string GetPublicKey() const&;
		void genKeys(unsigned int keySize);
	private:
		CryptoPP::RSA::PrivateKey m_PrivateKey;
		CryptoPP::RSA::PublicKey m_PublicKey;

		CryptoPP::AutoSeededRandomPool m_RandPool;
	private:
		NetPacker() {};
		static NetPacker* m_Instance;
	};
}