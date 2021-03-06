#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

PYTHON = python3
CFLAGS += -Wall -Wextra -g3 -fPIC

#--------------------------------------------------

SRCS = buffer.c parser_ipv6.c schic.c fragment.c rule_engine.c \
 rule_engine_repr.c

HEADERS = $(SRCS:.c=.h)

OBJS = $(SRCS:.c=.o)

#---------------------------------------------------------------------------
# Main targets
#---------------------------------------------------------------------------

all: swig-all

swig-all: libschic.a _libschicmodule.so

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

libschic.a: ${OBJS}
	${AR} cru $@ ${OBJS}

#--------------------------------------------------------------------------
# SWIG wrapper
#---------------------------------------------------------------------------

_libschicmodule.so: libschicmodule.c setup.py ${SRCS} ${OBJS}
	rm -rf build
	${PYTHON} setup.py build #|| python setup.py build
	ln -sf build/lib*/libschicmodule*.so $@ || true

libschicmodule.c: libschic.i Makefile ${SRCS} ${HEADERS}
	swig -python -o $@ libschic.i

#---------------------------------------------------------------------------
# Cleaning
#---------------------------------------------------------------------------

clean:
	rm -f *.a *.so *.o *.d *~ *.pyc 
	rm -f libschicmodule.c libschicmodule.py
	rm -f generic-rule-db.schic frag-rule-db.schic \
              generic-rule.schic compress-rule-db.schic

really-clean: clean
	rm -rf build __pycache__

#---------------------------------------------------------------------------
# "Manual" testing
#---------------------------------------------------------------------------

SSCHC_EX_DIR=../schc-test/example-rule

test: all
	python3 testlibschic.py

test-frag:
	python3 rule_compiler.py compile \
              ${SSCHC_EX_DIR}/context-001.json \
              --frag-rule \
              ${SSCHC_EX_DIR}/fragment-rule-001.json \
              ${SSCHC_EX_DIR}/fragment-rule-002.json \
              --output frag-rule-db.schic
	python3 test_bc_schic.py frag-rule-db.schic

test-compress:
	python3 rule_compiler.py compile \
              ${SSCHC_EX_DIR}/context-001.json \
              --compress-rule \
              ${SSCHC_EX_DIR}/schc-rule-draft-09-r1.json \
              --output compress-rule-db.schic
	python3 test_bc_schic.py compress-rule-db.schic

test-bytecode:
	python3 rule_compiler.py compile \
              ${SSCHC_EX_DIR}/context-001.json \
              --frag-rule \
              ${SSCHC_EX_DIR}/fragment-rule-001.json \
              ${SSCHC_EX_DIR}/fragment-rule-002.json \
              --compress-rule \
              ${SSCHC_EX_DIR}/schc-rule-draft-09-r1.json \
              --output generic-rule-db.schic
	python3 test_bc_schic.py generic-rule-db.schic

#---------------------------------------------------------------------------
