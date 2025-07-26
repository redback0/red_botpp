
#pragma once

#include <dpp/dpp.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>

void commandHandler(dpp::cluster& bot, const dpp::slashcommand_t& event);
void commandPing(dpp::cluster& bot, const dpp::slashcommand_t& event);
