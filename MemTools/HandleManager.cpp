#include "pch.h"
#include "HandleManager.h"

HandleManager::HandleManager() : handle(NULL) {}

HandleManager::HandleManager(HANDLE h) : handle(h) {}

HandleManager::~HandleManager()
{
  if (handle && handle != INVALID_HANDLE_VALUE)
  {
    CloseHandle(handle);
  }
}

HandleManager::HandleManager(HandleManager&& other) noexcept : handle(other.handle)
{
  other.handle = NULL;
}

HandleManager& HandleManager::operator=(HandleManager&& other) noexcept
{
  if (this != &other)
  {
    if (handle && handle != INVALID_HANDLE_VALUE)
    {
      CloseHandle(handle);
    }
    handle = other.handle;
    other.handle = NULL;
  }
  return *this;
}

HANDLE HandleManager::GetHandle() const { return handle; }

void HandleManager::SetHandle(HANDLE h)
{
  if (handle && handle != INVALID_HANDLE_VALUE)
  {
    CloseHandle(handle);
  }
  handle = h;
}