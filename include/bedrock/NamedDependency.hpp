/*
 * (C) 2023 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __BEDROCK_NAMED_DEPENDENCY_H
#define __BEDROCK_NAMED_DEPENDENCY_H

#include <string>
#include <memory>
#include <functional>
#include <any>

namespace bedrock {

/**
 * @brief NamedDependency is a parent class for any object
 * that can be a dependency to another one, including providers,
 * provider handles, Argobots pools, etc.
 *
 * It abstract their internal handle as a void* with a
 * release function to call when the dependency is no longer used.
 *
 * If the dependency is an Argobots pool, getHandle<thallium::pool> can be used.
 * If the dependency is an Argobots xstream, getHandle<thallium::xstream> can be used.
 * If the dependency is a provider handle, getHandle<thallium::provider_handle> can be used.
 * If the dependency is a provider, the handle will contain a ComponentPtr,
 * from which ->getHandle() can be called to get the underlying actual handle to a provider (as a void*).
 */
class NamedDependency {

    public:

    template<typename T>
    NamedDependency(std::string name, std::string type, T&& handle)
    : m_name(std::move(name))
    , m_type(std::move(type))
    , m_handle(std::forward<T>(handle)) {}

    NamedDependency(NamedDependency&& other) = default;

    virtual ~NamedDependency() = default;

    const std::string& getName() const {
        return m_name;
    }

    const std::string& getType() const {
        return m_type;
    }

    template<typename H> H getHandle() const {
        return std::any_cast<H>(m_handle);
    }

    protected:

    std::string                m_name;
    std::string                m_type;
    void*                      m_handle;
};

class ProviderDependency : public NamedDependency {

    public:

    template<typename T>
    ProviderDependency(std::string name, std::string type, T handle, uint16_t provider_id)
    : NamedDependency(std::move(name), std::move(type), std::move(handle))
    , m_provider_id(provider_id) {}

    uint16_t getProviderID() const {
        return m_provider_id;
    }

    protected:

    uint16_t m_provider_id;
};

} // namespace bedrock

#endif
