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

add_executable(example ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(example
    PRIVATE
    sigfn_a
    event
    event_core
    hyperpage
)

add_custom_command(
    TARGET example PRE_BUILD
    COMMAND npm run build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/react-app
    COMMENT "Building React frontend"
)

add_custom_command(
    TARGET example POST_BUILD
    COMMAND $<TARGET_FILE:hyperpack> -o $<TARGET_FILE_DIR:example>/hyperpage.db ${CMAKE_CURRENT_SOURCE_DIR}/react-app/dist
    COMMENT "Building hyperpack archive from React app dist folder"
)

add_dependencies(example hyperpack)