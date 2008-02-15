include config.mk

SRC += sbed.c madtty.c
OBJ = ${SRC:.c=.o}

all: clean options sbed

options:
	@echo sbed build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

sbed: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

debug: clean
	@make CFLAGS='${DEBUG_CFLAGS}'

clean:
	@echo cleaning
	@rm -f sbed ${OBJ} sbed-${VERSION}.tar.gz *~

dist: clean
	@echo creating dist tarball
	@mkdir -p sbed-${VERSION}
	@cp -R Makefile README TODO config.mk \
		${SRC} madtty.h sbed-${VERSION}
	@tar -cf sbed-${VERSION}.tar sbed-${VERSION}
	@gzip sbed-${VERSION}.tar
	@rm -rf sbed-${VERSION}

.PHONY: all options clean dist debug
