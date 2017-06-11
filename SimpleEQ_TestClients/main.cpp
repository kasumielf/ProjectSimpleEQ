// SimpleEQ_TestClients.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "TestDaemon.h"
#include "../Common/ClientPacket.h"

int main()
{
	const int daemon_count = 1;

	TestDaemon dm(L"kasumielf");

	dm.Init();
	dm.Connect("test_daemon_001", "127.0.0.1", 4000);

	Sleep(5000);

	LOGIN login_packet;
	wchar_t id[12] = L"kasumielf";
	wcscpy_s(login_packet.ID_STR, id);

	dm.Send(0, reinterpret_cast<unsigned char*>(&login_packet));

	Sleep(60000);

	LOGOUT logout_packet;
	logout_packet.user_id = 1001;

	dm.Send(0, reinterpret_cast<unsigned char*>(&logout_packet));

	return 0;
}

