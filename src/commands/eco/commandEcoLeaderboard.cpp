
#include <algorithm>
#include <dpp/dispatcher.h>
#include <dpp/snowflake.h>
#include <exception>
#include <semaphore>
#include <sstream>
#include <string>
#include <vector>
#include <dpp/dpp.h>
#include "GuildUser.hpp"
#include "dppUtils.h"

void commandEcoLeaderboard(dpp::cluster &bot, const dpp::slashcommand_t &event)
{
    std::counting_semaphore sem(10);

    dpp::snowflake guild_id = event.command.guild_id;
    dpp::snowflake caller_id = event.command.member.user_id;

    std::vector<GuildUser> users = GuildUser::getRichestN(guild_id, 10);

    std::vector<std::string> names(users.size());

    for (int i = 0; i < users.size(); ++i)
    {
        sem.acquire();
        bot.guild_get_member(guild_id, users[i].getUserID(),
            [&sem, &names, i](const dpp::confirmation_callback_t& event)
            {
                if (event.is_error())
                {
                    names[i] = "UNKNOWN_USER";
                }
                else
                {
                    const dpp::guild_member& member =
                        event.get<dpp::guild_member>();

                    names[i] = getUserDisplayName(member);
                }
                sem.release();
            });
    }

    int aquired = 0;
    while (aquired < 10)
    {
        sem.acquire();
        aquired++;
    }

    bool foundCaller = false;
    std::ostringstream ossEmbed;
    for (int i = 0; i < users.size() && i < names.size(); i++)
    {
        if (users[i].getUserID() == caller_id)
            foundCaller = true;

        if (i <= 10)
        {
            ossEmbed << '\n' << i << ". " << users[i].getNetWorth()
                << ", " << names[i];
        }

        // still need to confirm caller
    }

    dpp::embed embed;
    embed.title = "Leaderboard";
    embed.color = dpp::colors::red;
    embed.description = ossEmbed.str();

    event.reply(embed);
}