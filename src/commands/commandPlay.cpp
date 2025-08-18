
#include <dpp/dpp.h>
#include <mpg123.h>
#include <out123.h>
#include "audioUtils.h"

void commandPlay(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    dpp::voiceconn* v = event.from()->get_voice(event.command.guild_id);

    if (!v || !v->voiceclient || !v->voiceclient->is_ready())
    {
        event.reply("Make sure to do /join first");
        return;
    }

    std::string sound_name =
        std::get<std::string>(event.get_parameter("sound"));

    if (sound_name == "robot")
    {
        uint8_t* song = nullptr;
        size_t song_size = 0;
        std::ifstream input ("./audio/Robot.pcm",
            std::ios::in|std::ios::binary|std::ios::ate);

        if (input.is_open()) {
            song_size = input.tellg();
            song = new uint8_t[song_size];
            input.seekg (0, std::ios::beg);
            input.read ((char*)song, song_size);
            input.close();
        }

        v->voiceclient->send_audio_raw((uint16_t*)song, song_size);

        delete song;
    }
    else if (sound_name == "anime wow")
    {
        std::vector<uint8_t> pcmdata = getDecodedAudio("audio/" + sound_name + ".mp3");

        v->voiceclient->send_audio_raw(
            (uint16_t*)pcmdata.data(), pcmdata.size());
    }
    else
    {
        event.reply("No such sound");
        return;
    }

    event.reply("Successfully played audio");
}