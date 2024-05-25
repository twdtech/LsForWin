#include <windows.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

static void SetConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

static void resConCol() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
}

static int GetConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;

    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return 80;
    }

    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    return columns;
}

static void PrintDirectoryContents(const char* path) {
    DIR* dir;
    struct dirent* ent;
    std::vector<std::pair<std::string, WORD>> files;

    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string fileName = ent->d_name;
            WORD color;

            if (fileName == "." || fileName == "..") continue;

            if (ent->d_type == DT_DIR) {
                color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
            else if (ent->d_type == DT_LNK) {
                color = FOREGROUND_RED | FOREGROUND_INTENSITY;
            }
            else if (fileName.substr(fileName.find_last_of(".") + 1) == "dll" || fileName.substr(fileName.find_last_of(".") + 1) == "exe") {
                color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
            else {
                color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }

            files.push_back(std::make_pair(fileName, color));
        }
        closedir(dir);
    }
    else {
        perror("");
    }

    std::sort(files.begin(), files.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.first.length() < rhs.first.length();
        });

    int consoleWidth = GetConsoleWidth();
    int maxFileNameLength = 0;
    int currentLineLength = 0;

    for (const auto& file : files) {
        int fileNameLength = file.first.length() + 1;

        if (fileNameLength > maxFileNameLength) {
            maxFileNameLength = fileNameLength;
        }

        if (currentLineLength + maxFileNameLength + 5 > consoleWidth) {
            std::cout << std::endl;
            currentLineLength = 0;
        }

        SetConsoleColor(file.second);
        std::cout << std::setw(maxFileNameLength) << std::setfill(' ') << std::left << file.first << "   ";
        currentLineLength += maxFileNameLength + 5;
    }

    std::cout << std::endl;
    SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}

int main() {
    PrintDirectoryContents(".");
    resConCol();
    return 0;
}