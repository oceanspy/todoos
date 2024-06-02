#ifndef BASHSTYLE_H
#define BASHSTYLE_H

#define RESET_STYLE   "\033[0m"
#define RED     "\033[31m"              /* Red */
#define LIGHT_RED     "\033[91m"        /* Light Red */
#define GREEN   "\033[32m"              /* Green */
#define LIGHT_GREEN   "\033[92m"              /* Green */
#define YELLOW  "\033[33m"              /* Yellow */
#define LIGHT_YELLOW  "\033[93m"        /* Yellow */
#define BLUE    "\033[34m"              /* Blue */
#define LIGHT_BLUE    "\033[94m"        /* Light Blue */
#define MAGENTA "\033[35m"              /* Magenta */
#define LIGHT_MAGENTA "\033[95m"              /* Magenta */
#define CYAN    "\033[36m"              /* Cyan */
#define LIGHT_CYAN    "\033[96m"        /* Light Cyan */
#define GRAY    "\033[90m"              /* Gray */
#define LIGHT_GRAY   "\033[37m"         /* White */
#define DARK_GRAY    "\033[38;5;242m"   /* Dark Gray */
#define WHITE   "\033[39m"              /* White */
#define ORANGE  "\033[38;5;208m"        /* Orange */

// BACKGROUND COLORS
#define BG_RED     "\033[41m"           /* Red */
#define BG_GREEN   "\033[42m"           /* Green */
#define BG_YELLOW   "\033[43m"          /* YELLOW */
#define BG_BLUE   "\033[44m"            /* Green */
#define BG_MAGENTA "\033[45m"              /* Magenta */
#define BG_CYAN    "\033[46m"              /* Cyan */

// bold
#define BOLD    "\033[1m"

// italic
#define ITALIC  "\033[3m"

#include <string>

class BashStyle {
public:
    static std::string getBashCode(std::string style);
};

#endif //BASHSTYLE_H
