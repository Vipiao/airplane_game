cmake_minimum_required(VERSION 3.16)
project(AirplaneGame LANGUAGES CXX C)   # C is needed for glad.c

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The compile database an editor's language server reads to parse a file with the
# flags it is really built with.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific flags
if(MSVC)
    add_compile_options(/W4 /O2)
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MD /DNDEBUG")
else()
    # GCC/Clang flags
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unknown-pragmas)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -DNDEBUG")
endif()

# Keep assert() active in optimized builds during development; set OFF for a
# production build (restores the standard NDEBUG semantics).
option(ASSERTS_IN_RELEASE "Keep assert() active in Release builds" ON)
if(ASSERTS_IN_RELEASE)
    foreach(flags_var CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REPLACE "/DNDEBUG" "" ${flags_var} "${${flags_var}}")
        string(REPLACE "-DNDEBUG" "" ${flags_var} "${${flags_var}}")
    endforeach()
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# --- Dependencies -----------------------------------------------------------
# System packages. glad, glm, stb and AudioFile are vendored under external/
# so the tree builds without hunting down header-only libraries.
find_package(assimp REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL)

if(NOT TARGET assimp::assimp)
    include_directories(${ASSIMP_INCLUDE_DIRS})
endif()

# OpenAL headers ship under external/AL as a fallback for platforms without a
# development package installed; a system OpenAL is preferred when present.
if(OpenAL_FOUND OR OPENAL_FOUND)
    set(AIRPLANE_OPENAL_INCLUDE "${OPENAL_INCLUDE_DIR}")
    set(AIRPLANE_OPENAL_LIBRARY "${OPENAL_LIBRARY}")
else()
    message(STATUS "No system OpenAL found; using vendored headers in external/")
    set(AIRPLANE_OPENAL_INCLUDE "${CMAKE_SOURCE_DIR}/external")
    find_library(AIRPLANE_OPENAL_LIBRARY NAMES openal OpenAL32 REQUIRED)
endif()

# Specify output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Game's own includes, plus the vendored third-party headers.
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/stb
    ${AIRPLANE_OPENAL_INCLUDE}
)

# Game sources. The recursive glob is scoped to src/ so stray copies of the
# tree never leak into the build.
#
# CONFIGURE_DEPENDS re-checks the glob at build time, so a new source file
# reaches every build directory and not just the one last reconfigured.
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
set(GLAD_SRC "${CMAKE_SOURCE_DIR}/external/glad/src/glad.c")

# Third-party code builds silently: the warning flags above are
# directory-scoped and would otherwise apply to the generated loader too.
set_source_files_properties(${GLAD_SRC} PROPERTIES COMPILE_OPTIONS "-w")

# Add the executable
add_executable(AirplaneGame ${SRC_FILES} ${GLAD_SRC})

target_link_libraries(AirplaneGame PRIVATE
    glfw
    ${OPENGL_LIBRARIES}
    ${AIRPLANE_OPENAL_LIBRARY}
)
if(TARGET assimp::assimp)
    target_link_libraries(AirplaneGame PRIVATE assimp::assimp)
else()
    target_link_libraries(AirplaneGame PRIVATE ${ASSIMP_LIBRARIES})
endif()

# --- Runtime assets ---------------------------------------------------------
# The game runs with bin/ as its working directory: it opens shaders by bare
# filename ("vertex_shader.vert") and the noise tables as "temp/*.dat", while
# media/ is reached as "../media/...". Mirroring src/ into bin/ at build time
# is what makes those paths resolve, so the source keeps its own layout.
file(GLOB SHADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.vert"
    "${CMAKE_SOURCE_DIR}/src/*.frag"
    "${CMAKE_SOURCE_DIR}/src/*.tcs"
    "${CMAKE_SOURCE_DIR}/src/*.tes"
    "${CMAKE_SOURCE_DIR}/src/*.glsl"
)
file(GLOB NOISE_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/temp/*.dat")

add_custom_command(TARGET AirplaneGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SHADER_FILES} $<TARGET_FILE_DIR:AirplaneGame>
    COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:AirplaneGame>/temp
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${NOISE_FILES} $<TARGET_FILE_DIR:AirplaneGame>/temp
    COMMENT "Staging shaders and noise tables into bin/"
    VERBATIM
)
