include(${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/HCppBox/master/hbox/cmake/FindMATIEC.cmake)

if(NOT MATIEC_IEC2C_FOUND)
    message(FATAL_ERROR "iec2c must be existed!")
else()
    message(STATUS "iec2c: ${IEC2C}")
endif()

#添加C语言头文件
include_directories(
${MATIEC_C_HEADER}
${CMAKE_CURRENT_LIST_DIR}
${CMAKE_CURRENT_LIST_DIR}/softplc
)
#启用hsoftplc组件
add_compile_options( -DHSOFTPLC=1 )

#添加HCppBox库
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/HCppBox/master/hbox" HBox EXCLUDE_FROM_ALL)


#softplc.st
add_custom_target(softplc_st ALL
                 SOURCES ${CMAKE_CURRENT_LIST_DIR}/softplc.st
                 COMMAND ${IEC2C} -p -l -n -r -R -a -b -c -I ${MATIEC_IEC_HEADER} -T ${CMAKE_CURRENT_LIST_DIR}/softplc ${CMAKE_CURRENT_LIST_DIR}/softplc.st
                 )

##softplc.st库文件
file(GLOB SOFTPLC_ST_PLC_C_FILES ${CMAKE_CURRENT_LIST_DIR}/softplc/Config*.c ${CMAKE_CURRENT_LIST_DIR}/softplc/Res*.c ${CMAKE_CURRENT_LIST_DIR}/softplc/*.h)
add_library(softplc_st_plc ${SOFTPLC_ST_PLC_C_FILES})
##按照unix的代码编译,防止找不到time_t定义
target_compile_definitions(softplc_st_plc PRIVATE -D__unix__ )
##对转换的代码关闭警告
target_compile_options(softplc_st_plc PRIVATE -w )
add_dependencies(softplc_st_plc softplc_st)
set_property(TARGET softplc_st_plc PROPERTY CXX_STANDARD 11)
set_property(TARGET softplc_st_plc PROPERTY C_STANDARD 11)
target_link_libraries(HBox softplc_st_plc)
