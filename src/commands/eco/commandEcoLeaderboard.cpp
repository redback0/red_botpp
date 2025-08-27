
#include <algorithm>
#include <dpp/dispatcher.h>
#include <dpp/exception.h>
#include <dpp/misc-enum.h>
#include <dpp/snowflake.h>
#include <exception>
#include <semaphore>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <dpp/dpp.h>
#include "GuildUser.hpp"
#include "dppUtils.h"

static void guildMemberCallback(
    dpp::cluster& bot,
    const dpp::confirmation_callback_t& event,
    dpp::snowflake guild_id,
    dpp::snowflake user_id,
    std::counting_semaphore<10>& sem,
    std::string& name)
{
    if (event.is_error())
    {
        // I don't know how to fix this :)
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

static std::vector<std::string> getNamesIndividually(
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

static void guildMembersCallback(
    dpp::cluster& bot,
    const dpp::confirmation_callback_t& result,
    dpp::snowflake& guild_id,
    std::map<dpp::snowflake, std::string>& names,
    std::binary_semaphore& sem)
{
    if (result.is_error())
    {
        // :(
    }
    else
    {
        const dpp::guild_member_map& members =
            result.get<dpp::guild_member_map>();

        for (auto& name: names)
        {
            if (members.contains(name.first))
            {
                name.second = getUserDisplayName(members.at(name.first));
            }
            else
            {
                // THIS WILL BE REMOVED WHEN RECURSION IS ADDED
                bot.log(dpp::ll_warning, "eco leaderboard: name not found");
                name.second = "MEMBER_NOT_FOUND";
            }
        }

        // int count = 0;
        // for (auto& name: names)
        // {
        //     if (!name.second.empty())
        //         count++;
        // }

        // // need this for servers bigger than 200, but I'm not worrying about
        // // that right now
        // if (count < names.size())
        // {
        //     bot.guild_get_members(guild_id, 200, last_user_id,
        //         [&bot, &guild_id, &names](const dpp::confirmation_callback_t& conf)
        //         {
        //             guildMembersCallback(bot, conf, guild_id, names);
        //         });
        // }
    }
    sem.release();
}

std::map<dpp::snowflake, std::string> getNamesTogether(
    dpp::cluster& bot,
    std::vector<GuildUser>& users,
    dpp::snowflake guild_id)
{
    std::binary_semaphore sem(1);
    std::map<dpp::snowflake, std::string> names;

    dpp::snowflake last_user_id = 0;

    for (auto& user : users)
    {
        names.insert(std::pair(user.getUserID(), ""s));
    }

    sem.acquire();
    bot.guild_get_members(guild_id, 200, last_user_id,
        [&bot, &guild_id, &names, &sem](const dpp::confirmation_callback_t& conf)
        {
            guildMembersCallback(bot, conf, guild_id, names, sem);
        });

    sem.acquire();

    return names;
}

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

    auto names = getNamesTogether(bot, users, guild_id);

    bool foundCaller = false;
    std::ostringstream ossEmbed;
    for (int i = 0; i < users.size() && i < names.size(); i++)
    {
        if (users[i].getUserID() == caller_id)
            foundCaller = true;

        if (i <= 10)
        {
            ossEmbed << '\n' << i << ". " << users[i].getNetWorth()
                << ", " << names.at(users[i].getUserID());
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
