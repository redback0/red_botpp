
// #include "main.h"
#include <dpp/dpp.h>
#include <exception>
#include "eco_command.h"

static const std::map<std::string, EcoCommand> ecoCommands = {
    {"daily", commandEcoDaily},
    {"balance", commandEcoBalance},
    {"steal", commandEcoSteal},
    {"deposit", commandEcoDeposit},
    {"withdraw", commandEcoWithdraw},
    {"leaderboard", commandEcoLeaderboard}
};

void commandEco(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    dpp::command_interaction cmd_data = event.command.get_command_interaction();
    auto& sub = cmd_data.options[0];
    try
    {
        auto& subcommand = ecoCommands.at(sub.name);
        subcommand.Execute(bot, event);
    }
    catch (const std::exception& e)
    {
        event.reply("EcoCommand failed");
        std::cerr << "Command failed: Likely command not found: "
            << e.what() << '\n';
    }
}

EcoCommand::EcoCommand()
    : _Execute()
{
}

EcoCommand::EcoCommand(eco_command_func_ptr Execute)
    : _Execute(Execute)
{
}

void EcoCommand::Execute(
    dpp::cluster& bot, const dpp::slashcommand_t& event) const
{
    try
    {
        if (_Execute)
        {
            _Execute(bot, event);
        }
        else
        {
            std::cout << "Unable to find eco subcommand: "
                << event.command.get_command_interaction().options[0].name
                << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        event.reply("EcoCommand failed");
        std::cerr << e.what() << '\n';
    }
}