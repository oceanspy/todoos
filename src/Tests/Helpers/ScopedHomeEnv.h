#ifndef TODOOS_SCOPEDHOMEENV_H
#define TODOOS_SCOPEDHOMEENV_H

#include <cstdlib>
#include <optional>
#include <string>

// Overrides HOME for the current scope and restores it afterwards
class ScopedHomeEnv
{
  public:
    explicit ScopedHomeEnv(const char* value)
    {
        if (const char* current = std::getenv("HOME")) {
            original = current;
        }
        apply(value);
    }

    ~ScopedHomeEnv() { apply(original ? original->c_str() : nullptr); }

    ScopedHomeEnv(const ScopedHomeEnv&) = delete;
    ScopedHomeEnv& operator=(const ScopedHomeEnv&) = delete;

  private:
    std::optional<std::string> original;

    static void apply(const char* value)
    {
        if (value) {
            setenv("HOME", value, 1);
        } else {
            unsetenv("HOME");
        }
    }
};

#endif // TODOOS_SCOPEDHOMEENV_H
