
#include <filesystem>
#include <string>
#include <vector>
#include <dpp/dpp.h>
#include <mpg123.h>
#include <out123.h>
#include "audioUtils.h"

void commandPlay(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    event.thinking();
    dpp::voiceconn* v = event.from()->get_voice(event.command.guild_id);

    if (!v || !v->voiceclient || !v->voiceclient->is_ready())
    {
        event.edit_response("Make sure to do /join first");
        return;
    }

    std::string sound_name =
        std::get<std::string>(event.get_parameter("sound"));

    if (!std::filesystem::exists("audio/" + sound_name + ".mp3"))
    {
        event.edit_response("No such file");
        return;
    }

    std::vector<uint8_t> pcmdata =
        getDecodedAudio("audio/" + sound_name + ".mp3");

    v->voiceclient->send_audio_raw(
        (uint16_t*)pcmdata.data(), pcmdata.size());

    event.edit_response("Successfully played audio");
}