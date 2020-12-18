#include <iostream>
#include <locale>

#include  "WorldServer.h"

int main(int argc, char** argv)
{
    _wsetlocale(LC_ALL, L"kor");

    WorldServer server(5000, 4003);

    server.Init();
    Sleep(3000);
    server.Start();
    Sleep(3000);
    server.Connect("DB", "127.0.0.1", 4001);
    server.Connect("NPC", "127.0.0.1", 4002);

    while (true)
    {
        Sleep(1000);
    }
}