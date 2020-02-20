set(TargetName "cmakeTest")

file(GLOB_RECURSE TargetSrc
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)

source_group(PREFIX "" FILES ${TargetSrc} TREE ${CMAKE_CURRENT_LIST_DIR})

add_executable(${TargetName} ${TargetSrc})

target_include_directories(${TargetName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Src/Lib)

target_link_libraries(${TargetName} Widget)
target_link_libraries(${TargetName} Qt5::Widgets)