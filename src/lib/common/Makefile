SRC:=list.c \
     huix_str.c \

LINK:=

OBJ:=${SRC:.c=.o}
TARGET_DIR:=.
TARGET_NAME:=builtin.o
TARGET:=${TARGET_DIR}/${TARGET_NAME}
CFLAGS:=-I ${INCLUDE_DIR} -fPIC -shared

ifdef LINK               
  LINK_FLAGS:=-l ${LINK} 
else                     
  LINK_FLAGS:=           
endif                    

all:${TARGET}

${TARGET}:${OBJ}
	${LD} -r $^ -o $@ -L ${LIB_DIR} ${LINK_FLAGS}

.o:.c
	${CC} $^ -o $@

clean:
	rm -rf *.o
