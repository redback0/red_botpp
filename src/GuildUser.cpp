
#include "GuildUser.hpp"
#include "botDatabase.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <mutex>
#include <sqlite3.h>
#include <stdexcept>
#include <unistd.h>
#include <random>

extern sqlite3* db;

std::gamma_distribution<double> GuildUser::_steal_distr(3, 0.04);

std::mutex GuildUser::_map_lock;
GuildUser::lock_map_t GuildUser::_locks;

std::mutex GuildUser::_multiguilduser_lock;

GuildUser::GuildUser(id_t guild_id, id_t user_id)
{
    int err;
    sqlite3_stmt* read_stmt;

    _guild_id = guild_id;
    _user_id = user_id;

    {
        std::lock_guard<std::mutex> lock{_map_lock};

        // this needs to clear from the map as well
        _guilduser_lock = std::unique_lock(
            _locks[std::pair(guild_id, user_id)], std::defer_lock);
    }

    _guilduser_lock.lock();

    err = sqlite3_prepare_v2(db, DB_READ_GUILDUSER, -1, &read_stmt, NULL);
    if (err != SQLITE_OK)
    {
        throw (std::runtime_error(std::string(
            "Unable to prepare statement: ")
            + sqlite3_errmsg(db)));
    }

    QuickBindParam(read_stmt, 1, guild_id);
    QuickBindParam(read_stmt, 2, user_id);

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
        _last_daily = gu_time_t(sqlite3_column_int(read_stmt, 4));
        _last_steal = gu_time_t(sqlite3_column_int(read_stmt, 5));
    break;
    case SQLITE_DONE:
        _is_new_user = true;
        _wallet = DEFAULT_WALLET;
        _bank = DEFAULT_BANK;
        _last_daily = DEFAULT_LAST_DAILY;
        _last_steal = DEFUALT_LAST_STEAL;
    break;
    default:
        sqlite3_finalize(read_stmt);
        throw std::runtime_error("Error reading from DB: "s +
            sqlite3_errmsg(db));
    break;
    }
    while (sqlite3_step(read_stmt) != SQLITE_DONE)
    {
        // note: this isn't the only possibility, I should come back to this
        std::cout <<
            "Found more than one identical guilduser in database" << std::endl;
    }
    sqlite3_finalize(read_stmt);
}

GuildUser::GuildUser(id_t guild_id, sqlite3_stmt* read_stmt)
{
    _is_new_user = false;
    _guild_id = guild_id;
    _user_id = sqlite3_column_int64(read_stmt, 1);

    {
        std::lock_guard<std::mutex> lock{_map_lock};

        _guilduser_lock = std::unique_lock(
            _locks[std::pair(guild_id, _user_id)], std::defer_lock);
    }

    _guilduser_lock.lock();

    _wallet = sqlite3_column_int64(read_stmt, 2);
    _bank = sqlite3_column_int64(read_stmt, 3);
    _last_daily = gu_time_t(sqlite3_column_int(read_stmt, 4));
    _last_steal = gu_time_t(sqlite3_column_int(read_stmt, 5));
}

std::vector<GuildUser> GuildUser::getRichestN(id_t guild_id, int n)
{
    std::vector<GuildUser> users;
    sqlite3_stmt* read_stmt;
    int err;

    std::lock_guard<std::mutex> lock(_multiguilduser_lock);

    err = sqlite3_prepare_v2(db, DB_READ_GUILD, -1, &read_stmt, NULL);
    if (err != SQLITE_OK)
    {
        throw (std::runtime_error("Unable to prepare statement: "s +
            sqlite3_errmsg(db)));
    }

    QuickBindParam(read_stmt, 1, guild_id);

    for (int i = 0;
        i < n && (err = sqlite3_step(read_stmt)) != SQLITE_DONE;
        i++)
    {
        switch (err)
        {
        case SQLITE_ROW:
            users.push_back(GuildUser(guild_id, read_stmt));
        break;
        case SQLITE_BUSY:
            sleep(1);
            continue;
        break;
        default:
            sqlite3_finalize(read_stmt);
            throw std::runtime_error("Step returned unexpected value");
        break;
        }
    }

    sqlite3_finalize(read_stmt);

    return users;
}

std::vector<GuildUser> GuildUser::getWholeGuild(id_t guild_id)
{
    std::vector<GuildUser> users;
    sqlite3_stmt* read_stmt;
    int err;

    std::lock_guard<std::mutex> lock(_multiguilduser_lock);

    err = sqlite3_prepare_v2(db, DB_READ_GUILD, -1, &read_stmt, NULL);
    if (err != SQLITE_OK)
    {
        throw (std::runtime_error("Unable to prepare statement: "s +
            sqlite3_errmsg(db)));
    }

    QuickBindParam(read_stmt, 1, guild_id);

    while ((err = sqlite3_step(read_stmt)) != SQLITE_DONE)
    {
        switch (err)
        {
        case SQLITE_ROW:
            users.push_back(GuildUser(guild_id, read_stmt));
        break;
        case SQLITE_BUSY:
            // make this more inteligent
            sleep(1);
        break;
        default:
            sqlite3_finalize(read_stmt);
            throw std::runtime_error("Step returned unexpected value");
        }
    }

    sqlite3_finalize(read_stmt);

    return users;
}

GuildUser::id_t GuildUser::getUserID() const
{
    return _user_id;
}

sqlite3_int64 GuildUser::getWallet() const
{
    return _wallet;
}

sqlite3_int64 GuildUser::getBank() const
{
    return _bank;
}

sqlite3_int64 GuildUser::getNetWorth() const
{
    return _wallet + _bank;
}

std::mutex& GuildUser::getMultiGuildUserReadLock()
{
    return _multiguilduser_lock;
}

std::chrono::duration<long> GuildUser::getStealTimeDiff() const
{
    return _steal_result_info.steal_time_diff;
}

GuildUser::wallet_int GuildUser::getStealAmount() const
{
    return _steal_result_info.steal_amount;
}

bool GuildUser::doDaily()
{
    gu_tp_t now;
    gu_tp_t last;

    now = std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now());
    last = gu_tp_t(_last_daily);

    now = std::chrono::floor<std::chrono::days>(now);
    last = std::chrono::floor<std::chrono::days>(last);

    if (last < now)
    {
        _last_daily = now.time_since_epoch();
        _wallet += DAILY_AMOUNT;
        return true;
    }
    else
    {
        return false;
    }
}

GuildUser::StealResult GuildUser::doSteal(GuildUser& victim)
{
    using namespace std::literals;

    std::random_device rd;
    std::mt19937 gen(rd());

    gu_tp_t now;
    gu_tp_t last;

    now = std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now());
    last = gu_tp_t(_last_steal);

    _steal_result_info.steal_time_diff = now - last;

    if (_steal_result_info.steal_time_diff <= 1h)
    {
        return StealResult::STEAL_TO_RECENT;
    }

    if (victim._wallet <= 2000)
    {
        return StealResult::STEAL_TO_POOR;
    }

    _last_steal = now.time_since_epoch();

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

            _steal_result_info.steal_amount =
                std::min(victim._wallet, this->_wallet) * percent;

            victim._wallet -= _steal_result_info.steal_amount;
            this->_wallet += _steal_result_info.steal_amount;
            
            _last_steal = now.time_since_epoch();
            return StealResult::STEAL_SUCCESS;
        }
        case 1: // fail/lose points
        {
            double percent = _steal_distr(gen);

            _steal_result_info.steal_amount =
                std::min(victim._wallet, this->_wallet) * percent;

            victim._wallet += _steal_result_info.steal_amount;
            this->_wallet -= _steal_result_info.steal_amount;

            _last_steal = now.time_since_epoch();
            return StealResult::STEAL_FAIL;
        }
        case 2: // nothing
        {
            _last_steal = now.time_since_epoch();
            return StealResult::STEAL_NOTHING;
        }
        case 3: // bonus
        {
            // TODO: give this some functionality
            _last_steal = now.time_since_epoch();
            return StealResult::STEAL_BONUS;
        }
    }

    return StealResult::STEAL_ERROR;
}

GuildUser::DepositResult GuildUser::doDeposit(long& amount)
{
    long max_move_from_wallet;
    long max_move_to_bank;

    max_move_from_wallet = _wallet - DEP_WALLET_MIN;
    max_move_to_bank = (_wallet + _bank) * DEP_BANK_TOTAL_RATIO - _bank;

    if (amount > 0)
    {
        if (amount > max_move_from_wallet)
            return DEP_WALLET_EMPTY;

        if (amount > max_move_to_bank)
            return DEP_BANK_FULL;
    }
    else if (amount == -1)
    {
        if (max_move_from_wallet <= 0)
            return DEP_WALLET_EMPTY;
        if (max_move_to_bank <= 0)
            return DEP_BANK_FULL;

        amount = std::min(max_move_from_wallet, max_move_to_bank);
    }
    else
        return DEP_INVALID;

    _wallet -= amount;
    _bank += amount;
    return DEP_SUCCESS;
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
    QuickBindParam(write_stmt, "$bank", _bank);
    QuickBindParam(write_stmt, "$last_daily", _last_daily.count());
    QuickBindParam(write_stmt, "$last_steal", _last_steal.count());

    err = sqlite3_step(write_stmt);
}