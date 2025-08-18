
#include <dpp/dpp.h>

void commandLeave(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    dpp::voiceconn* v = event.from()->get_voice(event.command.guild_id);

    if (!v || !v->voiceclient || !v->voiceclient->is_ready())
    {
        event.reply("Not in a voice call");
        return;
    }

    event.from()->disconnect_voice(event.command.guild_id);

    event.reply("Leaving voice call");
}