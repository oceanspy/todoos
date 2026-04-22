#include "ItemTempFile.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

ItemTempFile::ItemTempFile(const DescribeItem& describeItem, const std::filesystem::path& cacheDirPath)
  : describeItem(describeItem)
  , cacheDirPath(cacheDirPath)
{
}

std::string
ItemTempFile::execute()
{
    std::filesystem::path editFilePath = cacheDirPath
                                       / "describe"
                                       / describeItem.getListName().getName()
                                       / (*describeItem.getListItem().getId() + ".md");

    std::filesystem::create_directories(editFilePath.parent_path());

    std::ofstream file(editFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not write to description edit file.");
    }
    file << describeItem.asDescriptionItemFile();
    file.close();

    openInEditor(editFilePath.string());

    return editFilePath.string();
}

void
ItemTempFile::reopen(const std::string& filePath)
{
    openInEditor(filePath);
}

void
ItemTempFile::repair(const std::string& filePath, const DescribeItem& describeItem)
{
    std::ifstream file(filePath);
    std::string recoveredBody;

    if (file.is_open()) {
        std::string line;
        std::string lastTitleLineContent;
        std::ostringstream afterTitle;
        bool titleFound = false;

        while (std::getline(file, line)) {
            if (line.rfind("# ", 0) == 0) {
                titleFound = true;
                afterTitle.str("");
                afterTitle.clear();
                continue;
            }
            if (titleFound) {
                afterTitle << line << "\n";
            }
        }

        recoveredBody = titleFound ? afterTitle.str() : "";
    }

    std::ofstream out(filePath);
    if (!out.is_open()) {
        throw std::runtime_error("Could not repair description edit file.");
    }
    out << describeItem.asDescriptionItemFile();
    out << recoveredBody;
}

void
ItemTempFile::openInEditor(const std::string& filePath)
{
    std::string editor = resolveEditor();

    pid_t pid = fork();
    if (pid == 0) {
        execlp(editor.c_str(), editor.c_str(), filePath.c_str(), nullptr);
        _exit(1);
    }
    if (pid == -1) {
        throw std::runtime_error("Could not launch editor.");
    }

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        throw std::runtime_error("Editor exited with error.");
    }
}

std::string
ItemTempFile::resolveEditor()
{
    const char* visual = std::getenv("VISUAL");
    if (visual && visual[0] != '\0') {
        return visual;
    }

    const char* editor = std::getenv("EDITOR");
    if (editor && editor[0] != '\0') {
        return editor;
    }

    return "nano";
}
