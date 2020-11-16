#include <iostream>
#include <memory>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <ctime>

//#include "client/network/chat/cl_message.h"
#include "client/page/chatapp.h"

#include "client/page/chat.h"
#include "client/page/login.h"
#include "client/console/msghandler.h"
#include "client/render/renderer.h"
#include "client/console/mtc.h"
#include "cryptopp/osrng.h"
#include "cryptopp/rsa.h"
#include "cryptopp/filters.h"
#include "cryptopp/gzip.h"

#include "pods/buffers.h"
#include "pods/json.h"

using namespace render;

int main(int argc, char** argv)
{
	Silenda::ChatApp testApp({ 0, 0, 0 });
	testApp.TimeStr(time(0));

	ChatMessage testMessage = { "TestSender", UserLevel::user, time(0), "Hello World!" };
	pods::ResizableOutputBuffer out;
	pods::PrettyJsonSerializer<decltype(out)> serializer(out);
	if (serializer.save(testMessage) != pods::Error::NoError)
	{
		printf("Serialization Error\n");
	}

	std::string outputJson(out.data(), out.size());
	ChatMessage loadedMessage;

	pods::InputBuffer in(outputJson.data(), outputJson.size());
	pods::JsonDeserializer<decltype(in)> deserializer(in);
	if (deserializer.load(loadedMessage) != pods::Error::NoError)
	{
		printf("Deserialization Error\n");
	}

	Silenda::MTConsole* consolePtr = Silenda::MTConsole::GetInstance();
	Silenda::MsgHandler* handlerPtr = Silenda::MsgHandler::GetInstance();

	Renderer* rPtr = Renderer::GetInstance();

	Silenda::Page* login = new Silenda::LoginPage();
	Silenda::Page* chat = new Silenda::ChatPage();

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

	std::string cipher, compressed, decompressed, recovered;

	CryptoPP::RSAES_OAEP_SHA_Encryptor e(publicKey);

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

	CryptoPP::StringSource ss2(cipher, true,
		new CryptoPP::PK_DecryptorFilter(rng, d,
			new CryptoPP::StringSink(recovered)
		) // PK_DecryptorFilter
	); // StringSource*/
		 
	return 0;
}