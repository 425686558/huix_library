SRC:=insert.c \
     delete.c \
     query

LINK:=huix

TESTCASE:=$(foreach n,${SRC},${n:.c=})

TARGET_DIR:=.
TARGET_NAME:=${TESTCASE}
TARGET:=$(foreach n,${TARGET_NAME},${TARGET_DIR}/${n})


ifdef LINK
  LINK_FLAGS:=-l ${LINK}
else
  LINK_FLAGS:=
endif
CFLAGS:=-I ${INCLUDE_DIR}

all:${TARGET}

${TARGET}:
	${CC} ${CFLAGS} $@.c -o $@ -L ${LIB_DIR} ${LINK_FLAGS}

clean:
	rm -rf *.o ${TARGET}
