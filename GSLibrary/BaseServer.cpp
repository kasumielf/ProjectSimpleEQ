#include "BaseServer.h"

BaseServer::~BaseServer()
{
	for (auto session : m_sessions)
	{
		session = nullptr;
		delete session;
	}

	m_sessions.clear();
}

void BaseServer::Init()
{

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		Logging(L"WSAStartup Failed");
		return;
	}

	m_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	if (m_iocp_handle == NULL)
	{
		Logging(L"CreateIoCompletionPort Failed");
		return;
	}

	for (int i = 0; i < m_capacity; ++i)
	{
		BaseSession* session = new BaseSession();
		
		ZeroMemory(&session->overlapped.wsaOverlapped, sizeof(session->overlapped.wsaOverlapped));
		session->recvBytes = 0;
		session->sendBytes = 0;
		session->overlapped.wsaBuf.buf = session->overlapped.iocp_buffer;
		session->overlapped.wsaBuf.len = MAX_PACKET_BUFFER_SIZE;
		session->use = false;

		m_sessions.push_back(session);
	}

	m_listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (m_listen_sock == INVALID_SOCKET)
	{
		Logging(L"Invalid Listen Socket is created!");
		return;
	}

	Logging(L"Init Complete");

	return;

}

void BaseServer::Start()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i)
	{
		std::thread *wth = new std::thread{ WorkerThreadFunc, this };
		m_worker_thread.push_back(wth);
	}

	m_accept_thread= std::thread{ AcceptThreadFunc, this };

	Logging(L"Server Start");

	m_accept_thread.join();

	for (auto th : m_worker_thread) {
		th->join();
		delete th;
	}
}

void BaseServer::Stop()
{
	closesocket(m_listen_sock);

	for (int i = 0; i<m_capacity; ++i)
	{
		closesocket(m_sessions[i]->socket);
	}

	WSACleanup();

	Logging(L"Server Stop");
}

void BaseServer::Bind()
{
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(m_port);

	int retval = bind(m_listen_sock, (SOCKADDR*)&addr, sizeof(addr));

	if (retval == SOCKET_ERROR)
	{
		Logging(L"Bind Failed");
		return;
	}

	Logging(L"Bind Complete");
}

void BaseServer::Listen()
{
	int retval = listen(m_listen_sock, 100);

	if (retval == SOCKET_ERROR)
	{
		Logging(L"Listen Failed");
		return;
	}

	Logging(L"Listen Complete");
	return;

}

void BaseServer::Connect(const char * server_name, const char * ip, const short port)
{
	if (m_internals.count(server_name) > 0)
	{
		Logging(L"Already connected to %ws", server_name);
	}
	else
	{
		int retry = 0;

		BaseSession *int_session = new BaseSession();

		int_session->socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		SOCKADDR_IN addr;

		ZeroMemory(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(ip);
		addr.sin_port = htons(port);
		Logging(L"Try connect to %S, %S, %d", server_name, ip, port);

		while (true)
		{
			int retval;
			int err = WSAGetLastError();

			retval = WSAConnect(int_session->socket, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL);

			if (err == WSAEISCONN)
				break;

			Sleep(1000);
			retry++;

			if (++retry >= 10)
			{
				Logging(L"%S Internal Connection is fail", server_name);
				return;
			}
		}

		Logging(L"%S Internal Connection is complete", server_name);
		m_internals[server_name] = int_session;
	}
}

void BaseServer::CloseSocket(const int id)
{
	m_sessions[id]->id = -1;
	m_sessions[id]->use = false;

	if (m_sessions[id]->sendBytes > 0)
	{
		// 남은 데이터 있으면 전송하기
	}

	closesocket(m_sessions[id]->socket);
}

void BaseServer::Send(const int id, char * packet)
{
	int retval = send(m_sessions[id]->socket, packet, packet[1], 0);
}

void BaseServer::SendToInternal(std::string name, char * packet)
{
	int retval = send(m_internals[name]->socket, packet, packet[1], 0);
}

void BaseServer::AcceptThreadFunc(BaseServer* self)
{
	self->Bind();
	self->Listen();

	SOCKET clientSock;
	SOCKADDR_IN clientAddr;
	DWORD recvBytes, flag;
	int addrlen;

	self->Logging(L"Accept Start");

	while (true)
	{
		addrlen = sizeof(clientAddr);

		clientSock = accept(self->m_listen_sock, (SOCKADDR*)&clientAddr, &addrlen);

		if (clientSock == INVALID_SOCKET)
		{
			self->Logging(L"Invalid Socket is created!");
			continue;
		}

		self->Logging(L"Client Accept from %S:%d", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		int newId = -1;

		for (int i = 0; i <self->m_capacity; ++i)
		{
			if (self->m_sessions[i]->use == false)
			{
				newId = i;
				break;
			}
		}

		if (newId == -1) {
			self->Logging(L"User capacity is full");
			closesocket(clientSock);
			continue;
		}

		self->m_sessions[newId]->socket = clientSock;
		self->m_sessions[newId]->id = newId;
		self->m_sessions[newId]->use = true;
		self->m_sessions[newId]->received = 0;
		self->m_sessions[newId]->recvBytes = 0;
		ZeroMemory(&self->m_sessions[newId]->overlapped, sizeof(self->m_sessions[newId]->overlapped));
		self->m_sessions[newId]->overlapped.optype = IOCPOpType::OpRecv;
		self->m_sessions[newId]->overlapped.wsaBuf.buf = reinterpret_cast<CHAR *>(self->m_sessions[newId]->overlapped.iocp_buffer);
		self->m_sessions[newId]->overlapped.wsaBuf.len = sizeof(self->m_sessions[newId]->overlapped.iocp_buffer);

		CreateIoCompletionPort((HANDLE)clientSock, self->m_iocp_handle, newId, 0);

		flag = 0;

		int retval = WSARecv(clientSock, &self->m_sessions[newId]->overlapped.wsaBuf, 1, &recvBytes, &flag, &self->m_sessions[newId]->overlapped.wsaOverlapped, NULL);

		if (retval == SOCKET_ERROR)
		{
			int d = WSAGetLastError();
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				self->Logging(L"recv Failed");
			}
			continue;
		}
	}

}

void BaseServer::WorkerThreadFunc(BaseServer* self)
{
	while (true)
	{
		DWORD io_size;
		DWORD id;
		//Client *cptr = new Client;
		OverlappedEx *over;

		int retval = GetQueuedCompletionStatus(self->m_iocp_handle, &io_size, (PULONG_PTR)&id, (LPOVERLAPPED*)&over, INFINITE);

		switch (over->optype)
		{
			case IOCPOpType::OpSend:
			{
				if (io_size != over->wsaBuf.len) {
					self->Logging(L"Send Incomplete Error!\n");

					self->CloseSocket(id);
				}
				delete over;

				break;
			}
			case IOCPOpType::OpRecv:
			{
				char* buf = self->m_sessions[id]->overlapped.iocp_buffer;
				unsigned short curr_packet_size = self->m_sessions[id]->recvBytes;
				unsigned short prev_packet_size = self->m_sessions[id]->received;

				self->Logging(L"Recv from client : %d", id);
				while (io_size)
				{
					if (curr_packet_size == 0)
						curr_packet_size = (short)buf[0];

					//unsigned int required = cptr->overlapped.recvBytes - cptr->overlapped.received;

					if (io_size + prev_packet_size >= curr_packet_size)
					{
						char packet[MAX_PACKET_BUFFER_SIZE];

						memcpy(packet, self->m_sessions[id]->packet_buffer, prev_packet_size);
						memcpy(packet + prev_packet_size, buf, curr_packet_size - prev_packet_size);
						self->ProcessPacket(id, buf);

						io_size -= curr_packet_size - prev_packet_size;
						buf += curr_packet_size - prev_packet_size;
						curr_packet_size = 0;
						prev_packet_size = 0;
					}
					else
					{
						memcpy(self->m_sessions[id]->packet_buffer + prev_packet_size, buf, io_size);
						prev_packet_size += (unsigned short)io_size;
						io_size = 0;

						break;
					}
				}

				self->m_sessions[id]->recvBytes = curr_packet_size;
				self->m_sessions[id]->received = prev_packet_size;
				DWORD flags = 0;
				retval = WSARecv(self->m_sessions[id]->socket, &self->m_sessions[id]->overlapped.wsaBuf, 1, NULL, &flags, &self->m_sessions[id]->overlapped.wsaOverlapped, NULL);

				delete over;

				break;
			}
			default:
			{
				if (self->m_iocp_events.count(over->optype) != 0)
				{
					self->m_iocp_events[over->optype]();
				}
				else
				{
					self->Logging(L"Unknown IOCP Event!");
					self->CloseSocket(id);
				}
			}
		}
	}
}

void BaseServer::AttachIOCPEvent(IOCPOpType type, std::function<void()> func)
{
	if (m_iocp_events.count(type) == 0)
	{
		m_iocp_events[type] = func;
	}
	else
	{
		Logging(L"Already attached Event! (type : %S", type);
	}

}


