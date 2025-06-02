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

add_custom_command(
    TARGET server POST_BUILD
    COMMAND $<TARGET_FILE:hyperpack> -o $<TARGET_FILE_DIR:server>/hyperpage.db ${CMAKE_CURRENT_SOURCE_DIR}/react-app/dist
    COMMENT "Building hyperpack archive from React app dist folder"
)

add_dependencies(server hyperpack)