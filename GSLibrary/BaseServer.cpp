#include "BaseServer.h"

BaseServer::~BaseServer()
{
	for (auto session : m_sessions)
	{
		session.second = nullptr;
		delete session.second;
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

	for (int j = 0; j < MAX_SESSION_ID_POOL; ++j)
	{
		m_id_lists.push_back(j);
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

	m_accept_thread = std::thread{ AcceptThreadFunc, this };

	if (use_timer)
	{
		m_timer_thread = std::thread{ TimerThreadFunc, this };
	}

	Logging(L"Server Start");
}

void BaseServer::Stop()
{
	closesocket(m_listen_sock);

	for (auto session : m_sessions)
	{
		closesocket(session.second->socket);
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
	if (m_internals_id.count(server_name) > 0)
	{
		Logging(L"Already connected to %ws", server_name);
	}
	else
	{
		int retry = 0;

		SOCKADDR_IN addr;

		ZeroMemory(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(ip);
		addr.sin_port = htons(port);
		Logging(L"Try connect to %S Server, %S, %d", server_name, ip, port);

		unsigned newId = -1;

		lock.lock();
		newId = m_id_lists.front();
		m_id_lists.pop_front();
		lock.unlock();

		InitSession(newId);
		m_sessions[newId]->socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		m_sessions[newId]->id = newId;
		m_sessions[newId]->use = true;
		m_sessions[newId]->received = 0;
		m_sessions[newId]->recvBytes = 0;
		ZeroMemory(&m_sessions[newId]->overlapped, sizeof(m_sessions[newId]->overlapped));
		m_sessions[newId]->overlapped.optype = IOCPOpType::OpRecv;
		m_sessions[newId]->overlapped.wsaBuf.buf = reinterpret_cast<CHAR *>(m_sessions[newId]->overlapped.iocp_buffer);
		m_sessions[newId]->overlapped.wsaBuf.len = sizeof(m_sessions[newId]->overlapped.iocp_buffer);

		CreateIoCompletionPort((HANDLE)m_sessions[newId]->socket, m_iocp_handle, newId, 0);

		DWORD recvBytes, flag = 0;

		while (true)
		{
			int retval;
			int err = WSAGetLastError();

			retval = WSAConnect(m_sessions[newId]->socket, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL);

			if (err == WSAEISCONN)
			{
				retval = WSARecv(m_sessions[newId]->socket, &m_sessions[newId]->overlapped.wsaBuf, 1, &recvBytes, &flag, &m_sessions[newId]->overlapped.wsaOverlapped, NULL);

				if (retval != 0)
				{
					int err_no = WSAGetLastError();
					ErrorDisplay("Error from SendToInternal ", err_no);
				}

				break;
			}

			Sleep(1000);
			retry++;

			if (++retry >= 10)
			{
				Logging(L"%S Internal Connection is fail", server_name);
				return;
			}
		}

		m_internals_id[server_name] = newId;

		Logging(L"%S Server Internal Connection is complete and allocated index is %d", server_name, newId);
	}
}

void BaseServer::CloseSocket(const int id)
{
	Lock();
	m_sessions[id]->id = -1;
	m_sessions[id]->use = false;

	closesocket(m_sessions[id]->socket);

	if(m_sessions.count(id) > 0)
		m_sessions.erase(id);
	Unlock();
}

void BaseServer::Send(const int id, unsigned char * packet)
{
	unsigned short psize = (unsigned short)packet[1];
	OverlappedEx *over = new OverlappedEx;
	ZeroMemory(&over->wsaOverlapped, sizeof(over->wsaOverlapped));
	over->optype = IOCPOpType::OpSend;
	over->caller_id = id;
	memcpy(over->iocp_buffer, packet, psize);
	over->wsaBuf.buf = reinterpret_cast<CHAR *>(over->iocp_buffer);
	over->wsaBuf.len = psize;

	DWORD send_flag = 0;
	DWORD send_byte;

	if(m_sessions[id] != nullptr)
		int ret = WSASend(m_sessions[id]->socket, &over->wsaBuf, 1, &send_byte, send_flag, &over->wsaOverlapped, NULL);
}

void BaseServer::SendToInternal(std::string name, unsigned char * packet)
{
	unsigned int internal_session_index = m_internals_id[name];
	unsigned short psize = (unsigned short)packet[1];

	OverlappedEx *over = new OverlappedEx;
	ZeroMemory(&over->wsaOverlapped, sizeof(over->wsaOverlapped));
	over->optype = IOCPOpType::OpSend;
	over->caller_id = internal_session_index;
	memcpy(over->iocp_buffer, packet, psize);
	over->wsaBuf.buf = reinterpret_cast<CHAR *>(over->iocp_buffer);
	over->wsaBuf.len = psize;

	DWORD send_flag = 0;
	int ret = WSASend(m_sessions[internal_session_index]->socket, &over->wsaBuf, 1, NULL, send_flag, &over->wsaOverlapped, NULL);

	if (ret != 0)
	{
		int err_no = WSAGetLastError();
		ErrorDisplay("Error from SendToInternal ", err_no);
	}
}

void BaseServer::InitSession(const int id)
{
	BaseSession* session = new BaseSession();

	ZeroMemory(&session->overlapped.wsaOverlapped, sizeof(session->overlapped.wsaOverlapped));
	session->recvBytes = 0;
	session->sendBytes = 0;
	session->overlapped.wsaBuf.buf = reinterpret_cast<CHAR*>(session->overlapped.iocp_buffer);
	session->overlapped.wsaBuf.len = MAX_PACKET_BUFFER_SIZE;
	session->use = false;

	m_sessions[id] = session;
}

void BaseServer::BroadCast(unsigned char * packet)
{
	auto iter_b = m_sessions.begin();
	auto iter_e = m_sessions.end();

	for (; iter_b!=iter_e; ++iter_b)
	{
		if(iter_b->second->use)
			Send(iter_b->first, packet);
	}
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

		unsigned int newId = -1;

		if (self->m_sessions.size() >= self->m_capacity) {
			self->Logging(L"User capacity is full");
			closesocket(clientSock);
			continue;
		}

		self->lock.lock();
		newId = self->m_id_lists.front();
		self->m_id_lists.pop_front();

		self->InitSession(newId);
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

		self->lock.unlock();

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
		OverlappedEx *over;

		int retval = GetQueuedCompletionStatus(self->m_iocp_handle, &io_size, (PULONG_PTR)&id, reinterpret_cast<LPOVERLAPPED*>(&over), INFINITE);

		if (io_size == 0)
		{
			self->CloseSocket(id);
			self->OnCloseSocket(id);
			continue;
		}

		switch (over->optype)
		{
			case IOCPOpType::OpSend:
			{
				if (io_size != over->wsaBuf.len) {
					self->CloseSocket(id);
				}
				delete over;

				break;
			}
			case IOCPOpType::OpRecv:
			{
				unsigned char* buf = self->m_sessions[id]->overlapped.iocp_buffer;
				unsigned char curr_packet_size = self->m_sessions[id]->recvBytes;
				unsigned char prev_packet_size = self->m_sessions[id]->received;

				while (io_size > 0)
				{
					if (curr_packet_size == 0)
					{
						curr_packet_size = buf[1];
					}
						
					//unsigned int required = cptr->overlapped.recvBytes - cptr->overlapped.received;

					if (io_size + prev_packet_size >= curr_packet_size)
					{
						unsigned char packet[MAX_PACKET_BUFFER_SIZE];

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
						prev_packet_size += (unsigned char)io_size;
						io_size = 0;

						break;
					}
				}

				self->m_sessions[id]->recvBytes = curr_packet_size;
				self->m_sessions[id]->received = prev_packet_size;
				DWORD flags = 0;
				retval = WSARecv(self->m_sessions[id]->socket, &self->m_sessions[id]->overlapped.wsaBuf, 1, NULL, &flags, &self->m_sessions[id]->overlapped.wsaOverlapped, NULL);

				break;
			}
			default:
			{
				if (self->m_iocp_events.count(over->optype) != 0)
				{
					self->m_iocp_events[over->optype](id, self);
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

void BaseServer::TimerThreadFunc(BaseServer * self)
{
	OverlappedEx overlapped;
	TimeEvent ev;

	while (true)
	{
		if (self->m_timerEvents.isEmpty() == false)
		{
			ev = self->m_timerEvents.Top();

			bool done = false;

			while (done == false)
			{
				auto now = std::chrono::high_resolution_clock::now();

				if (ev.time <= now)
				{
					overlapped.optype = ev.event.event_type;
					PostQueuedCompletionStatus(self->m_iocp_handle, 1, ev.event.provider, reinterpret_cast<LPOVERLAPPED>(&overlapped));
					self->m_timerEvents.Pop();
					done = true;
				}
			}
		}
		Sleep(100);
	}

}

void BaseServer::AttachIOCPEvent(IOCPOpType type, std::function<void(unsigned int, BaseServer*)> func)
{
	if (m_iocp_events.count(type) == 0)
	{
		m_iocp_events[type] = func;
	}
}

void BaseServer::ErrorDisplay(char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	Logging(L"%S\t%ws", msg, lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void BaseServer::PushTimerEvent(double duration, Event evt)
{
	m_timerEvents.Push(duration, evt);
}

