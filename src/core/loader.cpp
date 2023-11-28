#include "loader.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

modLoader::modLoader()
{
    printf("Created Mod Loader!\n");
}

modLoader::~modLoader()
{
    printf("Destroyed Mod Loader!\n");
    invokeEachModDestructor();
}

bool modLoader::Load(const std::string& path) 
{
    // handle the mods folder not existing
    if (!fs::exists(path))
    {
        fs::create_directory(path);
    }
    printf("Loading mods from %s\n", path.c_str());
    // Move all the mods to a temp folder
    auto tempPath = path + "\\temp";
    moveModsToTempFolder(path, tempPath);
    getEachModPath(tempPath);
    injectEachMod();
    return true;
}

void modLoader::getEachModPath(const std::string& basePath)
{
    printf("Getting each mod path from %s\n", basePath.c_str());

    for (const auto& entry : fs::directory_iterator(basePath))
    {
        if (entry.path().extension() == ".dll")
        {
            printf("Found mod: %s\n", entry.path().c_str());
            _eachModPath.push_back(entry.path().string()); // Store the path as a std::string
        }
        else if (fs::is_directory(entry.path()))
        {
            // Recursively call getEachModPath with the subdirectory path
            auto subDirPath = entry.path().string();
            getEachModPath(subDirPath);
        }
    }
}

void modLoader::injectEachMod()
{
    printf("Injecting each mod\n");
    getMinecraftWindowHandle(); // We can bin the return value because we already have it in _minecraftWindowHandle
    for (int i = 0; i < _eachModPath.size(); i++)
    {
        injectSingleMod(_eachModPath[i]);
    }
}

void modLoader::invokeEachModMain()
{
    printf("Invoking each mod main\n");
    for (int i = 0; i < _eachModPath.size(); i++)
    {
        invokeModMain(_eachModPath[i]);
    }
}

void modLoader::invokeEachModDestructor()
{
    printf("Invoking each mod destructor\n");
    for (int i = 0; i < _eachModPath.size(); i++)
    {
        invokeModDestructor(_eachModPath[i]);
    }
    // Clear the map
    _modHandles.clear();
    // faster than releasing 1 at a time due to the map not needing to be resorted
}

HANDLE modLoader::getMinecraftWindowHandle()
{
    printf("Getting minecraft window handle\n");
    HWND hwnd = FindWindowExW(FindWindowW(L"ApplicationFrameWindow", L"Minecraft"), NULL, NULL, L"Minecraft");
    if (hwnd != NULL)
    {
        printf("Found minecraft window handle: %p\n", hwnd);
    }
    else
    {
        // if failed start the app using explorer.exe shell:appsFolder\\Microsoft.MinecraftUWP_8wekyb3d8bbwe!App
        // run a shell command to start the app
        system("explorer.exe shell:appsFolder\\Microsoft.MinecraftUWP_8wekyb3d8bbwe!App");
        // wait for the app to start
        while (hwnd == NULL)
        {
            hwnd = FindWindowExW(FindWindowW(L"ApplicationFrameWindow", L"Minecraft"), NULL, NULL, L"Minecraft");
        }
    }

    DWORD procID;
    GetWindowThreadProcessId(hwnd, &procID);
    HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    _minecraftWindowHandle = procHandle;
    return procHandle;
}

void modLoader::injectSingleMod(const std::string& path)
{
    printf("Injecting mod: %s\n", path.c_str());
    LPVOID dll = VirtualAllocEx(_minecraftWindowHandle, 0, path.size(), MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(_minecraftWindowHandle, dll, path.c_str() + 1, path.size(), 0);
    HANDLE hLoadThread = CreateRemoteThread(
        _minecraftWindowHandle,
        0,
        0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"),
        dll,
        0,
        0
    );
    WaitForSingleObject(hLoadThread, INFINITE);
    // add the handle to the map
    _modHandles[path] = dll;
    // VirtualFreeEx(_minecraftWindowHandle, dll, path.size() + 1, MEM_RELEASE);
}

void modLoader::invokeModMain(const std::string& path)
{
    printf("Invoking mod main: %s\n", path.c_str());
    // Get the mod handle from the map
    LPVOID modHandle = _modHandles[path];
    // Get the mod main function
    LPVOID modMain = GetProcAddress((HMODULE)modHandle, "InitSBLMod");
    // Create a thread to run the mod main function
    HANDLE hModMainThread = CreateRemoteThread(
        _minecraftWindowHandle,
        0,
        0,
        (LPTHREAD_START_ROUTINE)modMain,
        modHandle,
        0,
        0
    );
    WaitForSingleObject(hModMainThread, INFINITE);
}

void modLoader::invokeModDestructor(const std::string& path)
{
    printf("Invoking mod destructor: %s\n", path.c_str());
    // Get the mod handle from the map
    LPVOID modHandle = _modHandles[path];
    // Get the mod destructor function
    LPVOID modDestructor = GetProcAddress((HMODULE)modHandle, "DestroySBLMod");
    // Create a thread to run the mod destructor function
    HANDLE hModDestructorThread = CreateRemoteThread(
        _minecraftWindowHandle,
        0,
        0,
        (LPTHREAD_START_ROUTINE)modDestructor,
        modHandle,
        0,
        0
    );
    WaitForSingleObject(hModDestructorThread, INFINITE);
    // Free the mod handle
    VirtualFreeEx(_minecraftWindowHandle, modHandle, path.size() + 1, MEM_RELEASE);
}

void modLoader::moveModsToTempFolder(const std::string& basePath, const std::string& tempPath)
{
    printf("Moving mods to temp folder\n");
    // create the temp folder if it doesn't exist
    if (!fs::exists(tempPath))
    {
        //fs::create_directory(tempPath);
        fs::create_directories(tempPath);
    }
    for (const auto& entry : fs::directory_iterator(basePath))
    {
        if (entry.path().extension() == ".dll")
        {
            printf("Found mod: %s\n", entry.path().c_str());
            fs::copy(entry.path(), tempPath);
        }
        else if (fs::is_directory(entry.path()))
        {
            // Recursively call getEachModPath with the subdirectory path
            auto subDirPath = entry.path().string();
            moveModsToTempFolder(subDirPath, tempPath);
        }
    }
}