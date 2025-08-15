
#include <string>
#include <dpp/dpp.h>

std::string getUserDisplayName(const dpp::guild_member& member)
{
    std::string name = member.get_nickname();

    if (name.empty())
    {
        const dpp::user* user = member.get_user();
        if (user == NULL)
            return "NAME_NOT_FOUND";

        name = user->global_name;
        if (name.empty())
            name = user->username;
    }

    return name;
}