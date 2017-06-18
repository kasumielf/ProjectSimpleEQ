#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include <WinSock2.h>

#include "../GSLibrary/IOCPOpType.h"
#include "../Common/ClientPacket.h"
#include "../Common/ServerPacket.h"


HANDLE g_hiocp;

std::chrono::high_resolution_clock::time_point last_connect_time;

void error_display(char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	std::wcout << msg << L"[Error] " << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}

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
	int recvBytes;
	int received;
	std::chrono::high_resolution_clock::time_point last_move_time;
	bool connect;
	bool standby = false;
	unsigned char packet_buf[MAX_PACKET_BUFFER_SIZE];
};

std::vector<std::wstring> test_account;
std::vector<CLIENT> clients;
std::vector<std::thread*> worker_threads;
std::thread move_thread;

bool ready = false;

bool Send(short id, BasePacket* packet)
{
	int ptype = packet->PACKET_ID;
	int psize = packet->SIZE;
	OverlappedEx *over = new OverlappedEx;
	ZeroMemory(&over->wsaOverlapped, sizeof(over->wsaOverlapped));
	over->optype = IOCPOpType::OpSend;
	memcpy(over->iocp_buffer, packet, psize);
	over->wsaBuf.buf = reinterpret_cast<CHAR *>(over->iocp_buffer);
	over->wsaBuf.len = psize;

	int retval = WSASend(clients[id].socket, &over->wsaBuf, 1, NULL, 0, &over->wsaOverlapped, NULL);

	if (retval != 0)
	{
		error_display("send error", WSAGetLastError());
		return false;
	}
	else
	{
	}

	return true;
}

void ProcessPacket(const int id, unsigned char* packet)
{
	unsigned char packet_id = packet[0];

	if (packet_id > 0)
	{
		switch (packet_id)
		{
			case ID_LOGIN_FAIL:
			{
				std::cout << "Login Fail" << std::endl;
				break;
			}
			case ID_CONNECT_SERVER:
			{
				CONNECT_SERVER *res = reinterpret_cast<CONNECT_SERVER*>(packet);

				clients[id].connect = false;
				closesocket(clients[id].socket);

				clients[id].socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

				char ip[15];
				size_t sz;
				wcstombs_s(&sz, ip, 15, res->ip, _TRUNCATE);
				SOCKADDR_IN ServerAddr;
				ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
				ServerAddr.sin_family = AF_INET;
				ServerAddr.sin_port = htons(res->port);
				ServerAddr.sin_addr.s_addr = inet_addr(ip);

				int retval = WSAConnect(clients[id].socket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

				while (retval != 0)
				{
					Sleep(1000);

					if (retval != 0)
					{
						error_display("reconnect error", WSAGetLastError());
					}

					retval = WSAConnect(clients[id].socket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
				}

				DWORD recv_flag = 0;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(clients[id].socket), g_hiocp, id, 0);
				WSARecv(clients[id].socket, &clients[id].overlapped.wsaBuf, 1, NULL, &recv_flag, &clients[id].overlapped.wsaOverlapped, NULL);
				clients[id].connect = true;

				Request_Enter_GameWorld req;

				req.user_uid = res->user_uid;
				
				bool send_result = false;
				while (send_result == false)
				{
					send_result = Send(id, &req);
					Sleep(1);
				}

				break;
			}
			case ID_LOGIN_OK:
			{
				LOGIN_OK *res = reinterpret_cast<LOGIN_OK*>(packet);

				std::cout << "User " << res->username << " makes connection with World Server" << std::endl;

				clients[id].x = res->X_POS;
				clients[id].y = res->Y_POS;
				clients[id].standby = true;

				break;
			}
			case ID_Notify_Player_Move:
			{
				std::cout << "Player move" << std::endl;
				break;
			}
			default:
			{
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

void WorkerThread()
{
	int size = clients.size();

	while (true)
	{
		DWORD io_size;
		DWORD id;
		OverlappedEx *over;
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, (PULONG_PTR)&id, reinterpret_cast<LPOVERLAPPED *>(&over), INFINITE);

		if (io_size == 0)
		{
			Disconnect(id);
			continue;
		}
		else if (over->optype == IOCPOpType::OpSend)
		{
			if (io_size != over->wsaBuf.len)
			{
				Disconnect(id);
			}
			delete over;
		}
		else if (over->optype == IOCPOpType::OpRecv)
		{
			unsigned char* buf = clients[id].overlapped.iocp_buffer;
			unsigned char curr_packet_size = clients[id].recvBytes;
			unsigned char prev_packet_size = clients[id].received;

			while (io_size > 0)
			{
				if (curr_packet_size == 0)
					curr_packet_size = buf[1];

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
					prev_packet_size += (unsigned char)io_size;
					io_size = 0;

					break;
				}
			}

			clients[id].recvBytes = curr_packet_size;
			clients[id].received = prev_packet_size;

			DWORD flags = 0;
			int retval = WSARecv(clients[id].socket, &clients[id].overlapped.wsaBuf, 1, NULL, &flags, &clients[id].overlapped.wsaOverlapped, NULL);

			if (retval != 0)
			{
				error_display("recv error", WSAGetLastError());
			}
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
	for (int i = 1010; i < 6010; ++i)
	{
		std::wstring name = L"test" + std::to_wstring(i);

		test_account.push_back(name);
	}

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

	SYSTEM_INFO si;
	GetSystemInfo(&si);

	for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i)
	{
		worker_threads.push_back(new std::thread{ WorkerThread });
	}

	for (int i = 0; i < count; ++i)
	{
		clients[i].socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		SOCKADDR_IN ServerAddr;
		ZeroMemory(&ServerAddr, sizeof(ServerAddr));
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(4000);
		ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		clients[i].use = true;
		clients[i].received = 0;
		clients[i].recvBytes = 0;
		ZeroMemory(&clients[i].overlapped, sizeof(clients[i].overlapped));
		clients[i].overlapped.optype = IOCPOpType::OpRecv;
		clients[i].overlapped.wsaBuf.buf = reinterpret_cast<CHAR *>(clients[i].overlapped.iocp_buffer);
		clients[i].overlapped.wsaBuf.len = sizeof(clients[i].overlapped.iocp_buffer);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clients[i].socket), g_hiocp, i, 0);

		int Result = WSAConnect(clients[i].socket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

		DWORD recv_flag = 0;

		int retval = WSARecv(clients[i].socket, &clients[i].overlapped.wsaBuf, 1, NULL, &recv_flag, &clients[i].overlapped.wsaOverlapped, NULL);

		if (retval != 0)
		{
			error_display("recv error", WSAGetLastError());
		}

		clients[i].connect = true;
	}

	for (int i = 0; i < count; ++i)
	{
		if (clients[i].connect)
		{
			LOGIN login;
			wcscpy_s(login.ID_STR, test_account[i].c_str());
			Send(i, &login);
			Sleep(10);
		}
	}

	move_thread = std::thread{ MoveThread };

	while (true);

	for (int i = 0; i < count; ++i)
	{
		if(clients[i].connect == true)
			closesocket(clients[i].socket);
	}
}

