#!/bin/sh

#==============================================================================
# Build script for ImGui

cd imgui/

CXX=g++
CXXFLAGS="-Wall -I. -Ibackends"

SOURCES=("imgui.cpp" "imgui_draw.cpp" "imgui_widgets.cpp" "imgui_tables.cpp" "backends/imgui_impl_glfw.cpp" "backends/imgui_impl_opengl3.cpp")
OBJECTS=()
LIBRARY="libimgui.a"

for src in "${SOURCES[@]}"; do
    OBJECTS+=("${src%.cpp}.o")
done

build_all() {
    build_library
}

build_library() {
    for src in "${SOURCES[@]}"; do
        $CXX $CXXFLAGS -c $src -o "${src%.cpp}.o"
    done
    ar rcs $LIBRARY "${OBJECTS[@]}"
}

clean() {
    rm -f "${OBJECTS[@]}" $LIBRARY
}

case "$1" in
    all)
        build_all
        ;;
    clean)
        clean
        ;;
    *)
        echo "Usage: $0 {all|clean}"
        exit 1
        ;;
esac

cd ..

#==============================================================================

