cmake_minimum_required(VERSION 3.16.3)

project(rang
    VERSION 3.2.0
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CMakeUtilities.cmake)
include(GNUInstallDirs)
set_verbose(RANG_INC_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE STRING
    "Installation directory for include files, a relative path that "
    "will be joined with ${CMAKE_INSTALL_PREFIX} or an absolute path.")

set(RANG_HEADERS include/rang.hpp)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(rang INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${RANG_INC_DIR}>
  )

include(CMakePackageConfigHelpers)

set_verbose(RANG_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/rang CACHE STRING
  "Installation directory for cmake files, a relative path that "
  "will be joined with ${CMAKE_INSTALL_PREFIX} or an absolute "
  "path.")
set(version_config ${PROJECT_BINARY_DIR}/rang-config-version.cmake)
set(project_config ${PROJECT_BINARY_DIR}/rang-config.cmake)
set(pkgconfig ${PROJECT_BINARY_DIR}/rang.pc)
set(targets_export_name rang-targets)

set_verbose(RANG_PKGCONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/pkgconfig CACHE PATH
  "Installation directory for pkgconfig (.pc) files, a relative "
  "path that will be joined with ${CMAKE_INSTALL_PREFIX} or an "
  "absolute path.")

write_basic_package_version_file(
    ${version_config}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

join_paths(includedir_for_pc_file "\${prefix}" "${RANG_INC_DIR}")

# Configure the PkgConfig
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/rang.pc.in"
    "${pkgconfig}"
    @ONLY
    )

# Configuring the CMake Installer Helper
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/rang-config.cmake.in
    ${project_config}
    INSTALL_DESTINATION ${RANG_CMAKE_DIR}
    )

set(INSTALL_TARGETS rang)

# Install the library and headers.
install(TARGETS ${INSTALL_TARGETS} EXPORT ${targets_export_name}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Use a namespace because CMake provides better diagnostics for namespaced
# imported targets.
export(TARGETS ${INSTALL_TARGETS} NAMESPACE rang::
    FILE ${PROJECT_BINARY_DIR}/${targets_export_name}.cmake
    )

# Install version, config and target files.
install(
    FILES ${project_config} ${version_config}
    DESTINATION ${RANG_CMAKE_DIR}
    )

install(EXPORT ${targets_export_name}
    DESTINATION ${RANG_CMAKE_DIR}
    NAMESPACE rang::)

install(FILES ${RANG_HEADERS} DESTINATION "${RANG_INC_DIR}")
install(FILES "${pkgconfig}" DESTINATION "${RANG_PKGCONFIG_DIR}")
