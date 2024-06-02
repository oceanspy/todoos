#include "BashStyle.h"

std::string BashStyle::getBashCode(std::string style)
{
    if (style == "GRAY") return GRAY;
    else if (style == "LIGHT_GRAY") return LIGHT_GRAY;
    else if (style == "DARK_GRAY") return DARK_GRAY;
    else if (style == "WHITE") return WHITE;
    else if (style == "RED") return RED;
    else if (style == "LIGHT_RED") return LIGHT_RED;
    else if (style == "GREEN") return GREEN;
    else if (style == "LIGHT_GREEN") return LIGHT_GREEN;
    else if (style == "YELLOW") return YELLOW;
    else if (style == "LIGHT_YELLOW") return LIGHT_YELLOW;
    else if (style == "BLUE") return BLUE;
    else if (style == "LIGHT_BLUE") return LIGHT_BLUE;
    else if (style == "MAGENTA") return MAGENTA;
    else if (style == "LIGHT_MAGENTA") return LIGHT_MAGENTA;
    else if (style == "CYAN") return CYAN;
    else if (style == "LIGHT_CYAN") return LIGHT_CYAN;
    else if (style == "ORANGE") return ORANGE;
    else if (style == "BOLD") return BOLD;
    else if (style == "ITALIC") return ITALIC;
    else if (style == "RESET") return RESET_STYLE;
    else return "";
}
