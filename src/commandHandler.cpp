#include "main.h"

typedef void (*command_func)(dpp::cluster&, const dpp::slashcommand_t&);

std::map<std::string, command_func> g_command{
    {"ping", commandPing}
};


void commandHandler(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    auto command = g_command[event.command.get_command_name()];
    if (command)
        command(bot, event);
}

void commandPing(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    event.reply("Pong!");
}