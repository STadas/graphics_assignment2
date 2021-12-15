/** utils.h
 * utility functions
 * Tadas Saltenis November 2021
 */
#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>

typedef std::vector<glm::vec3> transforms_t;

std::string readFile(const char *filePath);
transforms_t parseTransformsSection(std::string s);
std::pair<transforms_t, transforms_t> parseTransformsFile(const char *filePath);
