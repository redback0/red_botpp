
#include <dpp/dpp.h>
#include <random>
#include <string>
#include <variant>

void commandCringe(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    std::string text;

    const auto& param = event.get_parameter("text");
    if (std::holds_alternative<std::string>(param))
    {
        text = std::get<std::string>(param);
    }
    else
    {
        text = event.command.member.get_mention();
    }

    int percent = 0;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution percent_gen(0, 100);
    percent = percent_gen(gen);

    event.reply(text + " is " + std::to_string(percent) + "% cringe");
}