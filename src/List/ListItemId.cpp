#include "ListItemId.h"

const std::string ListItemId::idLetters = "letters";
const std::string ListItemId::idLettersLowercase = "letters-lowercase";
const std::string ListItemId::idRandom = "random";

int
ListItemId::getIdLength()
{
    return idLength;
}

std::string const
ListItemId::generate(const std::string& generationType)
{
    std::string id;
    if (generationType == idLettersLowercase) {
        id = StringHelpers::randomLettersLowercase(idLength);
    } else if (generationType == idLetters) {
        id = StringHelpers::randomAlNumString(idLength);
    } else {
        id = StringHelpers::randomString(idLength);
    }

    return id;
}
