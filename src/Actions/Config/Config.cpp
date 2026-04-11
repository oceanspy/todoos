#include "Config.h"

Config::Config(IOService& ioService,
               Command& command,
               CommandService& commandService,
               ConfigService& configService,
               ListService& listService)
  : ioService(ioService)
  , command(command)
  , commandService(commandService)
  , configService(configService)
  , listService(listService)
{
}

void
Config::make()
{
    Command subCommand = commandService.getSubCommand(command);
    if (CommandService::isCommand(subCommand, "config") || subCommand.getName().empty()) {
        showAll();
        return;
    } else if (CommandService::isCommand(subCommand, "edit")) {
        if (subCommand.getArguments().size() < 2) {
            ioService.br();
            ioService.error("Please provide a key and a value.");
            ioService.br();
            return;
        }

        std::string key = subCommand.getArguments().at(0);
        std::string value = subCommand.getArguments().at(1);

        if (key == "defaultList") {
            if (listService.isListExist(subCommand.getArguments().at(1))) {
                configService.edit(key, value);
                ioService.br();
                ioService.success("Default list has been updated.");
                ioService.br();
                showAll();
                return;
            }

            ioService.br();
            ioService.error("List not found. Aborting.");
            ioService.br();
            showAll();
            return;
        } else if (key == "theme") {
            if (value == "default" || value == "mobile" || value == "color" || value == "colorMobile" ||
                value == "white") {
                configService.edit(key, value);
                ioService.br();
                ioService.success("Theme has been updated.");
                ioService.br();
                showAll();
                return;
            }

            ioService.br();
            ioService.error("Theme not found. Aborting.");
            ioService.br();
            showAll();
            return;
        }
        //        else if (subCommand.getArguments().at(0) == "fileDataStorageType")
        //        {
        //            if (value != "json" && value != "csv")
        //            {
        //                ioService.error("Invalid file data storage type. Please use 'json' or 'csv'.");
        //                return;
        //            }
        //            else
        //            {
        //                configService.edit(key, value);
        //                return;
        //            }
        //        }

        ioService.br();
        ioService.error("Sorry, the update of this configuration is not supported for the moment.");
        ioService.br();
        return;
    }

    throw std::invalid_argument("Invalid config action.");
}

void
Config::showAll()
{
    ioService.print("#######################################");
    ioService.print("# Current configuration               #");
    ioService.print("#######################################");
    std::vector<ConfigEntity> configItems = configService.get();
    for (const ConfigEntity& configItem : configItems) {
        std::string line;
        line = *configItem.getKey() + ": " + *configItem.getValue();
        ioService.print(line);
    }
}
