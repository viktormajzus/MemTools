#pragma once
#include "pch.h"
#include "MemTools.h"

/// Resolves the address of a multi-offset pointer (pointer chain)
template <size_t N>
uintptr_t MemTools::ResolveDMA(uintptr_t baseAddress, const std::array<uintptr_t, N>& offsets)
{
  uintptr_t addr = baseAddress;
  for (size_t i = 0; i < N; ++i)
  {
    if (!ReadProcessMemory(handleManager.GetHandle(), reinterpret_cast<LPCVOID>(addr), &addr, sizeof(addr), NULL))
    {
      OutputDebugString(L"Failed to read memory\n");
      return NULL;
    }

    addr += offsets[i];
  }
  return addr;
}

/// Wrapper for ReadProcessMemory
template<typename T>
T MemTools::Read(uintptr_t address)
{
  T val;
  if (!ReadProcessMemory(handleManager.GetHandle(), reinterpret_cast<LPCVOID>(address), &val, sizeof(T), NULL))
  {
    OutputDebugString(L"Failed to read memory\n");
    return NULL;
  }
  return val;
}

/// Wrapper for WriteProcessMemory
template<typename T>
bool MemTools::Write(uintptr_t address, T& val)
{
  if (!WriteProcessMemory(handleManager.GetHandle(), reinterpret_cast<LPVOID>(address), &val, sizeof(T), NULL))
  {
    OutputDebugString(L"Failed to write memory\n");
    return false;
  }
}