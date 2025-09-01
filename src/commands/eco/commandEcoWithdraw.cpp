
#include <dpp/dpp.h>
#include <string>
#include "GuildUser.hpp"

void commandEcoWithdraw(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    long amount = std::get<long>(event.get_parameter("amount"));

    dpp::snowflake guild_id = event.command.guild_id;
    dpp::snowflake user_id = event.command.member.user_id;

    GuildUser caller(guild_id, user_id);

    switch(caller.doWithdraw(amount))
    {
    case GuildUser::WITH_SUCCESS:
        caller.saveChanges();
        event.reply("Withdrew "s + std::to_string(amount) + " points");
    break;
    case GuildUser::WITH_BANK_EMPTY:
        event.reply("Unable to withdraw. Not enough in bank");
    break;
    case GuildUser::WITH_INVALID:
        event.reply("Number must be positive");
    break;
    case GuildUser::WITH_ERROR:
        event.reply("You broke it >:(");
    break;
    }
}