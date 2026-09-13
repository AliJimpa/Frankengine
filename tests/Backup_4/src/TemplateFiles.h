#pragma once
#include <string>

// All generated file contents live here as small template functions,
// keeping ProjectManager.cpp focused on filesystem orchestration.
namespace Templates
{
    std::string TasksJson();
    std::string LaunchJson(const std::string& projectName);
    std::string CppPropertiesJson();
    std::string BuildScriptSh(const std::string& projectName);
    std::string BuildScriptBat(const std::string& projectName);
    std::string MainCpp(const std::string& projectName);
    std::string ConfigIni(const std::string& projectName);
    std::string ReadmeMd(const std::string& projectName);
    std::string GitIgnore();
    std::string LibraryHeaderStub(const std::string& libraryName);
}
