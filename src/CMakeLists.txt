# set source files
set (lib-src-files
     ModuleManager.cpp)

# load package helper for generating cmake CONFIG packages
include (CMakePackageConfigHelpers)

# where to install files for "find_package"
set (bedrock-module-api-pkg "share/cmake/bedrock-module-api")

# set version fo shared library
set (bedrock-module-api-vers "${BEDROCK_MODULE_API_VERSION_MAJOR}.${BEDROCK_MODULE_API_VERSION_MINOR}")

# library
add_library (bedrock-module-api ${lib-src-files})
add_library (bedrock::module-api ALIAS bedrock-module-api)
target_compile_options (bedrock-module-api PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries (bedrock-module-api
    PRIVATE nlohmann_json::nlohmann_json
    PUBLIC
    thallium
    spdlog::spdlog
    fmt::fmt)
target_include_directories (bedrock-module-api PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (bedrock-module-api BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (bedrock-module-api BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (bedrock-module-api
    PROPERTIES VERSION ${BEDROCK_MODULE_API_VERSION}
    SOVERSION ${BEDROCK_MODULE_API_VERSION_MAJOR})

# installation stuff (packaging and install commands)
write_basic_package_version_file (
    "bedrock-module-api-config-version.cmake"
    VERSION ${BEDROCK_MODULE_API_VERSION}
    COMPATIBILITY AnyNewerVersion)

# generate our config file for find_package()
set (INCLUDE_INSTALL_DIR include/)
set (LIB_INSTALL_DIR lib/)
configure_package_config_file (bedrock-module-api-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/bedrock-module-api-config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/bedrock-module-api/cmake
  PATH_VARS INCLUDE_INSTALL_DIR)

# some bits for the pkg-config file
set (DEST_DIR "${CMAKE_INSTALL_PREFIX}")
set (PRIVATE_LIBS "-lbedrock-module-api")
configure_file ("bedrock-module-api.pc.in" "bedrock-module-api.pc" @ONLY)

# "make install" rules
install (TARGETS bedrock-module-api
         EXPORT bedrock-module-api-targets
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
install (EXPORT bedrock-module-api-targets
         DESTINATION ${bedrock-module-api-pkg}
         FILE "bedrock-module-api-targets.cmake")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/bedrock-module-api-config.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/bedrock-module-api-config-version.cmake"
         DESTINATION ${bedrock-module-api-pkg} )
install (DIRECTORY ../include/bedrock
         DESTINATION include
         FILES_MATCHING PATTERN "*.hpp")
install (DIRECTORY ../include/bedrock
         DESTINATION include
         FILES_MATCHING PATTERN "*.h")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/bedrock-module-api.pc"
         DESTINATION "lib/pkgconfig/")
