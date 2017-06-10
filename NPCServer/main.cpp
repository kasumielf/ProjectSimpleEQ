#include <iostream>
#include <locale>

#include  "NPCServer.h"

int main(int argc, char** argv)
{
	_wsetlocale(LC_ALL, L"kor");

	NPCServer server(5000, 4002);

	server.Init();
	server.Start();
	Sleep(5000);
	server.Connect("World", "127.0.0.1", 4003);
	Sleep(10000);
	server.InitTemporaryNPCs();

	while (true)
	{
	}
}