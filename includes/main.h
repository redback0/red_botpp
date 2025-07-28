
#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <dpp/dpp.h>

void registerCommands(dpp::cluster& bot);

void commandHandler(dpp::cluster& bot, const dpp::slashcommand_t& event);
