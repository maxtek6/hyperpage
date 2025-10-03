get_filename_component(HYPERPAGE_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

list(APPEND CMAKE_MODULE_PATH ${HYPERPAGE_CMAKE_DIR})

if(NOT TARGET hyperpage::hyperpage)
    include("${HYPERPAGE_CMAKE_DIR}/hyperpage-targets.cmake")
endif()

set(hyperpage_LIBRARIES hyperpage::hyperpage)
