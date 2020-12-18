#include <iostream>
#include <locale>

#include  "DBServer.h"

int main(int argc, char** argv)
{
    _wsetlocale(LC_ALL, L"kor");

    DBServer server(5000, 4001);

    server.Init();
    server.DBConnect(L"127.0.0.1", L"simple_eq", L"kasumielf", L"zktmal1#5");
    server.Start();

    while (true)
    {
        Sleep(1000);
    }
}