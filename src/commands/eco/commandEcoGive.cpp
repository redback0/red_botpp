
#include <dpp/dpp.h>
#include <mutex>
#include "GuildUser.hpp"

void commandEcoGive(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    dpp::snowflake user_id =
        std::get<dpp::snowflake>(event.get_parameter("user"));

    std::string account = std::get<std::string>(event.get_parameter("account"));
    long amount = std::get<long>(event.get_parameter("amount"));

    dpp::snowflake guild_id = event.command.guild_id;

    GuildUser user(guild_id, user_id);

    user.doGive(account, amount);
    user.saveChanges();

    event.reply("Points given");
}
