#pragma once
#include "pch.h"
#include "HandleManager.h"

class MemTools
{
public:
  explicit MemTools(const std::wstring& processName, DWORD dwDesiredAccess);
  bool GetModuleBase(std::wstring moduleName, uintptr_t& moduleBase);
  MODULEENTRY32 GetModule(std::wstring moduleName);
  uintptr_t Resolve(uintptr_t baseAddress, uintptr_t offset);
  template <size_t N>
  uintptr_t ResolveDMA(uintptr_t baseAddress, const std::array<uintptr_t, N>& offsets);
  template<typename T>
  T Read(uintptr_t address);
  bool ReadSized(uintptr_t address, size_t size, LPCVOID& buffer);
  template<typename T>
  bool Write(uintptr_t address, T& val);
  SIZE_T Patch(uintptr_t address, const std::vector<BYTE>& opcodes);
  SIZE_T Nop(uintptr_t address, SIZE_T numBytes);
  uintptr_t PatternScan(uintptr_t moduleBase, size_t textSize, const std::string& IDAPattern);

private:
  uintptr_t procId;
  HandleManager handleManager;
};

#include "MemToolsTemplates.h"