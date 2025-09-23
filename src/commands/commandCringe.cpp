
#include <dpp/dpp.h>
#include <dpp/message.h>
#include <random>
#include <string>
#include <variant>

static struct s_cringe {
    dpp::slashcommand_t event;
    std::string text;
} lastCringe = {};

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
    lastCringe = {event, text};
}

void commandEditCringe(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    long percent;

    if (lastCringe.text.empty())
    {
        dpp::message message("Nuh uh");
        message.set_flags(dpp::m_ephemeral);
        event.reply(message);
        throw std::runtime_error("No cringe to edit");
    }

    const auto& param = event.get_parameter("percent");
    if (std::holds_alternative<long>(param))
    {
        percent = std::get<long>(param);
    }
    else
    {
        // shouldn't occur
        dpp::message message("how'd you miss the parameter");
        message.set_flags(dpp::m_ephemeral);
        event.reply(message);
        throw std::runtime_error("Parameter 'percent' was not present");
    }

    dpp::message newCringe(lastCringe.text + " is " +
        std::to_string(percent) + "% cringe");
    lastCringe.event.edit_original_response(newCringe);

    dpp::message message("Edited message");
    message.set_flags(dpp::m_ephemeral);
    event.reply(message);
}