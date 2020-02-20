set(TargetName "Widget")

file(GLOB_RECURSE TargetSrc
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)

source_group(PREFIX "" FILES ${TargetSrc} TREE ${CMAKE_CURRENT_LIST_DIR})

add_library(${TargetName} SHARED ${TargetSrc})

target_link_libraries(${TargetName} Qt5::Widgets)
