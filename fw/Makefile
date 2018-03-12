
# git clone http://github.com/aabadie/RIOT -b tp3-lora-riot

APPLICATION = lethryk

RIOTBASE ?= ${CURDIR}/../RIOT
LETHRYKBASE ?= ${CURDIR}/../src
BOARD ?= b-l072z-lrwan1

USEMODULE += xtimer
USEMODULE += sx1276

USEPKG += semtech-loramac

CFLAGS += -DREGION_EU868 -I${LETHRYKBASE}

include ${RIOTBASE}/Makefile.include
