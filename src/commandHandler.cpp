
#include <string>
#include <map>
#include <dpp/dpp.h>
#include "main.h"
#include "command.h"

std::map<std::string, command_func_ptr> g_command;

void registerCommands(dpp::cluster& bot)
{
    // g_command["ping"] = commandPing;
    // g_command["test"] = commandTest;
    // g_command["repeat"] = commandRepeat;

    // bot.global_bulk_command_create({
    //     dpp::slashcommand("ping", "Ping pong!", bot.me.id),
    //     dpp::slashcommand("test", "This is a test", bot.me.id),
    //     dpp::slashcommand("repeat", "Repeat a message", bot.me.id)
    //         .add_option(dpp::command_option(
    //             dpp::command_option_type::co_string,
    //             "text", "String to repeat", true
    //         ))
    // });

    ADD_COMMAND("ping", "Ping pong!", commandPing,);
    ADD_COMMAND("test", "This is a test", commandTest,);
    ADD_COMMAND("repeat", "Repeat a message", commandRepeat,
        .add_option(dpp::command_option(
            dpp::command_option_type::co_string,
            "text", "String to repeat", true
        ))
    );
    ADD_COMMAND("eco", "A set of economy commands", commandEco,
        .add_option(
            dpp::command_option(
                dpp::command_option_type::co_sub_command,
                "daily", "Free points, daily", false
            )
        )
        .add_option(
            dpp::command_option(
                dpp::command_option_type::co_sub_command,
                "balance", "Check your account balance", false
            ).add_option(
                dpp::command_option(
                    dpp::command_option_type::co_user,
                    "user", "User who's balance to check", false
                )
            )
        )
    );
}

void commandHandler(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    auto command = g_command.at(event.command.get_command_name());
    if (command)
        command(bot, event);
    else
        std::cout << "Unable to find command"
            << event.command.get_command_name() << std::endl;
}

void commandNotFound(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    // purposely empty
}