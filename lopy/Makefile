#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

GITURL_PYCOM_MICROPYTHON?=https://gitlab.inria.fr/adjih/pycom-micropython -b module-libschic
GITURL_MICROPYTHON?=https://gitlab.inria.fr/adjih/micropython -b module-libschic
BOARD?=LOPY4

PM=pycom-micropython
M=micropython

${PM}:
	git clone ${GITURL_PYCOM_MICROPYTHON}
	cd ${PM} && git submodule update --init
	cd ${PM}/mpy-cross && make


${M}:
	git clone ${GITURL_MICROPYTHON}
	cd ${M} && git submodule update --init
	cd ${M}/ports/unix && make axtls


# . ~/esp/p   : contains:
#export PATH="$PATH:/home/vmadmin/esp/xtensa-esp32-elf/bin"
#export IDF_PATH=/home/vmadmin/esp/pycom-esp-idf

build:
	test -L ${PM}/esp32/schic-upy || ln -s ../.. ${PM}/esp32/schic-upy
	cd ${PM}/esp32 && . ~/esp/p && make BOARD=${BOARD} TARGET=boot
	cd ${PM}/esp32 && . ~/esp/p && make BOARD=${BOARD} TARGET=app V=1

flash: build
	cd ${PM}/esp32 && . ~/esp/p && killall miniterm ; \
             make BOARD=${BOARD} flash ESPPORT=/dev/ttyACM0

clean:
	cd ${PM}/esp32 && . ~/esp/p && make BOARD=${BOARD} clean


native-build:
	make ${M}
	test -L ${M}/ports/unix/schic-upy \
           || ln -s ../../.. ${M}/ports/unix/schic-upy
	cd ${M}/ports/unix && make axtls
	cd ${M}/ports/unix && make V=1

run:
	make native-build
	cd ${M}/ports/unix && ./micropython ../../../test_schic_upy.py

#---------------------------------------------------------------------------
