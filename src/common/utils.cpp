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

transforms_t parseTransformsSection(std::string s)
{
    std::stringstream ssInput;
    ssInput.str(s);
    transforms_t transforms;
    for (std::string line; getline(ssInput, line, '\n'); )
    {
        std::vector<float> pos;
        std::stringstream ssLine;
        ssLine.str(line);
        for (std::string floatStr; getline(ssLine, floatStr, ','); )
            pos.push_back(std::stof(floatStr));

        if (pos.size() == 3)
            transforms.push_back(glm::vec3(pos[0], pos[1], pos[2]));
    }
    return transforms;
}

std::pair<transforms_t, transforms_t> parseTransformsFile(const char *filePath)
{
    std::string tvTransformsStr = readFile(filePath);
    int splitPos = tvTransformsStr.find("\n\n");

    transforms_t translations = parseTransformsSection(tvTransformsStr.substr(0, splitPos));
    transforms_t rotations = parseTransformsSection(tvTransformsStr.substr(splitPos, std::string::npos));

    return std::pair<transforms_t, transforms_t>(translations, rotations);
}
