/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include <bedrock/ModuleManager.hpp>
#include <bedrock/AbstractComponent.hpp>
#include <bedrock/DetailedException.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <dlfcn.h>
#include <unordered_map>

namespace bedrock {

using nlohmann::json;

static std::vector<std::string> s_loaded_libraries;
static std::unordered_map<std::string, ModuleManager::RegisterFn> s_register_fn;
static std::unordered_map<std::string, ModuleManager::GetDependenciesFn> s_get_dep_fn;

bool ModuleManager::registerModule(const std::string&  moduleName,
                                   ModuleManager::RegisterFn register_fn,
                                   ModuleManager::GetDependenciesFn get_dep_fn) {
    spdlog::trace("Registering module {}", moduleName);
    if(s_register_fn.count(moduleName)) {
        spdlog::error("Module {} was already registered", moduleName);
        return false;
    }
    s_register_fn.insert(std::make_pair(moduleName, std::move(register_fn)));
    s_get_dep_fn.insert(std::make_pair(moduleName, std::move(get_dep_fn)));
    return true;
}

bool ModuleManager::loadModule(const std::string& library) {
    if (std::find(s_loaded_libraries.begin(),
                  s_loaded_libraries.end(),
                  library) ==  s_loaded_libraries.end()) {
        return false;
    }
    spdlog::trace("Loading module(s) from library {}", library);

    void* handle = nullptr;
    if (library == "")
        handle = dlopen(nullptr, RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
    else
        handle = dlopen(library.c_str(), RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
    // XXX we use RTLD_NODELETE to prevent the symbols from being deleted
    // when shutting down, allowing ASAN to detect leaks in module libraries.
    // However if we ever want to add the possibility to unload and reload libraries,
    // we will need something better than this.
    if (!handle)
        throw BEDROCK_DETAILED_EXCEPTION("Could not dlopen library {}: {}", library, dlerror());
    s_loaded_libraries.push_back(library);
    return true;
}

void ModuleManager::loadModulesFromJSON(const std::string& jsonString) {
    auto libraries = json::parse(jsonString);
    if (libraries.is_null()) return;
    if (libraries.is_string()) loadModule(libraries.get_ref<const std::string&>());
    if (libraries.is_array()) {
        for(auto& lib : libraries) {
            if(!lib.is_string()) {
                throw BEDROCK_DETAILED_EXCEPTION("Module library should be a string");
            }
        }
        for(auto& lib : libraries) loadModule(lib.get_ref<const std::string&>());
    }
}

std::string ModuleManager::getCurrentConfig() {
    std::string config = "[";
    unsigned    i      = 0;
    for (const auto& m : s_loaded_libraries) {
        config += "\"" + m + "\"";
        i += 1;
        if (i < s_loaded_libraries.size()) config += ",";
    }
    config += "]";
    return config;
}

std::shared_ptr<AbstractComponent> ModuleManager::createComponent(
        const std::string& modName, const ComponentArgs& args) {
    auto it = s_register_fn.find(modName);
    if(it == s_register_fn.end()) {
        throw bedrock::Exception{
            std::string{"Could not find registration function for module \""}
            + modName + "\""};
    }
    return it->second(args);
}

std::vector<Dependency> ModuleManager::getDependencies(
        const std::string& modName, const ComponentArgs& args) {
    auto it = s_get_dep_fn.find(modName);
    if(it == s_get_dep_fn.end()) {
        throw bedrock::Exception{
            std::string{"Could not find registration function for module \""}
            + modName + "\""};
    }
    return it->second(args);
}

} // namespace bedrock
