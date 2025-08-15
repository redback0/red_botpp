
#include <dpp/dpp.h>
#include <dpp/scheduled_event.h>
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

    if (victim_id == caller_id)
    {
        event.reply("You can't steal from yourself!");
        return;
    }

    GuildUser caller;
    GuildUser victim;

    {
        std::lock_guard lock{GuildUser::getMultiGuildUserReadLock()};

        caller = GuildUser(guild_id, caller_id);
        victim = GuildUser(guild_id, victim_id);
    }

    switch (caller.doSteal(victim))
    {
    case GuildUser::STEAL_SUCCESS:
        caller.saveChanges();
        victim.saveChanges();
        event.reply("Success! Stealing "s +
            std::to_string(caller.getStealAmount()) + " points from " +
            dpp::user::get_mention(victim_id));
    break;
    case GuildUser::STEAL_FAIL:
        caller.saveChanges();
        victim.saveChanges();
        event.reply("Oops, got caught. You paid "s +
            std::to_string(caller.getStealAmount()) + " points to " +
            dpp::user::get_mention(victim_id));
    break;
    case GuildUser::STEAL_NOTHING:
        caller.saveChanges();
        event.reply("You looked everywhere but couldn't find anything");
    break;
    case GuildUser::STEAL_BONUS:
        caller.saveChanges();
        victim.saveChanges();
        event.reply("How'd you manage this?");
    break;
    case GuildUser::STEAL_TO_POOR:
        event.reply("That user is too broke");
    break;
    case GuildUser::STEAL_TO_RECENT:
        event.reply("You stole too recently, wait "s +
            std::to_string(std::chrono::duration_cast<std::chrono::minutes>(
                caller.getStealTimeDiff()).count()) + " minutes");
    break;
    case GuildUser::STEAL_ERROR:
        event.reply("You broke something >:(");
        return;
    break;
    }
}