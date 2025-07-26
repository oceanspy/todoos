#include "ListName.h"
#include <algorithm>
#include <stdexcept>

ListName::ListName(std::string name, std::string variant)
  : name(std::move(name))
  , variant(std::move(variant))
{
}

ListName
ListName::createVariant(ListName& name, std::string variant)
{
    if (!isVariantExists(variant)) {
        throw std::invalid_argument("Unexpected error when setting up list");
    }

    return ListName(name.getName(), variant);
}

std::string
ListName::getName() const
{
    return name;
}

std::string
ListName::getVariant() const
{
    return variant;
}

bool
ListName::isVariantExists(std::string& variant)
{
    if (variant == "default") {
        return true;
    } else if (variant == "archive") {
        return true;
    } else if (variant == "delete") {
        return true;
    }

    return false;
}
