#!/bin/sh
#!/bin/sh

rm -r build
mkdir -p build

SOURCE="src/main.cpp"
NAME="a.out"
VERSION="00"

# includes and libs
INCS="${INCS} -I libs/base -I libs/glad -I libs/imgui"
LIBS="-lm -lGL -lpthread -lglfw -L ./libs/imgui -limgui -ldl"

# flags
CPPFLAGS="${CPPFLAGS} -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\""
CFLAGS="${CFLAGS} -g -Wall -Wno-deprecated-declarations -Wno-write-strings -Wno-unused-function -O3 ${INCS} ${CPPFLAGS}"

# compiler
CC="g++"

# building
echo "[build.sh]# building $NAME"
echo ${CC} $CFLAGS -o "build/$NAME" $SOURCE $LIBS
${CC} $CFLAGS -o "build/$NAME" $SOURCE $LIBS
echo "[build.sh]# finished building"
