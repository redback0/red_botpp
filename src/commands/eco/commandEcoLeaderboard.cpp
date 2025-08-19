
#include <algorithm>
#include <dpp/dispatcher.h>
#include <dpp/exception.h>
#include <dpp/snowflake.h>
#include <exception>
#include <semaphore>
#include <sstream>
#include <string>
#include <vector>
#include <dpp/dpp.h>
#include "GuildUser.hpp"
#include "dppUtils.h"

void guildMemberCallback(
    dpp::cluster& bot,
    const dpp::confirmation_callback_t& event,
    dpp::snowflake guild_id,
    dpp::snowflake user_id,
    std::counting_semaphore<10>& sem,
    std::string& name)
{
    if (event.is_error())
    {
        if (event.get_error().code == dpp::err_rate_limited)
        {
            bot.guild_get_member(guild_id, user_id,
                [&bot, &sem, guild_id, user_id, &name](const dpp::confirmation_callback_t& event)
                {
                    guildMemberCallback(
                        bot, event, guild_id, user_id, sem, name);
                });
            return;
        }

        name = "UNKNOWN_USER";
    }
    else
    {
        const dpp::guild_member& member =
            event.get<dpp::guild_member>();

        name = getUserDisplayName(member);
    }
    sem.release();
}

std::vector<std::string> getNamesIndividually(
    dpp::cluster& bot,
    std::vector<GuildUser>& users,
    dpp::snowflake guild_id)
{
    std::counting_semaphore<10> sem(10);

    std::vector<std::string> names(users.size());

    for (int i = 0; i < users.size(); ++i)
    {
        sem.acquire();
        bot.guild_get_member(guild_id, users[i].getUserID(),
            [&bot, &sem, guild_id, &users, &names, i](const dpp::confirmation_callback_t& event)
            {
                guildMemberCallback(
                    bot, event, guild_id, users[i].getUserID(), sem, names[i]);
            });
        usleep(50);
    }

    int aquired = 0;
    while (aquired < 10)
    {
        sem.acquire();
        aquired++;
    }

    return names;
}

// TODO: implement this function so we can get all leaderboard users in 1 request
// std::vector<std::string> getNamesTogether(
//     dpp::cluster& bot,
//     std::vector<GuildUser>& users,
//     dpp::snowflake guild_id)
// {
//     std::vector<std::string> names(users.size());

//     std::vector<dpp::snowflake> user_ids;

//     for (auto& user : users)
//     {
//         user_ids.push_back(user.getUserID());
//     }

//     dpp::snowflake last_user_id;

//     for (int num_found = 0; num_found < users.size();)
//     {
//         // I don't wanna do this right now :/
//         bot.guild_get_members(guild_id, )
//     }

//     return names;
// }

void commandEcoLeaderboard(dpp::cluster &bot, const dpp::slashcommand_t &event)
{
    dpp::snowflake guild_id = event.command.guild_id;
    dpp::snowflake caller_id = event.command.member.user_id;

    std::mutex lock;
    lock.lock();

    event.thinking(false, [&lock](const dpp::confirmation_callback_t&)
        {
            lock.unlock();
        });

    std::vector<GuildUser> users = GuildUser::getRichestN(guild_id, 10);

    std::vector<std::string> names = getNamesIndividually(bot, users, guild_id);

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

    lock.lock();
    event.edit_original_response(embed);
}
