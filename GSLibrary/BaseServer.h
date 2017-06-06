#pragma once

#include <unordered_map>
#include "IOCPOpType.h"
#include <functional>
#include <vector>
#include <WinSock2.h>
#include <thread>

#include "BaseSession.h"

class BaseServer
{
private :
	short m_port;
	SOCKET m_listen_sock;
	HANDLE m_iocp_handle;
	int m_capacity;
	int m_current_user_count;

	std::unordered_map < IOCPOpType, std::function<void()> > m_iocp_events;
	std::thread m_accept_thread;
	std::thread m_timer_thread;
	std::vector<std::thread*> m_worker_thread;
	std::vector<BaseSession*> m_sessions;
	std::unordered_map<std::string, BaseSession*> m_internals;

public:
	BaseServer(const int capacity, const short port)  : m_port(port), m_capacity(capacity) {}
	~BaseServer();

	void Init();
	void Start();
	void Stop();
	void Bind();
	void Listen();
	void Connect(const char* server_name, const char* ip, const short port);
	void CloseSocket(const int id);
	void Send(const int id, char* packet);
	void SendToInternal(std::string, char* packet);

	static void AcceptThreadFunc(BaseServer* self);
	static void WorkerThreadFunc(BaseServer* self);
	static void TimerThreadFunc(BaseServer* self);

	void AttachIOCPEvent(IOCPOpType type, std::function<void()> func);

	virtual void ProcessPacket(const int id, char* packet) = 0;
	virtual void Logging(const wchar_t* msg, ...) = 0;
};

