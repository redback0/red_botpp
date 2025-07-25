
#include <dpp/dpp.h>
#include <cstdlib>
#include <iostream>

int main(int ac, char** av, char** ev)
{
    const char* token = std::getenv("BOT_TOKEN");

    if (!token)
    {
        std::cout << "Failed to get token" << std::endl;
        exit(1);
    }

    dpp::cluster bot(token);

    bot.on_log(dpp::utility::cout_logger());

    std::cout << "defining slashcommand callback" << std::endl;
    bot.on_slashcommand([](const dpp::slashcommand_t& event)
    {
        if (event.command.get_command_name() == "ping")
        {
            event.reply("Pong!");
        }
    });

    std::cout << "defining onready" << std::endl;
    bot.on_ready([&bot](const dpp::ready_t& event)
    {
        if (dpp::run_once<struct register_bot_commands>())
        {
            bot.global_command_create(
                dpp::slashcommand("ping", "Ping pong!", bot.me.id)
            );
        }
    });

    std::cout << "starting bot" << std::endl;
    bot.start(dpp::st_wait);
    return 0;
}