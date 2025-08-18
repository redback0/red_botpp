
#include <dpp/dpp.h>

void commandJoin(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    dpp::guild* guild = dpp::find_guild(event.command.guild_id);

    if (!guild->connect_member_voice(*event.owner,
        event.command.get_issuing_user().id))
    {
        event.reply("Error joining voice");
        return;
    }

    event.reply("Joining voice");
}