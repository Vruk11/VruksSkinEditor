# CMakeList.txt : CMake project for VruksSkinEditor, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

option(VSE_USE_GTK_FILE_PICKER	"Disable using xdg-deskop-portals on linux and use GTK's file picker instead"	OFF)

project ("VruksSkinEditor")

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/VruksSkinEditor")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/VruksSkinEditor")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
# Add source to this project's executable.
add_executable (VruksSkinEditor WIN32)
target_link_options(VruksSkinEditor PRIVATE "/SUBSYSTEM:WINDOWS")
target_link_options(VruksSkinEditor PRIVATE "/ENTRY:mainCRTStartup")
else()
add_executable (VruksSkinEditor)
endif()

target_sources(VruksSkinEditor PRIVATE 
	src/main.cpp
	

	src/WindowSections/KeyValuesText.h
	src/WindowSections/KeyValuesText.cpp
	src/WindowSections/MDLFileTab.h
	src/WindowSections/MDLFileTab.cpp
	src/WindowSections/SkinEditorWindow.h
	src/WindowSections/SkinEditorWindow.cpp
	src/WindowSections/SkinTable.h
	src/WindowSections/SkinTable.cpp
	src/WindowSections/TexturesTable.h
	src/WindowSections/TexturesTable.cpp

	src/MDLFile/MDLFile.h
	src/MDLFile/MDLFile.cpp
	src/MDLFile/MDLFileFactory.h
	src/MDLFile/MDLFileFactory.cpp

	src/MDLFile/V48/MDLFile_V48.h
	src/MDLFile/V48/MDLFile_v48.cpp
)

target_include_directories(VruksSkinEditor PRIVATE src/MDLFile)
target_include_directories(VruksSkinEditor PRIVATE src/MDLFile/V48)
target_include_directories(VruksSkinEditor PRIVATE src/WindowSections)

# External

target_sources(VruksSkinEditor PRIVATE
	src/External/imgui/imgui.h
	src/External/imgui/imgui.cpp
	src/External/imgui/imgui_internal.h
	src/External/imgui/imgui_draw.cpp
	src/External/imgui/imgui_tables.cpp
	src/External/imgui/imgui_widgets.cpp
	src/External/imgui/backends/imgui_impl_sdl3.h
	src/External/imgui/backends/imgui_impl_sdl3.cpp
	src/External/imgui/backends/imgui_impl_opengl3.h
	src/External/imgui/backends/imgui_impl_opengl3.cpp
	src/External/imgui/misc/cpp/imgui_stdlib.h
	src/External/imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(VruksSkinEditor PRIVATE src/External/imgui)
target_include_directories(VruksSkinEditor PRIVATE src/External/imgui/misc/cpp)
target_include_directories(VruksSkinEditor PRIVATE src/External/imgui/backends)

target_include_directories(VruksSkinEditor PRIVATE src/External/nativefiledialog-extended/src/include)

target_include_directories(VruksSkinEditor PRIVATE src/External/SDL/include/SDL3)
target_include_directories(VruksSkinEditor PRIVATE src/External/SDL_image/include)

find_package(OpenGL REQUIRED)
target_include_directories(VruksSkinEditor PRIVATE ${OPENGL_INCLUDE_DIRS})
target_link_libraries(VruksSkinEditor PRIVATE ${OPENGL_LIBRARIES})


# Just want PNG for the icon file
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_BMP ON)
set(SDLIMAGE_GIF OFF)
set(SDLIMAGE_JPG OFF)
set(SDLIMAGE_JXL OFF)
set(SDLIMAGE_LBM OFF)
set(SDLIMAGE_PCX OFF)
set(SDLIMAGE_PNG OFF)
set(SDLIMAGE_PNM OFF)
set(SDLIMAGE_QOI OFF)
set(SDLIMAGE_SVG OFF)
set(SDLIMAGE_TGA OFF)
set(SDLIMAGE_TIF OFF)
set(SDLIMAGE_WEBP OFF)
set(SDLIMAGE_XCF OFF)
set(SDLIMAGE_XPM OFF)
set(SDLIMAGE_XV OFF)

if(VSE_USE_GTK_FILE_PICKER)
	set(NFD_PORTAL OFF CACHE BOOL "Use GTK's file picker" FORCE)
else()
	set(NFD_PORTAL ON CACHE BOOL "Use XDG-DESKTOP_PORTAL's org.freedesktop.impl.portal.FileChooser portal for file picking" FORCE)
endif()


add_subdirectory(src/External/SDL)
add_subdirectory(src/External/SDL_image)
target_link_libraries(VruksSkinEditor PRIVATE SDL3::SDL3)
target_link_libraries(VruksSkinEditor PRIVATE SDL3_image::SDL3_image)

add_custom_command(TARGET VruksSkinEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/Icon.bmp  ${CMAKE_CURRENT_BINARY_DIR}/VruksSkinEditor/Icon.bmp
)

#if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#add_custom_command(TARGET VruksSkinEditor POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/src/External/SDL/libSDL3.so  ${CMAKE_CURRENT_BINARY_DIR}/libSDL3.so
#)
#elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#add_custom_command(TARGET VruksSkinEditor POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/src/External/SDL/SDL3.dll  ${CMAKE_CURRENT_BINARY_DIR}/SDL3.dll
#)
#endif()
#install(FILES $<TARGET_RUNTIME_DLLS:SDL3.dll> TYPE BIN)

#file(COPY ${CMAKE_CURRENT_BINARY_DIR}/src/External/SDL/SDL3.dll DESTINATION SDL3.dll)
#configure_file(${CMAKE_CURRENT_BINARY_DIR}/src/External/SDL/SDL3.dll SDL3.dll COPYONLY)

add_subdirectory(src/External/nativefiledialog-extended)
target_link_libraries(VruksSkinEditor PRIVATE nfd)

target_include_directories(VruksSkinEditor PRIVATE src/External/imgui)
target_include_directories(VruksSkinEditor PRIVATE src/External/imgui/misc/cpp)
target_include_directories(VruksSkinEditor PRIVATE src/External/imgui/backends)
target_include_directories(VruksSkinEditor PRIVATE src/External/SDL/include)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VruksSkinEditor PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
