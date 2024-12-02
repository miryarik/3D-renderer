. ~/opt/raylib.env
CFLAGS="-Wall -Wextra `pkg-config --cflags raylib`"
LIBS=" `pkg-config --libs raylib` -lm -ldl"
clang $CFLAGS -o main main.c $LIBS