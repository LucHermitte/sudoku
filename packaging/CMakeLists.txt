include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(SudokuLib_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/SudokuLib"
    CACHE STRING "Path to SudokuLib CMake files")

install(TARGETS SudokuLib EXPORT SudokuLib_Targets
        RUNTIME COMPONENT SudokuLib_Runtime
        LIBRARY COMPONENT SudokuLib_Runtime
        NAMELINK_COMPONENT SudokuLib_Development
        ARCHIVE COMPONENT SudokuLib_Development
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY "${SudokuLib_SOURCE_DIR}/include/" "${SudokuLib_BINARY_DIR}/include/"
        TYPE INCLUDE
        COMPONENT SudokuLib_Development)

if (BUILD_SHARED_LIBS)
    set(type shared)
else ()
    set(type static)
endif ()

install(EXPORT SudokuLib_Targets
        DESTINATION "${SudokuLib_INSTALL_CMAKEDIR}"
        NAMESPACE SudokuLib::
        FILE SudokuLib-${type}-targets.cmake
        COMPONENT SudokuLib_Development)

write_basic_package_version_file(
    SudokuLibConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/SudokuLibConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/SudokuLibConfigVersion.cmake"
        DESTINATION "${SudokuLib_INSTALL_CMAKEDIR}"
        COMPONENT SudokuLib_Development)

# TODO: add additional CPack variables here
include(CPack)

