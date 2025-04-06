cmake_minimum_required(VERSION 3.24...3.30)
project(raylib-game-template)

include(FetchContent)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
# Dependencies
set(RAYLIB_VERSION 5.0)
set(CMAKE_POLICY_VERSION_MINIMUM "3.5")
FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(raylib)

# Our Project
add_executable(${PROJECT_NAME})
add_subdirectory(src)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

if ("${PLATFORM}" STREQUAL "Web")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
    )
    #DEPENDS ${PROJECT_NAME}
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    )
    #DEPENDS ${PROJECT_NAME}
endif()
# 非Web平台资源复制
if(NOT "${PLATFORM}" STREQUAL "Web")
    # 确保资源目录存在
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/resources")
        # 列出所有资源文件
        # file(GLOB RESOURCE_FILES "${CMAKE_SOURCE_DIR}/src/resources/*")
        add_custom_target(copy_resources ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
            COMMENT "Copying resource files..."
            VERBATIM
        )

        # 复制资源到输出目录（仅变化时复制）
        # add_custom_command(
        #     TARGET ${PROJECT_NAME} POST_BUILD
        #     COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
        #     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RESOURCE_FILES} $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
        #     COMMENT "Updating resource files..."
        #     DEPENDS ${RESOURCE_FILES}
        # )
    endif()
else()
    # Web平台的特殊处理
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/resources")
        file(GLOB RESOURCE_FILES "${CMAKE_SOURCE_DIR}/src/resources/*")
        add_custom_command(
            TARGET ${PROJECT_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RESOURCE_FILES} $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
            COMMENT "Updating web resource files..."
            DEPENDS ${RESOURCE_FILES}
        )
    endif()
endif()
#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)

# Web Configurations
if ("${PLATFORM}" STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PUBLIC -sUSE_GLFW=3 PUBLIC --preload-file resources)
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
