
#include <dpp/dpp.h>
#include "GuildUser.hpp"

void commandEcoDaily(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    dpp::snowflake user_id = event.command.member.user_id;
    dpp::snowflake guild_id = event.command.guild_id;

    GuildUser caller(guild_id.str(), user_id.str());

    if (caller.doDaily())
    {
        caller.saveChanges();
        event.reply(
            std::string("You got 500 points! Your total is now ")
            + std::to_string(caller.getWallet()));
    }
    else
    {
        event.reply(
            "Oops! You've already done this today, try again tomorrow!");
    }
}