#include <iostream>
#include <locale>

#include  "AuthServer.h"

int main(int argc, char** argv)
{
    _wsetlocale(LC_ALL, L"kor");

    AuthServer server(5000, 4000);

    server.Init();
    Sleep(3000);
    server.Start();
    Sleep(3000);

    server.Connect("DB", "127.0.0.1", 4001);
    server.Connect("World", "127.0.0.1", 4003);

    while (true)
    {
        Sleep(1000);
    }
}