#!/bin/sh
#!/bin/sh

rm -r build
mkdir -p build

SOURCE="src/main.cpp"
NAME="a.out"
VERSION="00"

# includes and libs
INCS="${INCS} -I libs/base -I libs/glad -I libs/imgui -I libs/linmath -I libs/stb"
LIBS="-L ./libs/archives -lm -lGL -lpthread -lglfw -limgui -lstb"

# flags
CPPFLAGS="${CPPFLAGS} -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\""
CFLAGS="${CFLAGS} -fsanitize=address -g -Wall -Wno-deprecated-declarations -Wno-write-strings -Wno-unused-function -O3 ${INCS} ${CPPFLAGS}"

# compiler
CC="g++"

# building
echo "[build.sh]# building $NAME"
echo ${CC} $CFLAGS -o "build/$NAME" $SOURCE $LIBS
${CC} $CFLAGS -o "build/$NAME" $SOURCE $LIBS
echo "[build.sh]# finished building"

