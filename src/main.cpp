
#include <dpp/dpp.h>
#include <cstdlib>
#include <iostream>
#include <string>

std::string get_bot_token()
{
    std::ifstream env(".env");

    std::string line;

    while (std::getline(env, line).good())
    {
        if (line.starts_with("BOT_TOKEN="))
        {
            return line.substr(std::strlen("BOT_TOKEN="));
        }
    }
    return std::string();
}

int main(int ac, char** av)
{
    std::string token = get_bot_token();

    if (token.empty())
    {
        std::cout << "Failed to get token" << std::endl;
        exit(1);
    }

    dpp::cluster bot(token, dpp::i_default_intents | dpp::i_message_content);

    bot.on_log(dpp::utility::cout_logger());

    std::cout << "defining slashcommand callback" << std::endl;
    bot.on_slashcommand([](const dpp::slashcommand_t& event)
    {
        if (event.command.get_command_name() == "ping")
        {
            event.reply("Pong!");
        }
    });

    bot.on_message_create([&bot](const dpp::message_create_t& event)
    {
        if (event.msg.author.is_bot()) return;
        if (event.msg.content == "yo wuddup")
        {
            event.send("yo wuddup");
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