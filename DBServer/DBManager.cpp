#include "DBManager.h"

DBManager::DBManager()
{
}


DBManager::~DBManager()
{
}

bool DBManager::Connect(const wchar_t * ip, const wchar_t * schema, const wchar_t * id, const wchar_t * pwd)
{
    SQLRETURN ret;

    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    SQLSetConnectAttrW(hDbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
    ret = SQLConnect(hDbc, (SQLWCHAR *)schema, SQL_NTS, (SQLWCHAR *)id, SQL_NTS, (SQLWCHAR *)pwd, SQL_NTS);

    HandleDiagnosticRecord(hDbc, SQL_HANDLE_DBC, ret);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool DBManager::Disconnect()
{
    SQLRETURN ret;

    ret = SQLDisconnect(hDbc);
    ret = SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    ret = SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    if (ret == SQL_SUCCESS)
        return true;
    else
        return false;
}

ResultMap* DBManager::Execute(const wchar_t* query)
{
    SQLHSTMT hStmt;

    SQLRETURN ret;
    ResultMap* result = new ResultMap();
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    ret = SQLExecDirectW(hStmt, (SQLWCHAR *)query, SQL_NTS);

    HandleDiagnosticRecord(hDbc, SQL_HANDLE_DBC, ret);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
    {
        SQLSMALLINT i = 1;
        SQLSMALLINT colnum;

        std::vector<Column> column;

        SQLNumResultCols(hStmt, &colnum);

        for (int i = 1; i <= colnum; ++i)
        {
            Column col;
            wchar_t name[64];

            col.order = i;
            SQLDescribeColW(hStmt, i, name, sizeof(name), nullptr, nullptr/*&col.coltype*/, &col.colsize, nullptr, nullptr);

            wcscpy_s(col.colname, name);

            column.push_back(col);
        }

        ret = SQLFetch(hStmt);

        if (ret == SQL_NO_DATA)
            return result;

        while (ret != SQL_NO_DATA)
        {
            for (int i = 0; i < colnum; ++i)
            {
                wchar_t buffer[256];

                short size = column[i].colsize;

                SQLGetData(hStmt, column[i].order, SQL_C_WCHAR, buffer, 256, nullptr);

                result->insert({ std::wstring(column[i].colname), std::wstring(buffer) });
            }

            if (ret == SQL_NO_DATA)
                return result;

            ret = SQLFetch(hStmt);

        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return result;
}

void DBManager::ExecuteDirect(const wchar_t * query)
{
    SQLHSTMT hStmt;
    SQLRETURN ret;
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    ret = SQLExecDirectW(hStmt, (SQLWCHAR *)query, SQL_NTS);

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    HandleDiagnosticRecord(hDbc, SQL_HANDLE_DBC, ret);
}

void DBManager::HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
    SQLSMALLINT iRec = 0;
    SQLINTEGER  iError;
    WCHAR       wszMessage[1000];
    WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];

    if (RetCode == SQL_INVALID_HANDLE)
    {
        fwprintf(stderr, L"Invalid handle!\n");
        return;
    }
    while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage, (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT *)NULL) == SQL_SUCCESS)
    { // Hide data truncated.. 
        if (wcsncmp(wszState, L"01004", 5))
        {
            fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
        }
    }
}