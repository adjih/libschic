#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

PYTHON = python3
CFLAGS += -Wall -Wextra -g3 -fPIC

#--------------------------------------------------

SRCS = buffer.c parser_ipv6.c lethryk.c

HEADERS = $(SRCS:.c=.h)

OBJS = $(SRCS:.c=.o)

#---------------------------------------------------------------------------
# Main targets
#---------------------------------------------------------------------------

all: swig-all

swig-all: liblethryk.a _liblethrykmodule.so

#---------------------------------------------------------------------------
# Dependencies
# http://stackoverflow.com/questions/97338/gcc-dependency-generation-for-a-different-output-directory
#---------------------------------------------------------------------------

DEPS = $(SRCS:.c=.d)

include $(DEPS)

%.d : %.c
	$(CC) $(CCFLAGS) -MF"$@" -MG -MM -MP -MT"$@" -MT"$(<:.c=.o)" "$<"

#---------------------------------------------------------------------------
# Library
#---------------------------------------------------------------------------

liblethryk.a: ${OBJS}
	${AR} cru $@ ${OBJS}

#--------------------------------------------------------------------------
# SWIG wrapper
#---------------------------------------------------------------------------

_liblethrykmodule.so: liblethrykmodule.c setup.py ${SRCS} ${OBJS}
	rm -rf build
	${PYTHON} setup.py build #|| python setup.py build
	ln -sf build/lib*/liblethrykmodule*.so $@ || true

liblethrykmodule.c: liblethryk.i Makefile ${SRCS} ${HEADERS}
	swig -python -o $@ liblethryk.i

#---------------------------------------------------------------------------
# Cleaning
#---------------------------------------------------------------------------

clean:
	rm -f *.a *.so *.o *.d *~ *.pyc 
	rm -f liblethrykmodule.c liblethrykmodule.py

really-clean: clean
	rm -rf build __pycache__

#---------------------------------------------------------------------------
