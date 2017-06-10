#pragma once

#include <unordered_map>
#include "IOCPOpType.h"
#include <functional>
#include <vector>
#include <WinSock2.h>
#include <thread>
#include <mutex>

#include "BaseSession.h"
#include "TimerEventManager.h"

class BaseServer
{
protected:
	HANDLE m_iocp_handle;

private :
	short m_port;
	SOCKET m_listen_sock;
	int m_capacity;
	int m_current_user_count;

	std::list<unsigned int> m_id_lists;
	std::mutex lock;

	std::unordered_map < IOCPOpType, std::function<void(unsigned int, BaseServer*)> > m_iocp_events;
	std::thread m_accept_thread;
	std::thread m_timer_thread;
	std::vector<std::thread*> m_worker_thread;
	std::unordered_map<int, BaseSession*> m_sessions;
	std::unordered_map<std::string, int> m_internals_id;

protected:
	TimerEventManager m_timerEvents;

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
	void Send(const int id, unsigned char* packet);
	void SendToInternal(std::string, unsigned char* packet);
	void InitSession(const int id);

	void BroadCast(unsigned char* packet);

	static void AcceptThreadFunc(BaseServer* self);
	static void WorkerThreadFunc(BaseServer* self);
	static void TimerThreadFunc(BaseServer* self);

	void AttachIOCPEvent(IOCPOpType type, std::function<void(unsigned int, BaseServer*)> func);

	void Lock() { lock.lock(); }
	void Unlock() { lock.unlock(); }

	virtual void ProcessPacket(const int id, unsigned char* packet) = 0;
	virtual void OnCloseSocket(const int id) = 0;
	virtual void Logging(const wchar_t* msg, ...) = 0;
	void ErrorDisplay(char* msg, int err_no);
};

