include config.mk

SRC += sbed.c
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

${OBJ}: config.h config.mk

sbed: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

debug: clean
	@make CFLAGS='${DEBUG_CFLAGS}'

distclean: clean
	@echo distcleaning
	@rm -f *~

clean:
	@echo cleaning
	@rm -f sbed ${OBJ} sbed-${VERSION}.tar.gz

dist: distclean
	@echo creating dist tarball
	@mkdir -p sbed-${VERSION}
	@cp -R Makefile README TODO config.h config.mk \
		${SRC} sbed-${VERSION}
	@tar -cf sbed-${VERSION}.tar sbed-${VERSION}
	@gzip sbed-${VERSION}.tar
	@rm -rf sbed-${VERSION}

.PHONY: all options clean dist distclean debug
