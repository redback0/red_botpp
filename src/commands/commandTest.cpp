
#include <dpp/dpp.h>

void commandTest(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    event.reply("Test successful!");
}