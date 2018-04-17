#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

all: schc-test lpwan-ipv6-schc # XXX: should be submodules

RIOT:
	git clone https://gitlab.inria.fr/adjih/RIOT -b dev-lethryk
	@#git clone http://github.com/aabadie/RIOT -b tp3-lora-riot

Tutorials:
	git clone https://github.com/RIOT-OS/Tutorials

SCHC:
	git clone https://github.com/adjih/SCHC

schc-test:
	git clone https://github.com/adjih/schc-test -b dev-libschic
	cd schc-test && git submodule update --init --recursive

lpwan-ipv6-schc:
	git clone https://github.com/jia200x/lpwan-ipv6-schc

#---------------------------------------------------------------------------
