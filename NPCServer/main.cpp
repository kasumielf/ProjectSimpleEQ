#include <iostream>
#include <locale>

#include  "NPCServer.h"

int main(int argc, char** argv)
{
	_wsetlocale(LC_ALL, L"kor");

	NPCServer server(5000, 4002);

	server.Init();
	server.Start();

	while (true)
	{
	}
}