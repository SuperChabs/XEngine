module;

#include <unordered_map>
#include <functional>
#include <string>

export module XEngine.Core.CommandManager;

import XEngine.Core.Logger;

export class CommandManager
{
private:
    static std::unordered_map<std::string, std::function<void()>> commands;

public:
    static void RegisterCommand(const std::string& name, std::function<void()> command)
    {
        if(commands.contains(name))
        {
            Logger::Log(LogLevel::WARNING, LogCategory::CORE, "Command '" + name + "' already exists");
            return;
        }

        commands.insert({name, command});
    }

    static void ExecuteCommand(const std::string& name)
    {
        auto it = commands.find(name);
        if (it != commands.end())
            it->second();
    }

    static bool HasCommand(const std::string& name)
    {
        return commands.contains(name);
    }
};

module :private;
std::unordered_map<std::string, std::function<void()>> CommandManager::commands;