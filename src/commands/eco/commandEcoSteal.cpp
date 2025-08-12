
#include <dpp/dpp.h>
#include <dpp/snowflake.h>
#include <sstream>
#include <string>
#include "GuildUser.hpp"

void commandEcoSteal(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    dpp::snowflake victim_id =
        std::get<dpp::snowflake>(event.get_parameter("victim"));

    dpp::snowflake caller_id = event.command.member.user_id;
    dpp::snowflake guild_id = event.command.guild_id;

    GuildUser caller(guild_id.str(), caller_id.str());
    GuildUser victim(guild_id.str(), victim_id.str());

    switch (caller.doSteal(victim))
    {
    case GuildUser::SUCCESS:
        event.reply("Success! Stealing "s +
            std::to_string(caller.getStealAmount()) + " points from " +
            dpp::user::get_mention(victim_id));
    break;
    case GuildUser::FAIL:
        event.reply("Oops, got caught. You paid "s +
            std::to_string(caller.getStealAmount()) + " points to " +
            dpp::user::get_mention(victim_id));
    break;
    case GuildUser::NOTHING:
        event.reply("You looked everywhere but couldn't find anything");
    break;
    case GuildUser::BONUS:
        event.reply("How'd you manage this?");
    break;
    case GuildUser::TO_POOR:
        event.reply("That user is too broke");
    break;
    case GuildUser::TO_RECENT:
        event.reply("You stole too recently, wait "s +
            std::to_string(std::chrono::duration_cast<std::chrono::minutes>(
                caller.getStealTimeDiff()).count()) + " minutes");
    break;
    case GuildUser::ERROR:
        event.reply("You broke something >:(");
        return;
    break;
    }

    caller.saveChanges();
    victim.saveChanges();
}