#include "pch.h"
#include "logic.h"

void parseIDAPattern(const std::string& idaPattern, std::string& pattern, std::string& mask)
{
  std::istringstream iss(idaPattern);
  std::string byteStr;

  pattern.clear();
  mask.clear();

  while (iss >> byteStr)
  {
    if (byteStr == "?")
    {
      pattern += '\x00'; // Arbitrary placeholder byte for '?'
      mask += '?';
    } else
    {
      unsigned int byteVal = std::stoul(byteStr, nullptr, 16);
      pattern += static_cast<char>(byteVal);
      mask += 'x';
    }
  }
}

std::vector<int> computeLPSArray(const std::string& pattern, const std::string& mask)
{
  int length = 0;  // Length of the previous longest prefix suffix
  int i = 1;
  std::vector<int> lps(pattern.size(), 0);

  while (i < pattern.size())
  {
    if (mask[i] == '?' || pattern[i] == pattern[length])
    {
      length++;
      lps[i] = length;
      i++;
    } else
    {
      if (length != 0)
      {
        length = lps[length - 1];
      } else
      {
        lps[i] = 0;
        i++;
      }
    }
  }

  return lps;
}