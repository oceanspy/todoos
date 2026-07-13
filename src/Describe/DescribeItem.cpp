#include "DescribeItem.h"
#include "../Helpers/DateHelpers.h"
#include <sstream>

DescribeItem::DescribeItem(const ListName& listName,
                           const ListItemEntity& listItem,
                           const std::string& existingDescription)
  : listName(listName)
  , listItem(listItem)
  , existingDescription(existingDescription)
{
}

DescribeItem
DescribeItem::create(const ListName& listName,
                     const ListItemEntity& listItem,
                     const std::string& existingDescription)
{
    return DescribeItem(listName, listItem, existingDescription);
}

std::string
DescribeItem::asDescriptionItemFile() const
{
    std::string createdAt = "-";
    if (listItem.getCreatedAt() && *listItem.getCreatedAt() != 0) {
        createdAt = DateHelpers::formatTimestamp(*listItem.getCreatedAt());
    }

    std::string colId       = *listItem.getId();
    std::string colList     = listName.getName();
    std::string colStatus   = *listItem.status()->getName();
    std::string colPriority = *listItem.priority()->getName();
    std::string colCreated  = createdAt;

    auto w = [](const std::string& header, const std::string& value) {
        return std::max(header.size(), value.size());
    };

    size_t wId       = w("ID",       colId);
    size_t wList     = w("List",     colList);
    size_t wStatus   = w("Status",   colStatus);
    size_t wPriority = w("Priority", colPriority);
    size_t wCreated  = w("Created",  colCreated);

    auto pad = [](const std::string& value, size_t width) {
        return value + std::string(width - value.size(), ' ');
    };

    auto sep = [](size_t width) {
        return std::string(width, '-');
    };

    std::ostringstream out;

    out << "| " << pad("ID",       wId)       << " | " << pad("List",     wList)
        << " | " << pad("Status",   wStatus)   << " | " << pad("Priority", wPriority)
        << " | " << pad("Created",  wCreated)  << " |\n";
    out << "| " << sep(wId)       << " | " << sep(wList)
        << " | " << sep(wStatus)   << " | " << sep(wPriority)
        << " | " << sep(wCreated)  << " |\n";
    out << "| " << pad(colId,       wId)       << " | " << pad(colList,     wList)
        << " | " << pad(colStatus,   wStatus)   << " | " << pad(colPriority, wPriority)
        << " | " << pad(colCreated,  wCreated)  << " |\n";
    out << "\n";
    out << "<!-- todoos:title -->\n";
    out << "# " << *listItem.getValue() << "\n";
    out << "\n";
    out << existingDescription;

    return out.str();
}

const ListName&
DescribeItem::getListName() const
{
    return listName;
}

const ListItemEntity&
DescribeItem::getListItem() const
{
    return listItem;
}

const std::string&
DescribeItem::getExistingDescription() const
{
    return existingDescription;
}
