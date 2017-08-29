DIRECTORY:=common \
           hash

BUILT_DEPS:=$(foreach n,${DIRECTORY},${n}/builtin.o)
MAKE_COMMAND:=$(foreach n,${DIRECTORY},make -C ${n};)
CLEAN_COMMAND:=$(foreach n,${DIRECTORY},make -C ${n} clean;)
TARGET_DIR:=${LIB_DIR}
TARGET_NAME:=libhuix.so
TARGET:=${TARGET_DIR}/${TARGET_NAME}

${TARGET}:${BUILT_DEPS}
	gcc -fPIC -shared $^ -o $@

${BUILT_DEPS}:
	${MAKE_COMMAND}

clean:
	${CLEAN_COMMAND}
