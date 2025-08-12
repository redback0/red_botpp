
#include "botDatabase.h"
#include <stdexcept>
#include <string>

sqlite3* db;

void InitDB()
{
    int err;
    char* errmsg;

    err = sqlite3_open("red_botpp.db", &db);
    if (err != SQLITE_OK)
    {
        throw std::runtime_error(
            std::string("Unable to open database: ")
            + sqlite3_errmsg(db));
    }

    if (sqlite3_exec(db, DB_INIT_SQL, NULL, NULL, &errmsg))
    {
        throw std::runtime_error(
            std::string("Unable to initialize database")
            + errmsg);
    }
}

void QuickBindParam(
    sqlite3_stmt* stmt, std::string param_name, std::string value)
{
    int index = sqlite3_bind_parameter_index(stmt, param_name.c_str());
    if (index < 1)
        throw std::runtime_error(std::string("Unable to bind parameter ")
            + param_name + ": no such parameter");

    int err = sqlite3_bind_text(
        stmt, index, value.c_str(), value.length(), SQLITE_TRANSIENT);
    if (err != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Unable to bind parameter ")
            + param_name + ": Bind failed");
    }
}

void QuickBindParam(
    sqlite3_stmt* stmt, std::string param_name, sqlite3_int64 value)
{
    int index = sqlite3_bind_parameter_index(stmt, param_name.c_str());
    if (index < 1)
    {
        throw std::runtime_error(std::string("Unable to bind parameter ")
            + param_name + ": no such parameter");
    }

    int err = sqlite3_bind_int64(stmt, index, value);
    if (err != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Unable to bind parameter ")
            + param_name + ": Bind failed");
    }
}

void QuickBindParam(sqlite3_stmt* stmt, int param_index, std::string value)
{
    int err = sqlite3_bind_text(
        stmt, param_index, value.c_str(), value.length(), SQLITE_TRANSIENT);
    if (err != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Unable to bind parameter ")
            + std::to_string(param_index) + ": Bind failed");
    }
}
