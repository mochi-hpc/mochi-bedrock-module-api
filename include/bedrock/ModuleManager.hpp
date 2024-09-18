/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __BEDROCK_MODULE_MANAGER_HPP
#define __BEDROCK_MODULE_MANAGER_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>

namespace bedrock {

class AbstractComponent;
struct ComponentArgs;
struct Dependency;

/**
 * @brief The ModuleManager class contains functions to load modules.
 * Since modules are handled by dynamic libraries, the ModuleManager
 * class provides only static functions and manages these dynamic libraries
 * globally.
 */
class ModuleManager {

  public:
    ModuleManager() = delete;

    using RegisterFn = std::function<std::shared_ptr<AbstractComponent>(const ComponentArgs&)>;
    using GetDependenciesFn = std::function<std::vector<Dependency>(const ComponentArgs&)>;

    /**
     * @brief Register a module that is already in the program's memory.
     *
     * @param moduleName Module name.
     * @param register_fn Function for registering a new component.
     * @param get_dep_fn Function for getting the expected dependencies of a component.
     *
     * @return true if the module was registered, false if a module with the
     * same name was already present.
     */
    static bool registerModule(
        const std::string& moduleName,
        RegisterFn register_fn,
        GetDependenciesFn get_dep_fn);

    /**
     * @brief Load a module from the specified library file.
     *
     * @param library Library.
     *
     * @return true if the module was registered, false if a module with the
     * same name was already present.
     */
    static bool loadModule(const std::string& library);

    /**
     * @brief Load modules from a JSON-formatted array or string.
     *
     * @param jsonString JSON string.
     */
    static void loadModulesFromJSON(const std::string& jsonString);

    /**
     * @brief Return the current JSON configuration.
     */
    static std::string getCurrentConfig();

    /**
     * @brief Create a component from the designated module.
     */
    static std::shared_ptr<AbstractComponent> createComponent(
        const std::string& modName, const ComponentArgs& args);

    /**
     * @brief Get the dependencies for a designated module.
     */
    static std::vector<Dependency> getDependencies(
        const std::string& modName, const ComponentArgs& args);
};

} // namespace bedrock

#endif
