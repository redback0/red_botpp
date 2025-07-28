
#pragma once

#include <dpp/dpp.h>

#define ADD_COMMAND(name, description, function, params) \
    g_command[( name )] = ( function ); \
    bot.global_command_create( \
        dpp::slashcommand(( name ), ( description ), bot.me.id) params \
    )

typedef void (*command_func_ptr)(dpp::cluster&, const dpp::slashcommand_t&);


class Command : public dpp::slashcommand
{
    command_func_ptr    Execute;
};

void commandNotFound(dpp::cluster& bot, const dpp::slashcommand_t& event);

void commandPing(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandTest(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandRepeat(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandEco(dpp::cluster& bot, const dpp::slashcommand_t& event);
