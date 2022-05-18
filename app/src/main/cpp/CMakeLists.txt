cmake_minimum_required(VERSION 3.18.1)

project("skplayground")

set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
set(${CMAKE_CXX_FLAGS}, "${CMAKE_CXX_FLAGS} -std=c++17 --hash-style=both")

add_library(openxr_loader SHARED IMPORTED)
set_target_properties(openxr_loader PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/StereoKit/StereoKitC/lib/bin/${ANDROID_ABI}/Release/libopenxr_loader.so)
set_target_properties(openxr_loader PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external/StereoKit/StereoKitC/lib/include/openxr)

add_library(reactphysics3d STATIC IMPORTED)
set_target_properties(reactphysics3d PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/StereoKit/StereoKitC/lib/bin/${ANDROID_ABI}/Release/libreactphysics3d.a)
set_target_properties(reactphysics3d PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external/StereoKit/StereoKitC/lib/include/reactphysics3d)

file(GLOB_RECURSE STEREOKIT_SRCS ${CMAKE_SOURCE_DIR}/external/StereoKit/StereoKitC/*.cpp)
add_library(stereokitc STATIC ${STEREOKIT_SRCS})
set_target_properties(reactphysics3d PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external/StereoKit/StereoKitC)
target_include_directories(stereokitc PUBLIC ${CMAKE_SOURCE_DIR}/external/StereoKit/StereoKitC/lib/include ${CMAKE_SOURCE_DIR}/external/StereoKit/StereoKitC/lib/include_no_win)

add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
set_target_properties(native_app_glue PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ANDROID_NDK}/sources/android/native_app_glue)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_library(native-activity SHARED main.cpp app.cpp)

target_precompile_headers(native-activity PUBLIC "pch.h")

target_link_libraries(native-activity
    android
    native_app_glue
    EGL
    GLESv1_CM
    log
    openxr_loader
    reactphysics3d
    stereokitc)
