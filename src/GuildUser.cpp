
#include "GuildUser.hpp"
#include "botDatabase.h"
#include <algorithm>
#include <exception>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <unistd.h>
#include <random>

extern sqlite3* db;

std::gamma_distribution<double> GuildUser::_steal_distr(3, 5);

std::mutex GuildUser::_map_lock;
GuildUser::lock_map_t GuildUser::_locks;

GuildUser::GuildUser(std::string guild_id, std::string user_id)
{
    int err;
    sqlite3_stmt* read_stmt;

    _guild_id = guild_id;
    _user_id = user_id;

    {
        std::lock_guard<std::mutex> lock{_map_lock};

        // this needs to clear from the map as well
        _guilduser_lock = std::unique_lock(_locks[guild_id + "_" + user_id],
            std::defer_lock);
    }

    _guilduser_lock.lock();

    err = sqlite3_prepare_v2(db, DB_READ_GUILDUSER, -1, &read_stmt, NULL);
    if (err != SQLITE_OK)
    {
        throw (std::runtime_error(std::string(
            "Unable to prepare statement: ")
            + sqlite3_errmsg(db)));
    }

    err = sqlite3_bind_text(read_stmt, 1, guild_id.c_str(), guild_id.length(), SQLITE_STATIC);
    if (err != SQLITE_OK)
    {
        sqlite3_finalize(read_stmt);
        throw (std::runtime_error(std::string(
            "Unable to bind paramenter for guild_id: ")
            + sqlite3_errmsg(db)));
    }

    err = sqlite3_bind_text(read_stmt, 2, user_id.c_str(), user_id.length(), SQLITE_STATIC);
    if (err != SQLITE_OK)
    {
        sqlite3_finalize(read_stmt);
        throw (std::runtime_error(std::string(
            "Unable to bind paramenter for user_id: ")
            + sqlite3_errmsg(db)));
    }

    while ((err = sqlite3_step(read_stmt)) == SQLITE_BUSY)
    {
        // make this count x times then timeout
        sleep(1);
    }
    switch (err)
    {
    case SQLITE_ROW:
        _is_new_user = false;
        _wallet = sqlite3_column_int64(read_stmt, 2);
        _bank = sqlite3_column_int64(read_stmt, 3);
        // last_daily, last_steal
    break;
    case SQLITE_DONE:
        _is_new_user = true;
        _wallet = DEFAULT_WALLET;
        _bank = DEFAULT_BANK;
        // last_daily, last_steal
    break;
    }
    while (sqlite3_step(read_stmt) != SQLITE_DONE)
        std::cout <<
            "Found more than one identical guilduser in database" << std::endl;
    sqlite3_finalize(read_stmt);
}

sqlite3_int64 GuildUser::getWallet() const
{
    return _wallet;
}

sqlite3_int64 GuildUser::getBank() const
{
    return _bank;
}

bool GuildUser::doDaily()
{
    // check date :/

    _wallet += DAILY_AMOUNT;
    return true;
}

GuildUser::StealResult GuildUser::doSteal(GuildUser& victim)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    // check date

    // return StealResult::TO_RECENT;

    if (victim._wallet <= 2000)
    {
        return StealResult::TO_POOR;
    }

    std::discrete_distribution w({
        (double(victim._wallet) / this->_wallet) * STEAL_DEF_SUCCESS_CHANCE,
        STEAL_FAIL_CHANCE,
        STEAL_NOTHING_CHANCE,
        STEAL_BONUS_CHANCE
    });

    int result = w(gen);
    switch (result)
    {
        case 0: // steal
        {
            double percent = _steal_distr(gen);

            wallet_int steal_amount =
                std::min(victim._wallet, this->_wallet) * percent;

            victim._wallet -= steal_amount;
            this->_wallet += steal_amount;

            return StealResult::SUCCESS;
        }
        case 1: // fail/lose points
        {
            double percent = _steal_distr(gen);

            wallet_int steal_amount =
                std::min(victim._wallet, this->_wallet) * percent;

            victim._wallet += steal_amount;
            this->_wallet -= steal_amount;

            return StealResult::FAIL;
        }
        case 2: // nothing
        {
            return StealResult::NOTHING;
        }
        case 3: // bonus
        {
            // TODO: give this some functionality
            return StealResult::BONUS;
        }
    }

    return StealResult::ERROR;
}

void GuildUser::saveChanges()
{
    int err;
    sqlite3_stmt* write_stmt;

    if (_is_new_user)
    {
        err = sqlite3_prepare_v2(
            db, DB_INSERT_GUILDUSER, -1, &write_stmt, NULL);
        if (err != SQLITE_OK)
        {
            throw (std::runtime_error(std::string(
                "Unable to prepare statement: ")
                + sqlite3_errmsg(db)));
        }
    }
    else
    {
        err = sqlite3_prepare_v2(
            db, DB_UPDATE_GUILDUSER, -1, &write_stmt, NULL);
        if (err != SQLITE_OK)
        {
            throw (std::runtime_error(std::string(
                "Unable to prepare statement: ")
                + sqlite3_errmsg(db)));
        }
    }

    QuickBindParam(write_stmt, "$guild_id", _guild_id);
    QuickBindParam(write_stmt, "$user_id", _user_id);
    QuickBindParam(write_stmt, "$wallet", _wallet);
    QuickBindParam(write_stmt, "$bank", _wallet);
    // QuickBindParam(write_stmt, "$last_daily", _last_daily);
    // QuickBindParam(write_stmt, "$last_steal", _last_steal);

    err = sqlite3_step(write_stmt);
}