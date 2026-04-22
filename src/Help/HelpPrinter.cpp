#include "HelpPrinter.h"
#include "version.h"

HelpPrinter::HelpPrinter(IOService& ioService)
  : ioService(ioService)
{
}

void
HelpPrinter::show()
{
    ioService.message(" Help Center");
    for (std::string help : helpListForCli()) {
        ioService.message(help);
    }
}

void
HelpPrinter::listNotFound(std::string list)
{
    ioService.error("List \"" + list + "\" not found");
    ioService.info("Type 'todoos list' to show all available lists.");
}

void
HelpPrinter::commandNotFound()
{
    ioService.error("Command not found");
    ioService.info("Type '-h/--help' for help");
}

void
HelpPrinter::commandNotFoundSkipCommandAutocomplete(Command& command)
{
    if (command.isAutocompletion()) {
        return;
    }

    HelpPrinter::commandNotFound();
}

void
HelpPrinter::commandOptionNotSupported()
{
    ioService.error("The arguments of the command do not match. Aborting.");
    ioService.info("Type '-h/--help' for help");
}

void
HelpPrinter::commandOptionNotSupportedSkipCommandAutocomplete(Command& command)
{
    if (command.isAutocompletion()) {
        return;
    }

    HelpPrinter::commandOptionNotSupported();
}

std::vector<std::string>
HelpPrinter::helpListForCli()
{
    // Colors
    const std::string RST = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string CYN = "\033[96m"; // flags
    const std::string BLU = "\033[94m"; // options
    const std::string YLW = "\033[93m"; // arguments
    const std::string DIM = "\033[90m"; // dim / hints
    const std::string SEP =
        DIM +
        " \xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80"
        "\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80"
        "\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80"
        "\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80"
        "\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80"
        "\xE2\x94\x80" +
        RST;

    auto section = [&](const std::string& title) { return BOLD + " " + title + RST; };

    // clang-format off
    return {
        SEP,
        "",
        section("FLAGS"),
        "   " + CYN + "-h/--help"    + RST + "         Show the help center",
        "   " + CYN + "-v/--version" + RST + "      Show app version",
        "   " + CYN + "-c/--config"  + RST + "       Show app config",
        "   " + CYN + "-a/--archive" + RST + "      Include archived items  " + DIM + "(show, find)" + RST,
        "   " + CYN + "-d/--delete"  + RST + "       Include deleted items   " + DIM + "(show, find)" + RST,
        "   " + CYN + "-f/--force"   + RST + "        Force action            " + DIM + "(remove, copy-to, move-to)" + RST,
        "   " + CYN + "-Q/--no-quit" + RST + "      Keep app alive after command",
        "",
        section("OPTIONS"),
        "   " + BLU + "-l/--list {name}"     + RST + "         Target list",
        "   " + BLU + "-p/--priority {name}" + RST + "     Filter/set priority",
        "   " + BLU + "-s/--status {name}"   + RST + "       Filter/set status",
        "   " + BLU + "-i/--deadline {date}" + RST + "     Filter/set deadline",
        "",
        section("LISTS"),
        "   use "  + YLW + "{list}"           + RST + "                   Switch active list",   // col 13 + 19 = 32
        "   list                         Show all lists",                                         // col 7  + 25 = 32
        "     add " + YLW + "{name}"          + RST + "                 Create list",            // col 15 + 17 = 32
        "     rm  " + YLW + "{name}"          + RST + "                 Remove list",            // col 15 + 17 = 32
        "     mv  " + YLW + "{name} {new_name}" + RST + "      Rename list",                    // col 26 +  6 = 32
        "     current                    Show active list",                                       // col 12 + 20 = 32
        "     copy " + YLW + "{name} {new_name}" + RST + "     Duplicate list",                 // col 27 +  5 = 32
        "",
        section("ITEMS"),
        "   show                         Show items     " + BLU + "[-p] [-s] [-i] " + CYN + "[-a] [-d]" + RST, // col 7  + 25 = 32
        "   add     (a) " + YLW + "{value}"      + RST + "          Add item     " + BLU + "[-p] [-s] [-i]" + RST, // col 22 + 10 = 32
        "   edit    (e) " + YLW + "{id} {value}" + RST + "     Edit item     " + BLU + "[-p] [-s] [-i]" + RST,    // col 27 +  5 = 32
        "   append      " + YLW + "{id} {value}" + RST + "     Append to item" + BLU + " [-p] [-s] [-i]" + RST,  // col 27 +  5 = 32
        "   prepend     " + YLW + "{id} {value}" + RST + "     Prepend to item" + BLU + " [-p] [-s] [-i]" + RST, // col 27 +  5 = 32
        "   find    (f) " + YLW + "{word}"       + RST + "           Find item              " + CYN + "[-a] [-d]" + RST, // col 21 + 11 = 32
        "   describe    " + YLW + "{id}"       + RST + "             Edit the description of an item  " + CYN + "[-a] [-d]" + RST, // col 21 + 11 = 32
        "",
        section("MOVE & COPY"),
        "   remove  (rm) " + CYN + "[-f] " + YLW + "{ids}" + RST + "       Remove items",              // col 26 +  7 = 33
        "   move-to (mv) " + YLW + "{list} {ids}"           + RST + "     Move to list",               // col 28 +  5 = 33
        "   copy-to (cp) " + YLW + "{list} {ids}"           + RST + "     Copy to list",               // col 28 +  5 = 33
        "   duplicate    " + YLW + "{ids}"                  + RST + "            Duplicate items",      // col 21 + 12 = 33
        "   archive      " + YLW + "{ids}"                  + RST + "            Send to archive",      // col 21 + 12 = 33
        "   restore      " + YLW + "{ids}"                  + RST + "            Restore archived/deleted items", // col 21 + 12 = 33
        "",
        section("PRIORITY"),
        "   priority  (p)   " + YLW + "{priority} {ids}" + RST + "     Set priority",   // col 35 + 5 = 40
        "   increase  (inc) " + YLW + "[1-4] {ids}"     + RST + "          Raise priority", // col 30 + 10 = 40
        "   decrease  (dec) " + YLW + "[1-4] {ids}"     + RST + "          Lower priority", // col 30 + 10 = 40
        "",
        section("DEADLINE"),
        "   deadline " + YLW + "{date} {ids}"                     + RST + "                 Set deadline",
        "     " + DIM + "YYYY-MM-DD \xC2\xB7 MM-DD \xC2\xB7 DD" + RST,
        "     " + DIM + "today \xC2\xB7 tomorrow \xC2\xB7 next-week \xC2\xB7 next-month \xC2\xB7 next-year" + RST,
        "     " + DIM + "1d \xC2\xB7 1w \xC2\xB7 1m \xC2\xB7 1y \xC2\xB7 Xday(s) \xC2\xB7 Xweek(s) \xC2\xB7 Xmonth(s) \xC2\xB7 Xyear(s)" + RST,
        "     " + DIM + "remove: reset / none" + RST,
        "",
        section("STATUS"),
        "   queue    " + YLW + "{ids}"     + RST + "       \xE2\x86\x92 Queued",
        "   start    " + YLW + "{ids}"     + RST + "       \xE2\x86\x92 Started",
        "   triage   " + YLW + "{ids}"     + RST + "       \xE2\x86\x92 Triaged",
        "   pause    " + YLW + "{ids}"     + RST + "       \xE2\x86\x92 Paused",
        "   block    " + YLW + "{ids}"     + RST + "       \xE2\x86\x92 Blocked",
        "   complete " + YLW + "{ids}"     + RST + "       \xE2\x86\x92 Completed",
        "   cancel   " + YLW + "{ids}"     + RST + "       \xE2\x86\x92 Cancelled",
        "   reset    " + YLW + "{ids/all}" + RST + "   Reset to queued & clear date/deadline",
        "",
        section("BULK"),
        "   clean              Archive all completed/cancelled items",
        "   empty              Empty the list",
        "",
        SEP,
        DIM + "   shortcuts: create \xC2\xB7 up/upper \xC2\xB7 low/lower \xC2\xB7 current \xC2\xB7 all" + RST,
    };
    // clang-format on
}

std::string
HelpPrinter::getVersion()
{
    return "Version " APP_VERSION;
}
