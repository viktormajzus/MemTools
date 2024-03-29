#include "pch.h"
#include "MemTools.h"
#include "logic.h" // Include this only if necessary

MemTools::MemTools(const std::wstring& processName, DWORD dwDesiredAccess) : procId(0), handleManager()
{
  auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  HandleManager snapshotHandle(hSnapshot);

  if (hSnapshot == INVALID_HANDLE_VALUE)
  {
    OutputDebugString(L"Failed to create snapshot\n");
    return;
  }

  PROCESSENTRY32 procEntry;
  procEntry.dwSize = sizeof(PROCESSENTRY32);

  BOOL hResult = Process32First(hSnapshot, &procEntry);
  do
  {
    if (processName.compare(procEntry.szExeFile) == 0)
    {
      procId = procEntry.th32ProcessID;
      handleManager.SetHandle(OpenProcess(dwDesiredAccess, NULL, procId));
      return;
    }
  } while (Process32Next(hSnapshot, &procEntry));

  OutputDebugString(L"Failed to find process\n");
  return;
}

bool MemTools::GetModuleBase(std::wstring moduleName, uintptr_t& moduleBase)
{
  auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->procId);
  HandleManager snapshotHandle(hSnapshot);

  if (hSnapshot == INVALID_HANDLE_VALUE)
  {
    OutputDebugString(L"Failed to create snapshot\n");
    return false;
  }

  MODULEENTRY32 modEntry;
  modEntry.dwSize = sizeof(modEntry);

  BOOL hResult;
  hResult = Module32First(hSnapshot, &modEntry);

  do
  {
    if (moduleName.compare(modEntry.szModule) == 0)
    {
      moduleBase = (uintptr_t)modEntry.modBaseAddr;
      return true;
    }

    hResult = Module32Next(hSnapshot, &modEntry);
  } while (hResult);

  OutputDebugString(L"Failed to find module\n");
  return false;
}

MODULEENTRY32 MemTools::GetModule(std::wstring moduleName)
{
  auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->procId);
  HandleManager snapshotHandle(hSnapshot);

  if (hSnapshot == INVALID_HANDLE_VALUE)
  {
    OutputDebugString(L"Failed to create snapshot\n");
    MODULEENTRY32 failedModule{ -1 };
    return failedModule;
  }

  MODULEENTRY32 modEntry;
  modEntry.dwSize = sizeof(modEntry);

  BOOL hResult;
  hResult = Module32First(hSnapshot, &modEntry);

  do
  {
    if (moduleName.compare(modEntry.szModule) == 0)
    {
      return modEntry;
    }

    hResult = Module32Next(hSnapshot, &modEntry);
  } while (hResult);

  OutputDebugString(L"Failed to find module\n");
  MODULEENTRY32 failedModule{ -1 };
  return failedModule;
}

uintptr_t MemTools::Resolve(uintptr_t baseAddress, uintptr_t offset)
{
  uintptr_t addr = baseAddress;
  if (!ReadProcessMemory(handleManager.GetHandle(), reinterpret_cast<LPCVOID>(baseAddress), &addr, sizeof(addr), NULL))
  {
    OutputDebugString(L"Failed to read memory\n");
    return NULL;
  }
  addr += offset;
  return addr;
}

bool MemTools::ReadSized(uintptr_t address, size_t size, LPCVOID& buffer)
{
  if (!ReadProcessMemory(handleManager.GetHandle(), reinterpret_cast<LPCVOID>(address), &buffer, size, NULL))
  {
    OutputDebugString(L"Failed to read memory\n");
    return NULL;
  }
  return true;
}

SIZE_T MemTools::Patch(uintptr_t address, const std::vector<BYTE>& opcodes)
{
  DWORD oldProtect;
  SIZE_T bytes;

  if (!VirtualProtectEx(handleManager.GetHandle(), reinterpret_cast<LPVOID>(address), opcodes.size(), PAGE_EXECUTE_READWRITE, &oldProtect))
  {
    OutputDebugString(L"Patch failed: Failed to change protection\n");
    return NULL;
  }

  if (!WriteProcessMemory(handleManager.GetHandle(), reinterpret_cast<LPVOID>(address), opcodes.data(), opcodes.size(), &bytes))
  {
    VirtualProtectEx(handleManager.GetHandle(), reinterpret_cast<LPVOID>(address), opcodes.size(), oldProtect, &oldProtect);
    OutputDebugString(L"Patch failed: Failed to write memory\n");
    return NULL;
  }

  VirtualProtectEx(handleManager.GetHandle(), reinterpret_cast<LPVOID>(address), opcodes.size(), oldProtect, &oldProtect);
  return bytes;
}

SIZE_T MemTools::Nop(uintptr_t address, SIZE_T numBytes)
{
  std::vector<BYTE> nopArray(numBytes, 0x90);

  return this->Patch(address, nopArray);
}

uintptr_t MemTools::PatternScan(uintptr_t moduleBase, size_t textSize, const std::string& IDAPattern)
{
  std::string pattern, mask;
  parseIDAPattern(IDAPattern, pattern, mask);
  std::vector<int> lps = computeLPSArray(pattern, mask);

  size_t patternSize = pattern.size();
  std::vector<char> buffer;
  SIZE_T bytesRead;
  MEMORY_BASIC_INFORMATION mbi;
  uintptr_t endAddress = moduleBase + textSize;

  for (uintptr_t currAddress = moduleBase; currAddress < endAddress; currAddress += mbi.RegionSize)
  {
    if (!VirtualQueryEx(handleManager.GetHandle(), reinterpret_cast<LPCVOID>(currAddress), &mbi, sizeof(mbi)) ||
        mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS ||
        mbi.Protect & PAGE_GUARD)
    {
      continue;
    }

    size_t readSize = static_cast<size_t>(min(mbi.RegionSize, endAddress - currAddress));
    buffer.resize(readSize);
    if (!ReadProcessMemory(handleManager.GetHandle(), reinterpret_cast<LPCVOID>(currAddress), buffer.data(), readSize, &bytesRead))
    {
      OutputDebugStringA("Failed to read process memory\n");
      continue;
    }

    // Apply pattern scanning in the read memory
    for (size_t i = 0; i < bytesRead - patternSize; ++i)
    {
      size_t j = 0;
      for (; j < patternSize; ++j)
      {
        if (mask[j] != '?' && pattern[j] != buffer[i + j])
          break;
      }
      if (j == patternSize)
      {
        return (currAddress + i) - moduleBase; // Pattern found
      }
    }
  }

  OutputDebugStringA("Pattern not found\n");
  return 0; // Pattern not found, return 0 as an invalid address
}