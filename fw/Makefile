


APPLICATION = lethryk

RIOTBASE ?= ${CURDIR}/../RIOT
LETHRYKBASE ?= ${CURDIR}/../src
BOARD ?= b-l072z-lrwan1

USEMODULE += xtimer
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

#USEMODULE += posix_sockets
USEMODULE += gnrc_sock_udp
#USEMODULE += gnrc_icmpv6_echo
#USEMODULE += gnrc_ipv6_default
#USEMODULE += auto_init_gnrc_netif
#USEMODULE += gnrc_netdev_default

#USEMODULE += gnrc_sixlowpan
#USEMODULE += socket_zep
#USEMODULE += gnrc_nomac gnrc_zep
#USEMODULE += gnrc_raw

#USEMODULE += gnrc_udp
#USEMODULE += sock_udp

USEMODULE += gnrc_ipv6

CFLAGS += -DDEVELHELP -DMYDEBUG=1
QUIET ?= 1

#USEMODULE += sx1276
#USEPKG += semtech-loramac
CFLAGS += -DREGION_EU868 -I${LETHRYKBASE}

include ${RIOTBASE}/Makefile.include


#tap:
#	make all-debug term BOARD=native PORT=tap0

#board:
#	make flash term BOARD=b-l072z-lrwan1

# cd Tutorials/task-06 && make PORT=tap1 all term
# sudo tcpdump -i tap0 -nl -x
