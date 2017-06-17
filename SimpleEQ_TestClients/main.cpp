

#include <iostream>

#include <vector>
#include <chrono>
#include <thread>
#include <WinSock2.h>

#include "../GSLibrary/IOCPOpType.h"
#include "../Common/ClientPacket.h"
#include "../Common/ServerPacket.h"

HANDLE g_hiocp;

std::chrono::high_resolution_clock::time_point last_connect_time;

struct OverlappedEx
{
	WSAOVERLAPPED	wsaOverlapped;
	WSABUF			wsaBuf;
	unsigned char	iocp_buffer[MAX_PACKET_BUFFER_SIZE];
	IOCPOpType		optype;
	int				caller_id;
};

struct CLIENT
{
public:
	OverlappedEx overlapped;
	SOCKET socket;
	int id;
	int x;
	int y;
	bool use;
	int sendBytes;
	std::chrono::high_resolution_clock::time_point last_move_time;
	bool connect;
	bool standby = false;
	OverlappedEx recv_over;
	unsigned char packet_buf[MAX_PACKET_BUFFER_SIZE];
	int prev_packet_data;
	int curr_packet_size;
};

std::vector<CLIENT> clients;
std::vector<std::thread*> worker_threads;
std::thread test_thread;


void ProcessPacket(const int id, unsigned char* packet)
{
	unsigned char packet_id = packet[0];
	// 만약 Login 요청에 대한 Res가 와서 정상 처리 되면 client[id]에 대해
	// Reconnect를 해서 월드 서버로 연결시킨다.
	// World 서버로 연결한 뒤, EnterWorld에 대한 Res가 왔을 때, 해당 클라를
	// Stand by 상태로 만든다.
	// MoveThread는 일단 시작은 시켜 놓고, 클라이언트에 대해 Standby가 true인
	// 클라이언트만 이동 패킷을 날린다.

	if (packet_id > 0)
	{
		switch (packet_id)
		{
			case ID_CONNECT_SERVER:
			{
				CONNECT_SERVER *res = reinterpret_cast<CONNECT_SERVER*>(packet);

				break;
			}
			case ID_LOGIN_OK:
			{
				LOGIN_OK *res = reinterpret_cast<LOGIN_OK*>(packet);
				break;
			}
		}
	}
}

void Disconnect(int id)
{
	closesocket(clients[id].socket);
	clients[id].connect = false;
	std::cout << "client is closed " << id << std::endl;
}

bool Send(short id, BasePacket* packet)
{
	int ptype = packet->PACKET_ID;
	int psize = packet->SIZE;
	OverlappedEx *over = new OverlappedEx;

	over->optype = IOCPOpType::OpSend;
	memcpy(over->iocp_buffer, packet, psize);
	ZeroMemory(&over->wsaOverlapped, sizeof(over->wsaOverlapped));
	over->wsaBuf.buf = reinterpret_cast<CHAR *>(over->iocp_buffer);
	over->wsaBuf.len = psize;

	int ret = WSASend(clients[id].socket, &over->wsaBuf, 1, NULL, 0, &over->wsaOverlapped, NULL);

	return true;
}

void MoveThread()
{
	int size = clients.size();

	while (true)
	{
		for (int i = 0; i < size; ++i)
		{
			if (clients[i].connect == false || clients[i].standby == false)
				continue;

			if (clients[i].last_move_time + std::chrono::high_resolution_clock::duration(std::chrono::milliseconds(1000)) > std::chrono::high_resolution_clock::now())
				continue;

			clients[i].last_move_time = std::chrono::high_resolution_clock::now();

			MOVE move;

			move.DIR = rand() % 4 * 2;
				
			Send(i, &move);
			Sleep(1);
		}
		Sleep(10);
	}

}

void TestThread()
{
	int size = clients.size();

	for (int i = 0; i < size; ++i)
	{
		LOGIN login;
		wcscpy_s(login.ID_STR, L"test1111");
		Send(i, &login);
		Sleep(100);
	}
}

void WorkerThread()
{
	while (true)
	{
		DWORD io_size;
		unsigned long long id;
		OverlappedEx *over;
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, &id, reinterpret_cast<LPWSAOVERLAPPED *>(&over), INFINITE);

		if (io_size == 0)
		{
			Disconnect(id);
			continue;
		}
		else if (over->optype == IOCPOpType::OpSend)
		{
			if (io_size != over->wsaBuf.len)
			{
				closesocket(clients[id].socket);
				clients[id].connect = false;
			}
			//delete over;
		}
		else if (over->optype == IOCPOpType::OpRecv)
		{
			unsigned char* buf = clients[id].overlapped.iocp_buffer;
			unsigned short curr_packet_size = clients[id].curr_packet_size;
			unsigned short prev_packet_size = clients[id].prev_packet_data;

			while (io_size > 0)
			{
				if (curr_packet_size == 0)
					curr_packet_size = (unsigned short)buf[1];

				//unsigned int required = cptr->overlapped.recvBytes - cptr->overlapped.received;

				if (io_size + prev_packet_size >= curr_packet_size)
				{
					unsigned char packet[MAX_PACKET_BUFFER_SIZE];

					memcpy(packet, clients[id].packet_buf, prev_packet_size);
					memcpy(packet + prev_packet_size, buf, curr_packet_size - prev_packet_size);
					ProcessPacket(id, buf);

					io_size -= curr_packet_size - prev_packet_size;
					buf += curr_packet_size - prev_packet_size;
					curr_packet_size = 0;
					prev_packet_size = 0;
				}
				else
				{
					memcpy(clients[id].packet_buf + prev_packet_size, buf, io_size);
					prev_packet_size += (unsigned short)io_size;
					io_size = 0;

					break;
				}
			}

			clients[id].curr_packet_size = curr_packet_size;
			clients[id].prev_packet_data = prev_packet_size;
			DWORD flags = 0;
			int retval = WSARecv(clients[id].socket, &clients[id].overlapped.wsaBuf, 1, NULL, &flags, &clients[id].overlapped.wsaOverlapped, NULL);


		}
		else
		{
			std::cout << "Unknown event is received!" << std::endl;
			Disconnect(id);
		}
	}
}

int main()
{
	int count = 0;
	std::cout << "Input Test Client Amount : " << std::endl;
	std::cin >> count;

	for (int i = 0; i < count; ++i)
	{
		CLIENT cl;
		cl.connect = false;
		clients.push_back(cl);
	}

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	for (int i = 0; i < count; ++i)
	{
		clients[i].socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		SOCKADDR_IN ServerAddr;
		ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(4000);
		ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		int Result = WSAConnect(clients[i].socket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

		clients[i].curr_packet_size = 0;
		clients[i].prev_packet_data = 0;
		ZeroMemory(&clients[i].recv_over, sizeof(clients[i].recv_over));
		clients[i].recv_over.optype = IOCPOpType::OpRecv;
		clients[i].recv_over.wsaBuf.buf = reinterpret_cast<CHAR *>(clients[i].recv_over.iocp_buffer);
		clients[i].recv_over.wsaBuf.len = sizeof(clients[i].recv_over.iocp_buffer);

		DWORD recv_flag = 0;
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clients[i].socket), g_hiocp, i, 0);
		WSARecv(clients[i].socket, &clients[i].recv_over.wsaBuf, 1, NULL, &recv_flag, &clients[i].recv_over.wsaOverlapped, NULL);
		clients[i].connect = true;
	}

	for (int i = 0; i < 4; ++i)
		worker_threads.push_back(new std::thread{ WorkerThread });

	test_thread = std::thread{ TestThread };

	test_thread.join();

	for (auto pth : worker_threads)
	{
		pth->join();
		delete pth;
	}
}

