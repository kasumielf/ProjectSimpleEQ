#pragma once
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

struct Column
{
	SQLSMALLINT order;
	SQLWCHAR colname[64];
	//SQLSMALLINT colnamelen;
	//SQLSMALLINT coltype;
	SQLULEN colsize;
	//SQLSMALLINT scale;
	//SQLSMALLINT nullable;
};

typedef std::unordered_map<std::wstring, std::wstring> ResultMap;
typedef std::unique_ptr<ResultMap> ResultMapPtr;

class DBManager
{
private:
	SQLHENV hEnv;
	SQLHDBC hDbc;

	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

public:
	DBManager();
	~DBManager();

	bool Connect(const wchar_t* ip, const wchar_t* schema, const wchar_t* id, const wchar_t* pwd);
	bool Disconnect();

	ResultMap* Execute(const wchar_t* query);
	void ExecuteDirect(const wchar_t* query);

};

