
#include <dpp/appcommand.h>
#include <exception>
#include <string>
#include <map>
#include <dpp/dpp.h>
#include "main.h"
#include "command.h"

std::map<std::string, Command> g_command;

void registerCommands(dpp::cluster& bot)
{
    INIT_COMMAND_LIST;
    ADD_COMMAND("ping", "Ping pong!", commandPing,);
    ADD_COMMAND("test", "This is a test", commandTest,);
    ADD_COMMAND("repeat", "Repeat a message", commandRepeat,
        .add_option(dpp::command_option(
            dpp::command_option_type::co_string,
            "text", "String to repeat", true
        ))
    );
    ADD_COMMAND("cringe", "How cringe is this", commandCringe,
        .add_option(dpp::command_option(
            dpp::command_option_type::co_string,
            "text", "😬", false
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
        .add_option(
            dpp::command_option(
                dpp::command_option_type::co_sub_command,
                "steal", "Steal from another user", false
            ).add_option(
                dpp::command_option(
                    dpp::command_option_type::co_user,
                    "victim", "The user to steal from", true
                )
            )
        )
    );
    REGISTER_COMMANDS;
}

void commandHandler(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    auto& command = g_command.at(event.command.get_command_name());
    command.Execute(bot, event);
}

Command::Command()
    : _Execute()
{
}

Command::Command(command_func_ptr Execute)
    : _Execute(Execute)
{
}

void Command::Execute(dpp::cluster& bot, const dpp::slashcommand_t& event) const
{
    try
    {
        if (_Execute)
            _Execute(bot, event);
        else
            std::cout << "Unable to find command: "
                << event.command.get_command_name() << std::endl;
    }
    catch (const std::exception& e)
    {
        event.reply("Command failed");
        std::cerr << e.what() << '\n';
    }
}
