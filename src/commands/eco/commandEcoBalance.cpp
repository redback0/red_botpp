
#include <dpp/dpp.h>
#include <string>
#include <variant>
#include "GuildUser.hpp"

void commandEcoBalance(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    dpp::snowflake user_id;
    dpp::snowflake guild_id = event.command.guild_id;
    std::string guildMemberName;

    auto param = event.get_parameter("user");

    if (std::holds_alternative<dpp::snowflake>(param))
    {
        user_id = std::get<dpp::snowflake>(param);

        guildMemberName =
            event.command.get_resolved_member(user_id).get_nickname();

        if (guildMemberName.length() == 0)
        {
            const dpp::user& user = event.command.get_resolved_user(user_id);
            guildMemberName = user.global_name;
            if (guildMemberName.length() == 0)
                guildMemberName = user.username;
        }
    }
    else
    {
        user_id = event.command.member.user_id;

        guildMemberName =
            event.command.member.get_nickname();

        if (guildMemberName.length() == 0)
        {
            const dpp::user& user = event.command.get_issuing_user();
            guildMemberName = user.global_name;
            if (guildMemberName.length() == 0)
                guildMemberName = user.username;
        }
    }

    GuildUser user(guild_id.str(), user_id.str());

    dpp::embed msg;

    msg.set_color(dpp::colors::red);
    msg.set_title(guildMemberName + "'s Balances");
    msg.add_field("wallet", std::to_string(user.getWallet()));
    msg.add_field("bank", std::to_string(user.getBank()));

    event.reply(msg);
}