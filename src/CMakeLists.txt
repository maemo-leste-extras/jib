set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# PNG
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

# the source files
file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "lib/*.h"
        "lib/*.cpp"
        "lib/adblock/*.h"
        "lib/adblock/*.cpp"
        "lib/public_suffix/*.h"
        "lib/public_suffix/*.cpp"
        assets.qrc
        )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network WebEngineWidgets Svg Xml Sql)

set(EXECUTABLE_FLAG)

add_executable(jib ${EXECUTABLE_FLAG} main.cpp
        ${SOURCE_FILES}
        ${RESOURCES}
        )


set_property(TARGET jib PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_include_directories(jib PUBLIC ${Qt6Gui_PRIVATE_INCLUDE_DIRS})

target_include_directories(jib PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${X11_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Widgets_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
        ${Qt6Sql_INCLUDE_DIRS}
        ${Qt6Network_INCLUDE_DIRS}
        ${Qt6Svg_INCLUDE_DIRS}
        ${Qt6Xml_INCLUDE_DIRS}
        ${Qt6WebEngineWidgets_INCLUDE_DIRS}
        ${Qt6WebSockets_INCLUDE_DIRS}
)

add_definitions(${QT_DEFINITIONS})
if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DQT_NO_DEBUG)
else()
    target_compile_definitions(jib PRIVATE DEBUG=1)
endif()

if(MAEMO)
    target_compile_definitions(jib PRIVATE MAEMO=1)
endif()

target_compile_definitions(jib
        PUBLIC
        ${Qt6Core_DEFINITIONS}
        ${Qt6Widgets_DEFINITIONS}
        ${Qt6Gui_DEFINITIONS}
        ${Qt6Network_DEFINITIONS}
        ${Qt6Sql_DEFINITIONS}
        ${Qt6Xml_DEFINITIONS}
        ${Qt6WebSockets_DEFINITIONS}
        ${Qt6WebEngineWidgets_DEFINITIONS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")

target_link_libraries(jib PUBLIC
        ${CMAKE_DL_LIBS}
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::Network
        Qt6::Sql
        Qt6::Svg
        Qt6::Xml
        Qt6::WebEngineWidgets
        Threads::Threads
)

if(X11_FOUND)
    target_link_libraries(jib PUBLIC ${X11_LIBRARIES})
endif()

install(TARGETS jib
        DESTINATION bin
        )
