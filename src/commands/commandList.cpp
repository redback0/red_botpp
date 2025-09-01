
#include <dpp/dpp.h>
#include <sstream>
#include <string>

void commandList(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    std::ostringstream soundsString;

    for (auto& ai: std::filesystem::directory_iterator("./audio"))
    {
        if (ai.path().extension() != ".mp3")
            return;
        soundsString << '\n' << std::string(ai.path().stem());
    }

    dpp::embed embed;
    embed.title = "Sounds";
    embed.color = dpp::colors::red;
    embed.description = soundsString.str();

    event.reply(embed);
}