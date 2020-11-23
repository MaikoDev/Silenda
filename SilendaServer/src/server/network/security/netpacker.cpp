#include "pch.h"
#include "netpacker.h"

namespace SilendaServer
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
		if (raw.size() <= m_PlainSizeLimit)
		{
			std::string retString;

			// Load public key
			std::string buffer;
			CryptoPP::HexDecoder decoder;
			decoder.Attach(new CryptoPP::StringSink(buffer));
			decoder.Put((CryptoPP::byte*)pubStrKey.data(), pubStrKey.size());
			decoder.MessageEnd();

			CryptoPP::StringSource ss(buffer, true);
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

		return "";
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

	const std::string& NetPacker::GetPublicKey()
	{
		if (m_PublicKeyStr == "")
		{
			std::string buffer;
			CryptoPP::StringSink ss(buffer);
			m_PublicKey.Save(ss);

			std::string retString;
			CryptoPP::HexEncoder encoder;
			encoder.Attach(new CryptoPP::StringSink(retString));
			encoder.Put((CryptoPP::byte*)buffer.data(), buffer.size());
			encoder.MessageEnd();

			m_PublicKeyStr = retString;
		}

		return m_PublicKeyStr;
	}

	const bool NetPacker::validate(const std::string& data)
	{
		return (m_PublicKeyStr == decrypt(data));
	}

	const bool NetPacker::genKeys(unsigned int keySize)
	{
		if (IsPowTwo(keySize))
		{
			CryptoPP::InvertibleRSAFunction params;
			params.GenerateRandomWithKeySize(m_RandPool, keySize);

			m_PrivateKey = CryptoPP::RSA::PrivateKey(params);
			m_PublicKey = CryptoPP::RSA::PublicKey(params);
			m_PublicKeyStr = "";
			m_KeySizeInBytes = keySize / 8;
			m_PlainSizeLimit = m_KeySizeInBytes - 42;

			return 1;
		}

		return 0;
	}
}