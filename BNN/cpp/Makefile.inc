CXX=g++
CFLAGS=-O3 -std=c++0x -g
MINIZIP_LDFLAGS=-lminizip -laes -lz
LDFLAGS=$(MINIZIP_LDFLAGS)

AR=ar rcs
