/** utils.h
 * utility functions
 * Tadas Saltenis November 2021
 */
#include "utils.h"

/**
 * read and return contents of file
 * NOTE: modified example from Iain Martin
 */
std::string readFile(const char *filePath)
{
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open())
    {
        std::cerr << "Could not read file " << filePath << ". File does not exist."
                  << std::endl;
        return "";
    }

    std::string line = "";
    while (!fileStream.eof())
    {
        getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}
