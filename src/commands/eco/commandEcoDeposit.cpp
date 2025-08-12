
#include <dpp/dpp.h>
#include <string>
#include "GuildUser.hpp"

void commandEcoDeposit(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    long amount = std::get<long>(event.get_parameter("amount"));

    dpp::snowflake guild_id = event.command.guild_id;
    dpp::snowflake user_id = event.command.member.user_id;

    GuildUser caller(guild_id.str(), user_id.str());

    switch(caller.doDeposit(amount))
    {
    case GuildUser::DEP_SUCCESS:
        caller.saveChanges();
        event.reply("Deposited "s + std::to_string(amount) + " points");
    break;
    case GuildUser::DEP_WALLET_EMPTY:
        event.reply("Unable to deposit. Keep at least "s +
            std::to_string(GuildUser::DEP_WALLET_MIN));
    break;
    case GuildUser::DEP_BANK_FULL:
        event.reply("Your bank is full");
    break;
    case GuildUser::DEP_INVALID:
        event.reply("Number must be positive or -1");
    break;
    case GuildUser::DEP_ERROR:
        event.reply("You broke it >:(");
    break;
    }
}