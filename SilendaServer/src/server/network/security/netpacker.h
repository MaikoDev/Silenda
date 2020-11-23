#pragma once

namespace SilendaServer
{
	template<class T>
	const inline std::string serialize(const T& obj)
	{
		std::stringstream buffer;
		msgpack::pack(buffer, obj);

		buffer.seekg(0);
		std::string ret(buffer.str());

		return ret;
	}

	template<class T>
	const inline T deserialize(const std::string& serialized)
	{
		msgpack::object_handle objectHandle = msgpack::unpack(serialized.data(), serialized.size());
		msgpack::object deserialized = objectHandle.get();
		T retObj;
		deserialized.convert(retObj);

		return retObj;
	}

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

		const std::string& GetPublicKey();

		// validate
		// @param1 data encrypted with client public key.
		// @return whether the data sent back was self's public key
		//  used to validate if connections are properly encrypted.
		const bool validate(const std::string& data);
		const bool genKeys(unsigned int keySize);

		const inline unsigned int& getKeySizeInBytes() const & { return m_KeySizeInBytes; };
		const inline unsigned int& getPlainSizeLimit() const & { return m_PlainSizeLimit; };
	private:
		const inline bool IsPowTwo(const unsigned int& num) { return (num != 0) && ((num & (num - 1)) == 0); };
	private:
		CryptoPP::RSA::PrivateKey m_PrivateKey;
		CryptoPP::RSA::PublicKey m_PublicKey;
		std::string m_PublicKeyStr;

		unsigned int m_KeySizeInBytes;
		unsigned int m_PlainSizeLimit;

		CryptoPP::AutoSeededRandomPool m_RandPool;
	private:
		NetPacker() {};
		static NetPacker* m_Instance;
	};
}