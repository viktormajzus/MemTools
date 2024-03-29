#pragma once
#include "pch.h"

void ParseIDAPattern(const std::string& idaPattern, std::string& pattern, std::string& mask);
std::vector<int> ComputeLPSArray(const std::string& pattern, const std::string& mask);