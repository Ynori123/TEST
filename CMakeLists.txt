cmake_minimum_required(VERSION 3.10)
project(g4-ionization-stack)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- Geant4 ----------
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# ---------- ROOT ----------
set(ROOT_DIR $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree)
include_directories(${ROOT_INCLUDE_DIRS})

# ---------- Qt5（可视化用）----------
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
set(CMAKE_AUTOMOC ON)          # 让 CMake 自动处理 Qt 的元对象系统
add_definitions(-DGEANT4_USE_QT)

# ---------- 头文件 ----------
include_directories(include)

# ---------- 源文件 ----------
set(SOURCES
    src/main.cc
    src/ActionInitialization.cc
    src/DetectorConstruction.cc
    src/PrimaryGeneratorAction.cc
    src/RunAction.cc
    src/EventAction.cc
    src/SteppingAction.cc
    src/RootIO.cc
)

# ---------- 可执行目标 ----------
add_executable(${PROJECT_NAME} ${SOURCES})

# ---------- 链接库 ----------
if(TARGET ROOT::Core)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${Geant4_LIBRARIES}
        ROOT::Core ROOT::RIO ROOT::Tree
        Qt5::Core Qt5::Gui Qt5::Widgets
    )
else()
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${Geant4_LIBRARIES}
        ${ROOT_LIBRARIES}
        Qt5::Core Qt5::Gui Qt5::Widgets
    )
endif()

# ---------- 安装 ----------
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# ---------- 自动复制宏文件 ----------
file(GLOB MACRO_FILES "${CMAKE_SOURCE_DIR}/macros/*.mac")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/macros")
foreach(mac ${MACRO_FILES})
    configure_file(${mac} "${CMAKE_BINARY_DIR}/macros" COPYONLY)
endforeach()

