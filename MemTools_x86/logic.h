#pragma once
#include "pch.h"

void parseIDAPattern(const std::string& idaPattern, std::string& pattern, std::string& mask);
std::vector<int> computeLPSArray(const std::string& pattern, const std::string& mask);