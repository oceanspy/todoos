#ifndef TODOOS_APPINSTALLATION_INTERFACE_H
#define TODOOS_APPINSTALLATION_INTERFACE_H

#include <filesystem>
#include <string>

class AppInstallationInterface
{
  public:
    virtual ~AppInstallationInterface() = default;
    virtual bool isNew() = 0;
    virtual void make() = 0;
    virtual void populate() = 0;
    virtual bool wipe() = 0;
};

#endif // TODOOS_APPINSTALLATION_INTERFACE_H
