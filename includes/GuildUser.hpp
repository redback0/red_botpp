
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

    typedef std::chrono::time_point<std::chrono::system_clock> gu_tp_t;

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

    static std::mutex _map_lock;
    static lock_map_t _locks;

    static std::gamma_distribution<double> _steal_distr;

    const static wallet_int DEFAULT_WALLET = 2000ll;
    const static wallet_int DEFAULT_BANK = 0ll;
    constexpr const static gu_time_t DEFAULT_LAST_DAILY = 0s;
    constexpr const static gu_time_t DEFUALT_LAST_STEAL = 0s;

    const static wallet_int DAILY_AMOUNT = 500ll;

    const static int STEAL_DEF_SUCCESS_CHANCE = 50;
    const static int STEAL_FAIL_CHANCE = 50;
    const static int STEAL_NOTHING_CHANCE = 5;
    const static int STEAL_BONUS_CHANCE = 0; // unimplemented

public:

    enum StealResult
    {
        SUCCESS,
        FAIL,
        NOTHING,
        BONUS,
        TO_POOR,
        TO_RECENT,
        ERROR
    };

    GuildUser(std::string guild_id, std::string user_id);
    ~GuildUser() = default;

    sqlite3_int64 getWallet() const;
    sqlite3_int64 getBank() const;

    bool doDaily();
    StealResult doSteal(GuildUser& victim);

    void saveChanges();
};
