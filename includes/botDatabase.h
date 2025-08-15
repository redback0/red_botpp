
#pragma once

#include <sqlite3.h>
#include <string>

// "PRAGMA user_version = 1;" // use this for updating database
#define DB_INIT_SQL \
    "CREATE TABLE IF NOT EXISTS guilduser (" \
        "guild_id TEXT," \
        "user_id TEXT," \
        "wallet INTEGER," \
        "bank INTEGER," \
        "last_daily INTEGER," \
        "last_steal INTEGER," \
        "PRIMARY KEY (guild_id, user_id)" \
    ");"

#define DB_READ_GUILDUSER \
    "SELECT * FROM guilduser WHERE guild_id = ? AND user_id = ?"

#define DB_READ_GUILD \
    "SELECT * FROM guilduser WHERE guild_id = ?"

#define DB_INSERT_GUILDUSER \
    "INSERT INTO guilduser (" \
        "guild_id," \
        "user_id," \
        "wallet," \
        "bank," \
        "last_daily," \
        "last_steal" \
    ") VALUES ($guild_id, $user_id, $wallet, $bank, $last_daily, $last_steal);"

#define DB_UPDATE_GUILDUSER \
    "UPDATE guilduser SET " \
        "wallet = $wallet," \
        "bank = $bank," \
        "last_daily = $last_daily," \
        "last_steal = $last_steal " \
    "WHERE guild_id = $guild_id AND user_id = $user_id;"

void QuickBindParam(
    sqlite3_stmt* stmt, std::string param_name, std::string value);
void QuickBindParam(
    sqlite3_stmt *stmt, std::string param_name, sqlite3_int64 value);
void QuickBindParam(sqlite3_stmt *stmt, int param_index, std::string value);
