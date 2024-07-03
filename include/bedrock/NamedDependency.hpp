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

namespace bedrock {

/**
 * @brief NamedDependency is a parent class for any object
 * that can be a dependency to another one, including providers,
 * provider handles, clients, SSG groups, ABT-IO instances,
 * Argobots pools, etc.
 *
 * It abstract their internal handle as a void* with a
 * release function to call when the dependency is no longer used.
 */
class NamedDependency {

    public:

    using ReleaseFn = std::function<void(void*)>;

    template<typename T>
    NamedDependency(std::string name, std::string type, T handle, ReleaseFn release)
    : m_name(std::move(name))
    , m_type(std::move(type))
    , m_handle(reinterpret_cast<void*>(handle))
    , m_release(std::move(release)) {}

    NamedDependency(NamedDependency&& other)
    : m_name(std::move(other.m_name))
      , m_handle(other.m_handle)
      , m_release(std::move(other.m_release)) {
          other.m_handle = nullptr;
          other.m_release = ReleaseFn{};
      }

    virtual ~NamedDependency() {
        if(m_release) m_release(m_handle);
    }

    const std::string& getName() const {
        return m_name;
    }

    const std::string& getType() const {
        return m_type;
    }

    template<typename H> H getHandle() const {
        return reinterpret_cast<H>(m_handle);
    }

    protected:

    std::string                m_name;
    std::string                m_type;
    void*                      m_handle;
    std::function<void(void*)> m_release;
};

class ProviderDependency : public NamedDependency {

    public:

    template<typename T>
    ProviderDependency(std::string name, std::string type, T handle, ReleaseFn release, uint16_t provider_id)
    : NamedDependency(std::move(name), std::move(type), std::move(handle), std::move(release))
    , m_provider_id(provider_id) {}

    uint16_t getProviderID() const {
        return m_provider_id;
    }

    protected:

    uint16_t m_provider_id;
};

} // namespace bedrock

#endif
