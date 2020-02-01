cmake_minimum_required(VERSION 3.12)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE TYPE INTERNAL FORCE)
project(vtk_reader)

# Testado com Visual Studio 2015 - Qt 5.12.2 - VTK 7.1.1

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_SUPPRESS_REGENERATION true) #Disable zero_check

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

#Compiler options
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Qt5 REQUIRED Widgets)
find_package(VTK REQUIRED)

include(${VTK_USE_FILE})
include_directories(${VTK_INCLUDE_DIRS})

set(project_sources
Main.cxx
MainWindow.cxx
)

set(project_headers
MainWindow.h
)

set(project_ui
MainWindow.ui
)

file (GLOB_RECURSE Resources *.qrc)
qt5_add_resources(Resources ${Resources})
#add Resources no executavel

qt5_wrap_ui(ui_wrap ${project_ui})

#set  (VTK_LIBRARIES   vtkRendering vtkGraphics vtkHybrid QVTK)

add_executable(${PROJECT_NAME}
${project_sources}
${project_headers}
${ui_wrap}
${Resources}
)

target_link_libraries(${PROJECT_NAME}
Qt5::Widgets
${VTK_LIBRARIES}
)

SOURCE_GROUP("Ui" FILES
${project_ui}
)

IF(WIN32)
 #SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup ")
 #SET(CMAKE_CXX_FLAGS /D_CRT_SECURE_NO_WARNINGS /EHsc)
ENDIF(WIN32)

set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/bin)
