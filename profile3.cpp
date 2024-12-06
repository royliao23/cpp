#include <iostream>
#include <windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <vector>
#include <string>

using namespace std;

struct Person {
    wstring name;
    int age;
    wstring city;  // Changed from sex to city based on your table structure
};

void checkSQLResult(SQLRETURN retCode, SQLHANDLE handle, SQLSMALLINT handleType, const wstring &message) {
    if (retCode != SQL_SUCCESS && retCode != SQL_SUCCESS_WITH_INFO) {
        SQLWCHAR sqlState[1024];
        SQLWCHAR messageText[1024];
        SQLINTEGER nativeError;
        SQLSMALLINT textLength;
        SQLGetDiagRecW(handleType, handle, 1, sqlState, &nativeError, messageText, sizeof(messageText) / sizeof(SQLWCHAR), &textLength);
        wcerr << message << L": " << messageText << endl;
        exit(1);
    }
}

int main() {
    // Initialize ODBC handles
    SQLHANDLE sqlEnvHandle, sqlConnHandle, sqlStmtHandle;
    SQLRETURN retCode;

    // Allocate environment handle
    retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle);
    checkSQLResult(retCode, sqlEnvHandle, SQL_HANDLE_ENV, L"Error allocating environment handle");

    // Set ODBC version
    retCode = SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    checkSQLResult(retCode, sqlEnvHandle, SQL_HANDLE_ENV, L"Error setting ODBC version");

    // Allocate connection handle
    retCode = SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle);
    checkSQLResult(retCode, sqlConnHandle, SQL_HANDLE_DBC, L"Error allocating connection handle");

    // Connect to database
    SQLWCHAR connectionString[] = L"DRIVER={SQL Server};SERVER=localhost\\SQLEXPRESS;DATABASE=MyDatabase1;Trusted_Connection=yes;";
    retCode = SQLDriverConnectW(sqlConnHandle, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    checkSQLResult(retCode, sqlConnHandle, SQL_HANDLE_DBC, L"Error connecting to database");

    wcout << L"Connected to database successfully!" << endl;

    // Allocate statement handle
    retCode = SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle);
    checkSQLResult(retCode, sqlStmtHandle, SQL_HANDLE_STMT, L"Error allocating statement handle");

    // Create table (if not exists) - updated to match the correct columns in your table
    SQLWCHAR createTableQuery[] = 
        L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='Person' AND xtype='U') "
        L"CREATE TABLE Person (id INT IDENTITY(1,1) PRIMARY KEY, name NVARCHAR(50), age INT, city NVARCHAR(50));";
    retCode = SQLExecDirectW(sqlStmtHandle, createTableQuery, SQL_NTS);
    checkSQLResult(retCode, sqlStmtHandle, SQL_HANDLE_STMT, L"Error creating table");

    // Define the array of persons
    vector<Person> persons = {
        {L"Alice", 25, L"New York"},
        {L"Bob", 30, L"Los Angeles"},
        {L"Charlie", 28, L"Chicago"}
    };

    // Insert persons into database
    for (const auto &person : persons) {
        SQLWCHAR insertQuery[] = L"INSERT INTO Person (name, age, city) VALUES (?, ?, ?)";
        retCode = SQLPrepareW(sqlStmtHandle, insertQuery, SQL_NTS);
        checkSQLResult(retCode, sqlStmtHandle, SQL_HANDLE_STMT, L"Error preparing insert statement");

        // Bind parameters
        SQLBindParameter(sqlStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 50, 0, (SQLPOINTER)person.name.c_str(), 0, NULL);
        SQLBindParameter(sqlStmtHandle, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, (SQLPOINTER)&person.age, 0, NULL);
        SQLBindParameter(sqlStmtHandle, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 50, 0, (SQLPOINTER)person.city.c_str(), 0, NULL);

        // Execute the statement
        retCode = SQLExecute(sqlStmtHandle);
        checkSQLResult(retCode, sqlStmtHandle, SQL_HANDLE_STMT, L"Error inserting data");
        wcout << L"Inserted: " << person.name << L" from " << person.city << endl;
    }

    // Cleanup
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    SQLDisconnect(sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);

    wcout << L"Data inserted successfully and connection closed!" << endl;

    return 0;
}
