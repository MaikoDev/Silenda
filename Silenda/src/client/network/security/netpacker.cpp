#include "pch.h"
#include "netpacker.h"

namespace Silenda
{
	NetPacker* NetPacker::m_Instance = nullptr;

	NetPacker* NetPacker::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new NetPacker();
		return m_Instance;
	}

	const std::string NetPacker::encrypt(const std::string& raw, const std::string& pubStrKey)
	{
		std::string retString;

		// Load public key
		CryptoPP::StringSource ss(pubStrKey, true);
		CryptoPP::RSA::PublicKey publicKey;
		publicKey.Load(ss);

		// Encryption
		CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey);

		CryptoPP::StringSource ss1(raw, true, 
			new CryptoPP::PK_EncryptorFilter(m_RandPool, encryptor, 
				new CryptoPP::StringSink(retString)
			)
		);

		// Compression
		std::string compressed;

		CryptoPP::Gzip zipper(new CryptoPP::StringSink(compressed));
		zipper.Put((CryptoPP::byte*)retString.c_str(), retString.size());
		zipper.MessageEnd();

		retString = compressed;
		return retString;
	}

	const std::string NetPacker::decrypt(const std::string& data)
	{
		std::string retString;

		// Decompression
		std::string decompressed;

		CryptoPP::Gunzip unzipper(new CryptoPP::StringSink(decompressed));
		unzipper.Put((unsigned char*)data.data(), data.size());
		unzipper.MessageEnd();

		// Decryption
		CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(m_PrivateKey);

		CryptoPP::StringSource ss(decompressed, true, 
			new CryptoPP::PK_DecryptorFilter(m_RandPool, decryptor, 
				new CryptoPP::StringSink(retString)
			)
		);

		return retString;
	}

	const std::string NetPacker::compress(const std::string& data)
	{
		// Compression
		std::string compressed;

		CryptoPP::Gzip zipper(new CryptoPP::StringSink(compressed));
		zipper.Put((CryptoPP::byte*)data.c_str(), data.size());
		zipper.MessageEnd();

		return compressed;
	}

	const std::string NetPacker::decompress(const std::string& data)
	{
		// Decompression
		std::string decompressed;

		CryptoPP::Gunzip unzipper(new CryptoPP::StringSink(decompressed));
		unzipper.Put((unsigned char*)data.data(), data.size());
		unzipper.MessageEnd();

		return decompressed;
	}

	const std::string& NetPacker::GetPublicKey() const&
	{
		if (m_PublicKeyStr == "")
		{
			std::string retString;
			CryptoPP::StringSink ss(retString);
			m_PublicKey.Save(ss);
		}

		return m_PublicKeyStr;
	}

	const bool NetPacker::validate(const std::string& data)
	{
		return (m_PublicKeyStr == decrypt(data));
	}

	void NetPacker::genKeys(unsigned int keySize)
	{
		CryptoPP::InvertibleRSAFunction params;
		params.GenerateRandomWithKeySize(m_RandPool, keySize);

		m_PrivateKey = CryptoPP::RSA::PrivateKey(params);
		m_PublicKey = CryptoPP::RSA::PublicKey(params);
		m_PublicKeyStr = "";
	}
}