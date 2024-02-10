#pragma once

#include <cstdint>
#include <Windows.h>
#include <TlHelp32.h>
#include <string_view>

class Memory
{
private:
    std::uintptr_t processId = 0;
    HANDLE processHandle = nullptr;

public:
    Memory(const std::string_view processName) noexcept
    {
        ::PROCESSENTRY32 entry = { };
        entry.dwSize = sizeof(::PROCESSENTRY32);

        const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        while (::Process32Next(snapShot, &entry))
        {
            if (!processName.compare(entry.szExeFile))
            {
                processId = entry.th32ProcessID;
                processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
                break;
            }
        }

        if (snapShot)
            ::CloseHandle(snapShot);
    }

    ~Memory()
    {
        if (processHandle)
            ::CloseHandle(processHandle);
    }

    const std::uintptr_t GetModuleAddress(const std::string_view moduleName) const noexcept
    {
        ::MODULEENTRY32 entry = { };
        entry.dwSize = sizeof(::MODULEENTRY32);

        const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

        std::uintptr_t result = 0;

        while (::Module32Next(snapShot, &entry))
        {
            if (!moduleName.compare(entry.szModule))
            {
                result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
                break;
            }
        }

        if (snapShot)
            ::CloseHandle(snapShot);

        return result;
    }

    bool IsProcessValid() const noexcept
    {
        return processHandle != nullptr;
    }

    bool ReadRaw(std::uintptr_t address, void* buffer, size_t size)
    {
        if (!IsProcessValid())
            return false;

        SIZE_T bytesRead;
        return ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address), buffer, size, &bytesRead) && (bytesRead == size);
    }

    template <typename T>
    T Read(const std::uintptr_t& address) const noexcept
    {
        T value = { };
        ::ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address), &value, sizeof(T), nullptr);
        return value;
    }

    template <typename T>
    void Write(const std::uintptr_t& address, const T& value) const noexcept
    {
        ::WriteProcessMemory(processHandle, reinterpret_cast<void*>(address), &value, sizeof(T), nullptr);
    }
};
