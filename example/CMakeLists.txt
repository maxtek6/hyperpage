FetchContent_Declare(
    libevent
    GIT_REPOSITORY https://github.com/libevent/libevent.git
    GIT_TAG master)
set(EVENT__LIBRARY_TYPE STATIC CACHE STRING "" FORCE)
option(EVENT__DISABLE_TESTS "" ON)
option(EVENT__DISABLE_REGRESS "" ON)
option(EVENT__DISABLE_SAMPLES "" ON)
FetchContent_MakeAvailable(libevent)

FetchContent_Declare(
    sigfn
    GIT_REPOSITORY https://github.com/maxtek6/sigfn.git
    GIT_TAG master)
FetchContent_MakeAvailable(sigfn)

execute_process(
    COMMAND npm install
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/react-app
    RESULT_VARIABLE NPM_RESULT
)

if(NOT NPM_RESULT EQUAL 0)
    message(FATAL_ERROR "npm install failed with code ${NPM_RESULT}")
endif()

add_executable(server ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(server
    PRIVATE
    sigfn_a
    event
    event_core
    hyperpage
)

add_custom_command(
    TARGET server PRE_BUILD
    COMMAND npm run build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/react-app
    COMMENT "Building React frontend"
)

# Traditional approach using custom commands
add_custom_command(
    TARGET server POST_BUILD
    COMMAND $<TARGET_FILE:hyperpack> -o $<TARGET_FILE_DIR:server>/hyperpage.db ${CMAKE_CURRENT_SOURCE_DIR}/react-app/dist
    COMMENT "Building hyperpack archive from React app dist folder"
)

add_dependencies(server hyperpack)

# Alternative approach using the new hyperpage_add_archive function:
# This would replace the above custom command and dependency:
#
# hyperpage_add_archive(react_content "${CMAKE_CURRENT_SOURCE_DIR}/react-app/dist")
# add_dependencies(server react_content)
# 
# Then in the C++ code, you could access the archive at:
# get_target_property(ARCHIVE_FILE react_content HYPERPAGE_ARCHIVE_FILE)
# Or simply use: ${CMAKE_CURRENT_BINARY_DIR}/react_content.db