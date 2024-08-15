/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __BEDROCK_ABSTRACT_COMPONENT_HPP
#define __BEDROCK_ABSTRACT_COMPONENT_HPP

#include <bedrock/ModuleManager.hpp>
#include <bedrock/Exception.hpp>
#include <bedrock/NamedDependency.hpp>
#include <thallium.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cstdint>

/**
 * @brief Helper class to register module types into the ModuleContext.
 */
template <typename AbstractComponentFactoryType>
class __BedrockAbstractComponentFactoryRegistration;

namespace bedrock {

/**
 * @brief The Dependency structure specifies a dependency that the component
 * may need at initialization time.
 *
 * - name: name by which the dependency is known in the "dependencies" section
 *         of a component.
 * - type: type of the dependency.
 * - is_required: whether the dependency is required.
 * - is_array: whether the dependency can be an array. If is_required is true,
 *             the array should have at least one element.
 * - is_updatable: whether the dependency can be updated using changeDependency.
 */
struct Dependency {
    std::string name;
    std::string type;
    bool        is_required;
    bool        is_array;
    bool        is_updatable;
};

typedef std::vector<std::shared_ptr<NamedDependency>> NamedDependencyList;

typedef std::unordered_map<std::string, NamedDependencyList> ResolvedDependencyMap;

/**
 * @brief This structure is passed to a factory's registerComponent function
 * to provide the factory with relevant information to initialize the component.
 * The C equivalent of this structure is the bedrock_args_t handle.
 */
struct ComponentArgs {
    std::string              name;         // name of the component
    thallium::engine         engine;       // thallium engine
    uint16_t                 provider_id;  // provider id
    std::vector<std::string> tags;         // Tags
    std::string              config;       // JSON configuration
    ResolvedDependencyMap    dependencies; // dependencies
};

/**
 * @brief Abstract component interface used by Bedrock modules.
 *
 * To create a component usable by Bedrock, component implementers
 * should write a class that inherits from AbstractComponent and
 * overwrite its virtual functions. The class should also provide the following
 * static functions:
 *
 * static std::shared_ptr<AbstractComponent> Register(const ComponentArgs& args)
 *      Registers a component from the specified ComponentArgs.
 *
 * static std::vector<Dependency> GetDependencies(const ComponentArgs& args)
 *      Returns the list of expected dependencies from the given ComponentArgs.
 *      The args.dependency field will (obviously) not have been set yet.
 *
 * Finally, the following should be put in a .cpp file
 * BEDROCK_REGISTER_COMPONENT_TYPE(mymodule, MyComponent)
 * and the file should be built as a shared library.
 */
class AbstractComponent {

    public:

    /**
     * @brief Destructor.
     */
    virtual ~AbstractComponent() = default;

    /**
     * @brief Get the configuration of a component.
     *
     * @return the string configuration.
     */
    virtual std::string getConfig() {
        return "{}";
    }

    /**
     * @brief Get a void* handle that can be used by other components
     * when resolving dependencies to this type of provider.
     */
    virtual void* getHandle() = 0;

    /**
     * @brief Change a dependency used by a component.
     *
     * @param dep_name Name of the dependency to change.
     * @param dependencies New dependencies.
     *
     * This function may throw a bedrock::Exception if the change was not possible.
     */
    virtual void changeDependency(const std::string& dep_name,
                                  const NamedDependencyList& dependencies) {
        (void)dep_name;
        (void)dependencies;
        throw Exception{"Changing dependencies not supported for this component"};
    }

    /**
     * @brief Migrates the state of the designated component.
     *
     * @param dest_addr Destination address.
     * @param dest_component_id Destination component ID.
     * @param options_json JSON-formatted parameters.
     * @param remove_source Whether to remove the source.
     */
    virtual void migrate(
            const std::string& dest_addr,
            uint16_t dest_component_id,
            const std::string& options_json,
            bool remove_source) {
        (void)dest_addr;
        (void)dest_component_id;
        (void)options_json;
        (void)remove_source;
        throw Exception{"Migration not supported for this component"};
    }

    /**
     * @brief Snapshots the state of the designated component.
     *
     * @param dest_path Destination directory.
     * @param options_json JSON-formatted parameters.
     * @param remove_source Whether to remove the source.
     */
    virtual void snapshot(
            const std::string& dest_path,
            const std::string& options_json,
            bool remove_source) {
        (void)dest_path;
        (void)options_json;
        (void)remove_source;
        throw Exception{"Snapshot not supported for this component"};
    }

    /**
     * @brief Restores the state of the designated component.
     *
     * @param component Component to snapshot.
     * @param src_path Source directory.
     * @param options_json JSON-formatted parameters.
     */
    virtual void restoreComponent(
            void* component, const std::string& src_path,
            const char* options_json) {
        (void)component;
        (void)src_path;
        (void)options_json;
        throw Exception{"Restore not supported for this component"};
    }
};

} // namespace bedrock

#define BEDROCK_REGISTER_COMPONENT_TYPE(__module_name, __component_type) \
    __BedrockAbstractComponentFactoryRegistration<__component_type>        \
        __bedrock##__module_name##_module(#__module_name);

template <typename AbstractComponentType>
class __BedrockAbstractComponentFactoryRegistration {

  public:
    __BedrockAbstractComponentFactoryRegistration(const std::string& moduleName) {
        bedrock::ModuleManager::registerModule(
            moduleName,
            &AbstractComponentType::Register,
            &AbstractComponentType::GetDependencies);
    }
};

#endif
