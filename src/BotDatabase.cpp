
#include "BotDatabase.hpp"
#include "main.h"
#include <stdexcept>

struct GuildUser
{
    std::string guild_id;
    std::string user_id;
    long long   wallet;
    long long   bank;
    //  last_daily;
    //  last_steal;
    // later, inv
};

BotDatabase::BotDatabase()
    : _db()
    , _read_guilduser()
    , _insert_guilduser()
    , _update_guilduser()
{
    int err;
    char* errmsg;

    err = sqlite3_open("red_botpp.db", &_db);
    if (err != SQLITE_OK)
        throw(std::runtime_error(
            std::string("Unable to open database: ")
            + sqlite3_errmsg(_db)));

    if (sqlite3_exec(_db, _db_init, NULL, NULL, &errmsg))
        throw(std::runtime_error(
            std::string("Unable it initialize database")
            + errmsg));


    err = sqlite3_prepare_v2(_db,
        "SELECT * FROM guilduser WHERE guild_id == ? AND user_id == ?",
        -1, &_read_guilduser, NULL);
    if (err != SQLITE_OK)
        throw(std::runtime_error(
            std::string("Unable to create read query: ")
            + sqlite3_errmsg(_db)));

    err = sqlite3_prepare_v2(_db,
        "INSERT INTO guilduser ("
            "guild_id,"
            "user_id,"
            "wallet,"
            "bank,"
            "last_daily,"
            "last_steal"
        ") VALUES (?, ?, ?, ?, ?, ?)",
        -1, & _insert_guilduser, NULL);
    if (err != SQLITE_OK)
        throw(std::runtime_error(
            std::string("Unable to create insert query: ")
            + sqlite3_errmsg(_db)));

    err = sqlite3_prepare_v2(_db,
        "UPDATE guilduser SET "
            "wallet = ?,"
            "bank = ?,"
            "last_daily = ?,"
            "last_steal = ? "
        "WHERE guild_id = ? AND user_id = ?;",
        -1, &_update_guilduser, NULL);
    if (err != SQLITE_OK)
        throw(std::runtime_error(
            std::string("Unable to create update query: ")
            + sqlite3_errmsg(_db)));
}

BotDatabase::~BotDatabase()
{
    sqlite3_finalize(_read_guilduser);
    sqlite3_finalize(_insert_guilduser);
    sqlite3_finalize(_update_guilduser);
    sqlite3_close(_db);
}