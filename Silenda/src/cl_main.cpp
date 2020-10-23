#include <iostream>
#include <memory>
#include <io.h>
#include <fcntl.h>

#include "client\page\chat.h"
#include "client\page\login.h"
#include "client\console\msghandler.h"
#include "client\render\renderer.h"
#include "client\console\mtc.h"
#include "cryptopp\osrng.h"
#include "cryptopp\rsa.h"
#include "cryptopp\filters.h"
#include "cryptopp\gzip.h"

using namespace render;

class Board : public Renderable
{
public:
	Board() { test = new MeshFrame(5, 5, {0, 0, 0}); };
	~Board() { delete test; };

	inline void DebugDrawPoint(short x, short y) { test->DrawPoint(COLOR_GREEN, COLOR_BLACK, { x, y, 0 });
	}

	MeshFrame OnRender()
	{
		DebugDrawPoint(0, 0);
		DebugDrawPoint(0, 4);
		DebugDrawPoint(4, 4);
		DebugDrawPoint(4, 0);

		return *test;
	}
private:
	MeshFrame* test;
};

class SecBoard : public Renderable, public IObserver
{
public:
	SecBoard() { mesh = new MeshFrame(16, 5, { 10, 4, 0 }); };
	~SecBoard() { delete mesh; };

	inline void DebugDrawPoint(short x, short y) { mesh->DrawPoint(COLOR_PINK, COLOR_BLACK, { x, y, 0}); }

	void update(IObservable* src, const unsigned char controller = 0) override
	{
		if (controller == 0 || controller == 1)
		{
			Silenda::MTConsole* consolePtr = (Silenda::MTConsole*)src;
			inputString = consolePtr->GetMsgBuffer();
		}
	}

	MeshFrame OnRender()
	{
		mesh->reset();
		mesh->DrawRect(15, 2, COLOR_PINK, COLOR_BLACK, { 0, 0, 0 });
		mesh->DrawUText(inputString, COLOR_PINK, COLOR_BLACK, { 2, 1, 0 });

		return *mesh;
	}
private:
	MeshFrame* mesh;
	std::mutex mtx;
	std::wstring inputString;
};

int main(int argc, char** argv)
{
	Silenda::MTConsole* consolePtr = Silenda::MTConsole::GetInstance();
	Silenda::MsgHandler* handlerPtr = Silenda::MsgHandler::GetInstance();

	Renderer* rPtr = Renderer::GetInstance();

	Silenda::Page* login = new Silenda::LoginPage();
	Silenda::Page* chat = new Silenda::ChatPage();

	login->LinkPage("SilendaChat", chat);

	login->load();

	rPtr->init(100, 30);

	while (true)
	{
		rPtr->draw(Silenda::LoadedPage);
		rPtr->flush();
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