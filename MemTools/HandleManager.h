#pragma once
#include "pch.h"

class HandleManager
{
public:
  HandleManager();
  explicit HandleManager(HANDLE h);
  ~HandleManager();
  HandleManager(const HandleManager&) = delete;
  HandleManager& operator=(const HandleManager&) = delete;
  HandleManager(HandleManager&& other) noexcept;
  HandleManager& operator=(HandleManager&& other) noexcept;
  HANDLE GetHandle() const;
  void SetHandle(HANDLE h);

private:
  HANDLE handle;
};