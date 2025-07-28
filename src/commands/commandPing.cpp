
// #include "main.h"
#include <dpp/dpp.h>

void commandPing(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    event.reply("Pong!");
}
