
#pragma once

#include <dpp/dpp.h>

typedef void (*eco_command_func_ptr)(dpp::cluster&, const dpp::slashcommand_t&);

class EcoCommand
{
private:
    eco_command_func_ptr    _Execute;

public:
    EcoCommand();
    EcoCommand(eco_command_func_ptr Execute);

    void Execute(dpp::cluster& bot, const dpp::slashcommand_t& event) const;
};

class EcoAdminCommand : public EcoCommand
{
public:
    EcoAdminCommand(eco_command_func_ptr Execute) : EcoCommand(Execute) {};
};


void commandEcoDaily(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandEcoBalance(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandEcoSteal(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandEcoDeposit(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandEcoWithdraw(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandEcoLeaderboard(dpp::cluster& bot, const dpp::slashcommand_t& event);

// admin commands
void commandEcoGive(dpp::cluster& bot, const dpp::slashcommand_t& event);