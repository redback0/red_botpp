
// #include "main.h"
#include <dpp/dpp.h>

void commandEco(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    // event.reply("called an eco command");

    dpp::command_interaction cmd_data = event.command.get_command_interaction();
    auto& sub = cmd_data.options[0];
    if (sub.name == "daily")
    {
        // will be moved to a different file, under src/eco/
        event.reply("You gained 500 points");
    }
    else if (sub.name == "balance")
    {
        event.reply("You probably have a balance mhm");
    }
}