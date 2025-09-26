
#pragma once

#include <dpp/dpp.h>

#define INIT_COMMAND_LIST std::vector<dpp::slashcommand> _commands;
#define ADD_COMMAND(name, description, function, params) \
    g_command[( name )] = Command( function ); \
    _commands.push_back( \
        dpp::slashcommand(( name ), ( description ), bot.me.id) params \
    )
#define ADD_OPTION(type, name, description, required, params) \
    .add_option( \
        dpp::command_option( \
            ( type ), ( name ), ( description ), ( required ) \
        ) params \
    )
#define SET_PREV_DEF_PERM(perm) \
    _commands.back().default_member_permissions = ( perm );
#define REGISTER_COMMANDS bot.global_bulk_command_create(_commands);

typedef void (*command_func_ptr)(dpp::cluster&, const dpp::slashcommand_t&);


class Command
{
private:
    command_func_ptr    _Execute;

public:
    Command();
    Command(command_func_ptr Execute);

    void Execute(dpp::cluster& bot, const dpp::slashcommand_t& event) const;
};

void commandPing(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandTest(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandRepeat(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandCringe(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandEditCringe(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandEco(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandEcoAdmin(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandJoin(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandPlay(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandLeave(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandList(dpp::cluster& bot, const dpp::slashcommand_t& event);