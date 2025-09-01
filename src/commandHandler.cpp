
#include <dpp/appcommand.h>
#include <exception>
#include <string>
#include <map>
#include <dpp/dpp.h>
#include "main.h"
#include "command.h"
#include "audioUtils.h"

std::map<std::string, Command> g_command;

void registerCommands(dpp::cluster& bot)
{
    using namespace std::literals;

    INIT_COMMAND_LIST;
    ADD_COMMAND("ping", "Ping pong!", commandPing,);
    ADD_COMMAND("test", "This is a test", commandTest,);
    ADD_COMMAND("repeat", "Repeat a message", commandRepeat,
        ADD_OPTION(dpp::command_option_type::co_string,
            "text", "String to repeat", true,
        )
    );
    ADD_COMMAND("cringe", "How cringe is this", commandCringe,
        ADD_OPTION(dpp::command_option_type::co_string,
            "text", "😬", false,
        )
    );
    ADD_COMMAND("eco", "A set of economy commands", commandEco,
        ADD_OPTION(dpp::command_option_type::co_sub_command,
            "daily", "Free points, daily", false,
        )
        ADD_OPTION(dpp::command_option_type::co_sub_command,
            "balance", "Check your account balance", false,
            ADD_OPTION(dpp::command_option_type::co_user,
                "user", "User who's balance to check", false,
            )
        )
        ADD_OPTION(dpp::command_option_type::co_sub_command,
            "steal", "Steal from another user", false,
            ADD_OPTION(dpp::command_option_type::co_user,
                "victim", "The user to steal from", true,
            )
        )
        ADD_OPTION(dpp::command_option_type::co_sub_command,
            "deposit", "Deposit from wallet to bank", false,
            ADD_OPTION(dpp::command_option_type::co_integer,
                "amount", "The amount to deposit. -1 for max", true,
            )
        )
        ADD_OPTION(dpp::command_option_type::co_sub_command,
            "withdraw", "Withdraw from the bank", false,
            ADD_OPTION(dpp::command_option_type::co_integer,
                "amount", "The amount to withdraw", true,
            )
        )
        ADD_OPTION(dpp::command_option_type::co_sub_command,
            "leaderboard", "See the leaderboard!", false,
        )
    );
    ADD_COMMAND("join", "Join voice call", commandJoin,);
    ADD_COMMAND("list", "List available sounds", commandList,);

    // this stops working past 25 files
    // dpp::command_option sound_option(
    //     dpp::command_option_type::co_string,
    //     "sound", "The sound to play", true
    // );
    // for (auto& ai: std::filesystem::directory_iterator("./audio"))
    // {
    //     bot.log(dpp::ll_info, "Found audio file: "s + ai.path().string());
    //     std::string s = ai.path().stem();
    //     sound_option.add_choice(dpp::command_option_choice(s, s));
    // }
    ADD_COMMAND("play", "Play a sound in the voice call", commandPlay,
        ADD_OPTION(dpp::command_option_type::co_string,
            "sound", "The sound to play", true,
        )
    );
    ADD_COMMAND("leave", "Leave current voice call", commandLeave,);
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
