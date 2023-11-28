#pragma once
#include <string>
#include <vector>
#include <map>
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN

class modLoader
{
private:
    std::vector<std::string> _eachModPath;
    HANDLE _minecraftWindowHandle;
    std::map<std::string, LPVOID> _modHandles;

private:
    // This can get or CREATE the window handle for minecraft bedrock
    HANDLE getMinecraftWindowHandle();
    void getEachModPath(const std::string& basePath);
    void injectSingleMod(const std::string& path);
    void injectEachMod();
    void invokeModMain(const std::string& path);
    void invokeEachModMain();
    void invokeModDestructor(const std::string& path);
    void invokeEachModDestructor();
    void moveModsToTempFolder(const std::string& basePath, const std::string& tempPath);
public:
    modLoader();
    ~modLoader();

    bool Load(const std::string& path); // returns false if failed
    inline void InvokeMain() {invokeEachModMain();};
    inline void InvokeDestructor() {invokeEachModDestructor();}; 
};



