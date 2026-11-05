#ifndef TODOOS_PATHHELPERS_H
#define TODOOS_PATHHELPERS_H

#include <filesystem>

class PathHelpers
{
  public:
    // Returns the current user's home directory, from HOME or else the passwd entry.
    // Throws std::runtime_error if neither provides one.
    static std::filesystem::path getHomeDir();
    // Picks the home directory from the given candidates, either of which may be null or empty.
    // Throws std::runtime_error if neither provides one.
    static std::filesystem::path resolveHomeDir(const char* homeEnv, const char* passwdDir);
};

#endif // TODOOS_PATHHELPERS_H
