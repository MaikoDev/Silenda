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

		// Encrypt
		// @param1 raw serialized information to be encrypted
		// @param2 RSA public key to be used to encrypt
		// @return encrypted and compressed final string
		const std::string encrypt(const std::string& raw, const std::string& publicKey);

		// Decrypt
		// @param1 encrypted and compressed string awaiting decryption
		// @return raw serialized information after decryption
		// @info encrypted data must be encrypted with client's PK or data cannot be decrypted
		const std::string decrypt(const std::string& data);

		const std::string compress(const std::string& data);
		const std::string decompress(const std::string& data);

		const std::string& GetPublicKey() const&;

		// validate
		// @param1 data encrypted with client public key.
		// @return whether the data sent back was self's public key
		//  used to validate if connections are properly encrypted.
		const bool validate(const std::string& data);
		void genKeys(unsigned int keySize);
	private:
		CryptoPP::RSA::PrivateKey m_PrivateKey;
		CryptoPP::RSA::PublicKey m_PublicKey;
		std::string m_PublicKeyStr;

		CryptoPP::AutoSeededRandomPool m_RandPool;
	private:
		NetPacker() {};
		static NetPacker* m_Instance;
	};
}