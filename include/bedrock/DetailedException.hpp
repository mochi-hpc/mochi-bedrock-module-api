/*
 * (C) 2023 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __BEDROCK_DETAILED_EXCEPTION_H
#define __BEDROCK_DETAILED_EXCEPTION_H

#include <bedrock/Exception.hpp>
#include <mercury.h>
#include <string>

namespace bedrock {

class DetailedException : public Exception {

    public:

    template<typename ... Args>
    DetailedException(int line, const char* filename, Args&& ... args)
    : Exception(std::forward<Args>(args)...)
    , m_location(std::string(filename) + ":" + std::to_string(line)) {}


    const char* details() const noexcept override {
        return m_location.c_str();
    }

    private:

    std::string m_location;

};

#define BEDROCK_DETAILED_EXCEPTION(...) \
    ::bedrock::DetailedException(__LINE__, __FILE__, __VA_ARGS__)

} // namespace bedrock

#endif
