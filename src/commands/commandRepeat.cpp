
#include "main.h"

void commandRepeat(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    std::string text = std::get<std::string>(event.get_parameter("text"));

    event.reply(text);
}