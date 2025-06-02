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