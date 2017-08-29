export ROOT_DIR:=$(shell pwd)
export INCLUDE_DIR:=${ROOT_DIR}/include
export SRC_DIR:=${ROOT_DIR}/src
export LIB_DIR:=${ROOT_DIR}/lib
export OUT_DIR:=${ROOT_DIR}/out

export CC:=gcc
export LD:=ld
all:
	make -C ${SRC_DIR}

clean:
	make -C src clean
	make -C out clean
	make -C lib clean
