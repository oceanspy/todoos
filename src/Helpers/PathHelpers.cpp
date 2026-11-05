#include "PathHelpers.h"

#include <cstdlib>
#include <pwd.h>
#include <stdexcept>
#include <unistd.h>

std::filesystem::path
PathHelpers::getHomeDir()
{
    const char* homeEnv = std::getenv("HOME");
    if (homeEnv && *homeEnv) {
        return homeEnv;
    }

    const passwd* pw = getpwuid(getuid());
    return resolveHomeDir(homeEnv, pw ? pw->pw_dir : nullptr);
}

std::filesystem::path
PathHelpers::resolveHomeDir(const char* homeEnv, const char* passwdDir)
{
    if (homeEnv && *homeEnv) {
        return homeEnv;
    }
    if (passwdDir && *passwdDir) {
        return passwdDir;
    }

    throw std::runtime_error("No home directory found: set the HOME environment variable.");
}
