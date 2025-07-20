#include "Help.h"

Help::Help(IOService& ioService)
  : ioService(ioService)
{
}

void
Help::show()
{
    ioService.message("Help Center");
    ioService.message("----");
    for (std::string help : helpListForCli()) {
        ioService.message(help);
    }
}

void
Help::commandNotFound()
{
    ioService.error("Command not found");
    ioService.info("Type '-h/--help' for help");
}

void
Help::commandOptionNotSupported()
{
    ioService.error("The arguments of the command do not match. Aborting.");
    ioService.info("Type '-h/--help' for help");
}

std::vector<std::string>
Help::helpListForCli()
{
    return {
        "\033[96m-h/--help\033[0m                                                       Show the help center",
        "\033[96m-v/--version\033[0m                                                    Show current version of the "
        "app",
        "\033[96m-c/--config\033[0m                                                     Show the config of the app",
        "\033[96m-a/--archive\033[0m                                                    Show the archived items (valid "
        "for show/find command)",
        "\033[96m-d/--delete\033[0m                                                     Show the deleted items (valid "
        "for show/find command)",
        "\033[96m-f/--force\033[0m                                                      Force an action (valid for "
        "delete, copy-to & move-to commands)",
        "",
        "\033[94m-l/--list {name}\033[0m                                                Use a list for the command",
        "\033[94m-p/--priority {name}\033[0m                                            Precise the priority of the "
        "item(s)",
        "\033[94m-s/--status {name}\033[0m                                              Precise the status of the "
        "item(s)",
        "\033[94m-i/--deadline {cf. deadline}\033[0m                                    Precise the deadline of the "
        "item",
        "",
        "\033[0muse \033[93m{list}\033[0m                                                      Use a list",
        "\033[0mlist\033[0m                                                            Show all available lists",
        "     \033[0madd \033[93m{name}\033[0m                                                 Create new list",
        "     \033[0mrm \033[93m{name}\033[0m                                                  Remove list",
        "     \033[0mmv \033[93m{name} {new_nam}\033[0m                                        Remove list",
        "     \033[0mcurrent\033[0m                                                    Show the current list",
        "     \033[0mcopy \033[93m{listName} {newListName}\033[0m                              Copy/Duplicate the list",
        "",
        "\033[0mshow () \033[94m[-p {p}] [-s {S}] [-i {I}] \033[96m[-a] [-d]\033[0m                    Show items of "
        "the list",
        "\033[0madd (a) \033[93m{value} \033[94m[-p {p}] [-s {S}] [-i {I}]\033[0m                      Add a new item "
        "to the list",
        "\033[0medit (e) \033[93m{id} {value} \033[94m[-p {p}] [-s {S}] [-i {I}]\033[0m                Edit an item of "
        "the list",
        "\033[0mappend \033[93m{id} {value} \033[94m[-p {p}] [-s {S}] [-i {I}]\033[0m                  Append text to "
        "an item of the list",
        "\033[0mprepend \033[93m{id} {value} \033[94m[-p {p}] [-s {S}] [-i {I}]\033[0m                 Prepend text to "
        "an item of the list",
        "",
        "\033[0mfind (f) \033[93m{word} \033[96m[-a] [-d]\033[0m                                       Find an item in "
        "the list",
        "",
        "\033[0mremove (rm) \033[96m[-f] \033[93m{ids}\033[0m                                          Remove an item "
        "from the list",
        "\033[0mmove-to (mv) \033[93m{newList} {ids}\033[0m                                    Move an item to another "
        "list",
        "\033[0mcopy-to (cp) \033[93m{newList} {ids}\033[0m                                    Copy an item to another "
        "list",
        "\033[0mduplicate \033[93m{ids}\033[0m                                                 Duplicate an item on "
        "the list",
        "\033[0marchive \033[93m{ids}\033[0m                                                   Move the item(s) to the "
        "archive list",
        "\033[0mrestore \033[93m{ids}\033[0m                                                   Restore "
        "archived/deleted item(s)",
        "",
        "\033[0mpriority (p) \033[93m{priority} {ids}\033[0m                                   Set priority to one or "
        "more items",
        "\033[0mincrease (inc)/decrease (dec) \033[93m{1->4} {ids}\033[0m                      Increase or decrease "
        "priority by {1->4}(optional) amount to one or more items",
        "\033[0mdeadline \033[93m{date} {ids}\033[0m                                           Add a deadline to an "
        "item",
        "     \033[93m{date} \033[90m> Format: YYYY-MM-DD, MM-DD, DD\033[0m",
        "     \033[93m{date} \033[90m> Format: today, tomorrow, next-week, next-month, next-year\033[0m",
        "     \033[93m{date} \033[90m> Format: 1d, 1w, 1m, 1y, xday(s), xweek(s), xmonth(s), xyear(s)\033[0m",
        "     \033[93m{date} \033[90m> Remove: reset/none\033[0m",
        "",
        "\033[0mreset \033[93m{ids}/[all]\033[0m                                               Move the item(s)/all "
        "items to 'To-do' & reset timings",
        "\033[0mto-do \033[93m{ids}\033[0m                                                     Set the item(s) status "
        "to 'To do'",
        "\033[0mstart \033[93m{ids}\033[0m                                                     Set the item(s) status "
        "to 'started'",
        "\033[0mpause \033[93m{ids}\033[0m                                                     Set the item(s) status "
        "to 'paused'",
        "\033[0mreview \033[93m{ids}\033[0m                                                    Set the item(s) status "
        "to 'review'",
        "\033[0mpending \033[93m{ids}\033[0m                                                   Set the item(s) status "
        "to 'pending'",
        "\033[0mcomplete \033[93m{ids}\033[0m                                                  Set the item(s) status "
        "to 'completed'",
        "\033[0mcancel \033[93m{ids}\033[0m                                                    Set the item(s) status "
        "to 'cancel'",
        "",
        "\033[0mclean\033[0m                                                           Archive all Done/Cancelled of "
        "the list",
        "\033[0mempty\033[0m                                                           Empty the list",
    };
}

std::string
Help::getVersion()
{
    return "release candidate rc-1.2";
}
