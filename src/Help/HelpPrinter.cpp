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
HelpPrinter::listNotFound(const std::string& list)
{
    ioService.error("List \"" + list + "\" not found");
    ioService.info("Type 'todoos list' to show all available lists.");
}

void
HelpPrinter::listItemNotFound(const std::string& id, const ListName& listName)
{
    ioService.error("Item \"" + id + "\" not found in list: " + listName.getName());
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

    // A cell is a list of (text, color) parts padded to `width` visible chars (ASCII text only), so that
    // every column lines up whatever the colors
    using Parts = std::vector<std::pair<std::string, std::string>>;
    auto cell = [&](size_t width, const Parts& parts) {
        std::string render;
        size_t length = 0;
        for (const auto& [text, color] : parts) {
            render += color.empty() ? text : color + text + RST;
            length += text.size();
        }
        return render + std::string(length < width ? width - length : 1, ' ');
    };

    const size_t FLAG_WIDTH = 25; // FLAGS / OPTIONS: name column
    const size_t FLAG_DESC_WIDTH = 22;
    const size_t COMMAND_WIDTH = 36; // command sections: "name (alias) args" column
    const size_t DESC_WIDTH = 24;

    auto flag = [&](const std::string& name, const std::string& desc, const std::string& appliesTo = "") {
        return "   " + cell(FLAG_WIDTH, { { name, CYN } }) +
               (appliesTo.empty() ? desc : cell(FLAG_DESC_WIDTH, { { desc, "" } }) + DIM + "(" + appliesTo + ")" + RST);
    };
    auto option = [&](const std::string& name, const std::string& desc, const std::string& appliesTo = "") {
        return "   " + cell(FLAG_WIDTH, { { name, BLU } }) +
               (appliesTo.empty() ? desc : cell(FLAG_DESC_WIDTH, { { desc, "" } }) + DIM + "(" + appliesTo + ")" + RST);
    };
    // name column (10), alias column (7), then the arguments (yellow) and their options (blue, from " -")
    auto command = [&](const std::string& name,
                       const std::string& alias,
                       const std::string& args,
                       const std::string& desc,
                       const Parts& hints = {}) {
        Parts parts = { { name + std::string(name.size() < 10 ? 10 - name.size() : 1, ' '), "" } };
        std::string aliasText = alias.empty() ? "" : "(" + alias + ")";
        parts.push_back({ aliasText + std::string(aliasText.size() < 7 ? 7 - aliasText.size() : 1, ' '), "" });
        size_t optionsAt = args.find(" -");
        parts.push_back({ args.substr(0, optionsAt), YLW });
        if (optionsAt != std::string::npos) {
            parts.push_back({ args.substr(optionsAt), BLU });
        }
        std::string line = "   " + cell(COMMAND_WIDTH, parts);
        if (hints.empty()) {
            return line + desc;
        }
        line += cell(DESC_WIDTH, { { desc, "" } });
        for (size_t i = 0; i < hints.size(); i++) {
            line += (i == 0 ? "" : " ") + hints[i].second + hints[i].first + RST;
        }
        return line;
    };
    auto hintLine = [&](const std::string& text, const std::string& color) {
        return std::string(3 + COMMAND_WIDTH + DESC_WIDTH, ' ') + color + text + RST;
    };
    auto note = [&](const std::string& text) { return "     " + DIM + text + RST; };
    const Parts FILTERS = { { "[-p]", BLU }, { "[-s]", BLU }, { "[-x]", BLU } };
    auto withFlags = [&](Parts parts, const std::vector<std::string>& flags) {
        for (const std::string& f : flags) {
            parts.push_back({ "[" + f + "]", CYN });
        }
        return parts;
    };

    // clang-format off
    return {
        SEP,
        "",
        DIM + "   todoos [flags] [options] <command> [args]" + RST,
        "",
        section("FLAGS"),
        flag("-h/--help", "Show the help center"),
        flag("-v/--version", "Show app version"),
        flag("-c/--config", "Show app config"),
        flag("-A/--archived", "Show archived items", "show, find"),
        flag("-D/--deleted", "Show deleted items", "show, find"),
        flag("-d/--described", "Describe items", "add, show, find, clean"),
        flag("-f/--force", "Force action", "remove, move, copy, attach, detach"),
        flag("-F/--force-permanently", "Delete for good", "remove"),
        flag("-a/--attached", "Include child items", "clean"),
        flag("-Q/--no-quit", "Keep app alive after command"),
        "",
        section("OPTIONS"),
        option("-l/--list {name}", "Target list"),
        option("-p/--priority {name}", "Filter/set priority"),
        option("-s/--status {name}", "Filter/set status"),
        option("-x/--deadline {date}", "Filter/set deadline"),
        option("-t/--to {list}", "Target list", "move, copy"),
        option("-t/--to {id}", "Parent item", "add, attach"),
        option("-t/--to {newId}", "New item id", "reassign"),
        "",
        section("LISTS"),
        command("use", "", "{list}", "Switch active list"),
        command("list", "", "", "Show all lists"),
        command("  add", "", "{name}", "Create list"),
        command("  rm", "", "{name}", "Remove list"),
        command("  mv", "", "{name} {new_name}", "Rename list"),
        command("  copy", "", "{name} {new_name}", "Duplicate list"),
        command("  current", "", "", "Show active list"),
        "",
        section("ITEMS"),
        command("show", "", "", "Show items", withFlags(FILTERS, { "-A", "-d", "-D" })),
        command("add", "a", "{value}", "Add item", withFlags(FILTERS, { "-d" })),
        command("edit", "e", "{id} {value}", "Edit item", FILTERS),
        command("append", "", "{id} {value}", "Append to item", FILTERS),
        command("prepend", "", "{id} {value}", "Prepend to item", FILTERS),
        command("find", "f", "{word}", "Find items", withFlags({}, { "-A", "-d", "-D" })),
        command("describe", "d", "{id}", "Edit item description"),
        "",
        section("ITEM ACTIONS"),
        command("remove", "rm", "{ids}", "Remove items", withFlags({}, { "-f", "-F" })),
        command("duplicate", "", "{ids}", "Duplicate items"),
        command("archive", "", "{ids}", "Send to archive"),
        command("restore", "", "{ids}", "Restore archived/deleted items"),
        command("reassign", "", "{id} -t {newId}", "Choose an item's id"),
        note("4 letters/digits, free in the list; children & description follow"),
        "",
        section("PARENT / CHILD"),
        command("add", "a", "{value} -t {id}", "Add a child item"),
        command("attach", "", "{ids} -t {id}", "Attach to a parent", { { "[-f] switch parent", CYN } }),
        command("detach", "", "{ids}", "Detach children", { { "[-f] all, on a parent", CYN } }),
        note("children follow their parent: remove -f, archive, restore, move, copy"),
        "",
        section("TRANSFER"),
        command("move", "mv", "{ids} -t {list}", "Move to list", withFlags({}, { "-f" })),
        command("copy", "cp", "{ids} -t {list}", "Copy to list", withFlags({}, { "-f" })),
        "",
        section("PRIORITY"),
        command("priority", "p", "{priority} {ids}", "Set priority"),
        note("low \xC2\xB7 medium \xC2\xB7 high \xC2\xB7 urgent \xC2\xB7 critical"),
        command("increase", "inc", "[1-4] {ids}", "Raise priority"),
        command("decrease", "dec", "[1-4] {ids}", "Lower priority"),
        "",
        section("DEADLINE"),
        command("deadline", "dln", "{date} {ids}", "Set deadline"),
        note("YYYY-MM-DD \xC2\xB7 MM-DD \xC2\xB7 DD"),
        note("today \xC2\xB7 tomorrow \xC2\xB7 next-week \xC2\xB7 next-month \xC2\xB7 next-year"),
        note("1d \xC2\xB7 1w \xC2\xB7 1m \xC2\xB7 1y \xC2\xB7 Xday(s) \xC2\xB7 Xweek(s) \xC2\xB7 Xmonth(s) \xC2\xB7 Xyear(s)"),
        note("remove: reset / none"),
        "",
        section("STATUS"),
        command("queue", "", "{ids}", "\xE2\x86\x92 Queued"),
        command("start", "", "{ids}", "\xE2\x86\x92 Started"),
        command("triage", "", "{ids}", "\xE2\x86\x92 Triaged"),
        command("pause", "", "{ids}", "\xE2\x86\x92 Paused"),
        command("block", "", "{ids}", "\xE2\x86\x92 Blocked"),
        command("complete", "done", "{ids}", "\xE2\x86\x92 Completed"),
        command("cancel", "", "{ids}", "\xE2\x86\x92 Cancelled"),
        command("reset", "", "{ids/all}", "\xE2\x86\x92 Queued, clear date/deadline"),
        "",
        section("BULK"),
        command("clean", "", "", "Archive done/cancelled", { { "[-d] include described items", CYN } }),
        hintLine("[-a] include child items", CYN),
        command("empty", "", "", "Empty the list"),
        "",
        section("SHORTCUTS"),
        "   create            \xE2\x86\x92 add",
        "   up / upper        \xE2\x86\x92 increase",
        "   low / lower       \xE2\x86\x92 decrease",
        "   current           \xE2\x86\x92 list current",
        "   all               \xE2\x86\x92 show (all items)",
        "   done              \xE2\x86\x92 complete",
        "   dln               \xE2\x86\x92 deadline",
        "   desc              \xE2\x86\x92 describe",
        "",
        SEP,
        DIM + "   {id}  4-char item identifier  \xC2\xB7  {ids}  space-separated: ab12 cd34 ef56" + RST,
    };
    // clang-format on
}

std::string
HelpPrinter::getVersion()
{
    return "Version " APP_VERSION;
}
