
#include <chrono>
#include <mutex>
#include <map>
#include <random>
#include <string>
#include <utility>
#include "botDatabase.h"

using namespace std::literals;

class GuildUser
{
public:
    typedef sqlite3_int64 wallet_int;

    typedef std::chrono::seconds gu_time_t;

    typedef std::chrono::time_point<std::chrono::system_clock,
        std::chrono::seconds> gu_tp_t;

    typedef std::string lock_map_key_t;
    typedef std::map<lock_map_key_t, std::mutex> lock_map_t;
private:

    std::unique_lock<std::mutex> _guilduser_lock;

    bool        _is_new_user;
    std::string _guild_id;
    std::string _user_id;
    wallet_int   _wallet;
    wallet_int   _bank;
    gu_time_t _last_daily;
    gu_time_t _last_steal;
    // later, inv

    union steal_result_info_t
    {
        wallet_int steal_amount;
        std::chrono::duration<long> steal_time_diff;
    } _steal_result_info;

    static std::mutex _map_lock;
    static lock_map_t _locks;

    static std::gamma_distribution<double> _steal_distr;

    const static wallet_int DEFAULT_WALLET = 2000ll;
    const static wallet_int DEFAULT_BANK = 0ll;
    constexpr const static gu_time_t DEFAULT_LAST_DAILY = 0s;
    constexpr const static gu_time_t DEFUALT_LAST_STEAL = 0s;

    const static wallet_int DAILY_AMOUNT = 500ll;

public:
    const static wallet_int DEP_WALLET_MIN = 2000ll;
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

    GuildUser(std::string guild_id, std::string user_id);
    ~GuildUser() = default;

    sqlite3_int64 getWallet() const;
    sqlite3_int64 getBank() const;

    std::chrono::duration<long> getStealTimeDiff() const;
    wallet_int getStealAmount() const;

    bool doDaily();
    StealResult doSteal(GuildUser& victim);
    DepositResult doDeposit(long& amount);

    void saveChanges();
};
