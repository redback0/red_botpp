
// #include "main.h"
#include <dpp/cluster.h>
#include <dpp/dispatcher.h>

void commandPing(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    event.reply("Pong!");
}
