
#include <chrono>
#include <mutex>
#include <map>
#include <random>
#include <sqlite3.h>
#include <string>
#include <utility>
#include "botDatabase.h"

using namespace std::literals;

class GuildUser
{
public:
    typedef sqlite3_int64 id_t;
    typedef sqlite3_int64 wallet_int;

    typedef std::chrono::seconds gu_time_t;

    typedef std::chrono::time_point<std::chrono::system_clock,
        std::chrono::seconds> gu_tp_t;

    typedef std::pair<int, int> lock_map_key_t;
    typedef std::map<lock_map_key_t, std::mutex> lock_map_t;
private:

    GuildUser(id_t guild_id, sqlite3_stmt* read_stmt);

    std::unique_lock<std::mutex> _guilduser_lock;

    bool        _is_new_user;
    id_t        _guild_id;
    id_t        _user_id;
    wallet_int  _wallet;
    wallet_int  _bank;
    gu_time_t   _last_daily;
    gu_time_t   _last_steal;

    union steal_result_info_t
    {
        wallet_int steal_amount;
        std::chrono::duration<long> steal_time_diff;
    } _steal_result_info;

    static std::mutex _map_lock;
    static lock_map_t _locks;
    static std::mutex _multiguilduser_lock;

    static std::gamma_distribution<double> _steal_distr;

    const static wallet_int DEFAULT_WALLET = 2000ll;
    const static wallet_int DEFAULT_BANK = 0ll;
    constexpr const static gu_time_t DEFAULT_LAST_DAILY = 0s;
    constexpr const static gu_time_t DEFUALT_LAST_STEAL = 0s;

    const static wallet_int DAILY_AMOUNT = 500ll;

public:
    const static wallet_int DEP_WALLET_MIN = 3000ll;
private:
    constexpr const static double DEP_BANK_TOTAL_RATIO = 4.0/5;

    const static int STEAL_DEF_SUCCESS_CHANCE = 50;
    const static int STEAL_FAIL_CHANCE = 50;
    const static int STEAL_NOTHING_CHANCE = 5;
    const static int STEAL_BONUS_CHANCE = 0; // unimplemented

public:

    enum DepositResult
    {
        DEP_SUCCESS,
        DEP_WALLET_EMPTY,
        DEP_BANK_FULL,
        DEP_INVALID,
        DEP_ERROR
    };

    enum WithdrawResult
    {
        WITH_SUCCESS,
        WITH_BANK_EMPTY,
        WITH_INVALID,
        WITH_ERROR
    };

    enum StealResult
    {
        STEAL_SUCCESS,
        STEAL_FAIL,
        STEAL_NOTHING,
        STEAL_BONUS,
        STEAL_TO_POOR,
        STEAL_TO_RECENT,
        STEAL_ERROR
    };

    GuildUser() = default;
    GuildUser(id_t guild_id, id_t user_id);
    ~GuildUser() = default;

    GuildUser(GuildUser&) = delete;
    GuildUser(GuildUser&&) = default;

    GuildUser& operator=(GuildUser&) = delete;
    GuildUser& operator=(GuildUser&&) = default;

    static std::vector<GuildUser> getRichestN(id_t guild_id, int n);
    static std::vector<GuildUser> getWholeGuild(id_t guild_id);

    id_t            getUserID() const;
    sqlite3_int64   getWallet() const;
    sqlite3_int64   getBank() const;
    sqlite3_int64   getNetWorth() const;

    static std::mutex& getMultiGuildUserReadLock();

    std::chrono::duration<long> getStealTimeDiff() const;
    wallet_int getStealAmount() const;

    bool doDaily();
    StealResult doSteal(GuildUser& victim);
    DepositResult doDeposit(long& amount);
    WithdrawResult doWithdraw(long& amount);
    void doGive(std::string account, long amount);

    void saveChanges();
};
