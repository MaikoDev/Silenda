#include "pch.h"

#include "SilendaServer.h"

int main(int argc, char** argv)
{
	printf("Generating server keys...\n");
	SilendaServer::NetPacker::GetInstance()->genKeys(4096);
	printf("Done! Ready to accept clients!\n");

	SilendaServer::NetServer::GetInstance()->start(54000);

	return 0;
}