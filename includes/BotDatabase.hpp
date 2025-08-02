
#pragma once

#include <sqlite3.h>

class BotDatabase
{
private:
    sqlite3*        _db;
    sqlite3_stmt*   _read_guilduser;
    sqlite3_stmt*   _insert_guilduser;
    sqlite3_stmt*   _update_guilduser;

    constexpr const static char _db_init[] =
    // "PRAGMA user_version = 1;" // use this for updating database
    "CREATE TABLE IF NOT EXISTS guilduser ("
        "guild_id TEXT,"
        "user_id TEXT,"
        "wallet INTEGER,"
        "bank INTEGER,"
        "last_daily INTEGER,"
        "last_steal INTEGER,"
        "PRIMARY KEY (guild_id, user_id)"
    ");";

public:
    BotDatabase();
    ~BotDatabase();

    // delete copies
    BotDatabase(BotDatabase&) = delete;
    BotDatabase& operator=(BotDatabase&) = delete;
};
