#include <iostream>
#include <memory>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <ctime>

//#include "client/network/chat/cl_message.h"
#include "client/page/chatapp.h"
#include "msgpack.hpp"

#include "client/page/chat.h"
#include "client/page/login.h"
#include "client/console/msghandler.h"
#include "client/render/renderer.h"
#include "client/console/mtc.h"
#include "cryptopp/osrng.h"
#include "cryptopp/rsa.h"
#include "cryptopp/filters.h"
#include "cryptopp/gzip.h"

#include "client/network/security/netpacker.h"

using namespace render;

int main(int argc, char** argv)
{
	Silenda::NetPacker* packer = Silenda::NetPacker::GetInstance();
	packer->genKeys(4096);

	Silenda::ChatMessage message(L"Aegis", UserLevel::superuser, time(0), L"Hello World!");

	std::stringstream buffer;
	msgpack::pack(buffer, message);

	buffer.seekg(0);
	std::string str(buffer.str());

	msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());
	msgpack::object deserialized = oh.get();
	msgpack::type::tuple<std::wstring, unsigned int, __int64, std::wstring> dst;
	deserialized.convert(dst);

	Silenda::MTConsole* consolePtr = Silenda::MTConsole::GetInstance();
	Silenda::MsgHandler* handlerPtr = Silenda::MsgHandler::GetInstance();

	Renderer* rPtr = Renderer::GetInstance();

	Silenda::Page* chat = new Silenda::ChatPage();
	Silenda::Page* login = new Silenda::LoginPage();

	login->LinkPage("SilendaChat", chat);

	login->load();

	rPtr->init(117, 29);
	Silenda::BaseRunningState = true;

	while (Silenda::BaseRunningState)
	{
		rPtr->draw(Silenda::LoadedPage);
		rPtr->flush();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	/*CryptoPP::AutoSeededRandomPool rng;

	CryptoPP::InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 4096);

	CryptoPP::RSA::PrivateKey privateKey(params);
	CryptoPP::RSA::PublicKey publicKey(params);

	std::string testSave;
	CryptoPP::StringSink pss(testSave);
	publicKey.Save(pss);

	CryptoPP::StringSource ss(testSave, true);
	CryptoPP::RSA::PublicKey loadpublicKey;
	loadpublicKey.Load(ss);

	std::string cipher, compressed, decompressed, recovered;

	CryptoPP::RSAES_OAEP_SHA_Encryptor e(loadpublicKey);

	CryptoPP::StringSource ss1("Hello Secret World!", true,
		new CryptoPP::PK_EncryptorFilter(rng, e,
			new CryptoPP::StringSink(cipher)
		) // PK_EncryptorFilter
	); // StringSource

	CryptoPP::Gzip zipper(new CryptoPP::StringSink(compressed));
	zipper.Put((CryptoPP::byte*) cipher.c_str(), cipher.size());
	zipper.MessageEnd();

	CryptoPP::Gunzip unzipper(new CryptoPP::StringSink(decompressed));
	unzipper.Put((unsigned char*) compressed.data(), compressed.size());
	unzipper.MessageEnd();

	CryptoPP::RSAES_OAEP_SHA_Decryptor d(privateKey);

	CryptoPP::StringSource ss2(decompressed, true,
		new CryptoPP::PK_DecryptorFilter(rng, d,
			new CryptoPP::StringSink(recovered)
		) // PK_DecryptorFilter
	); // StringSource*/
		 
	return 0;
}