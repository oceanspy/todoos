#ifndef TODOOS_INSTALLATIONINTERFACE_H
#define TODOOS_INSTALLATIONINTERFACE_H

#include <filesystem>
#include <string>

class InstallationInterface
{
  public:
    virtual ~InstallationInterface() = default;
    virtual bool isNew() = 0;
    virtual void make() = 0;
    virtual void populate() = 0;
    virtual bool wipe() = 0;
};

#endif // TODOOS_INSTALLATIONINTERFACE_H
