#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

all: Tutorials RIOT

RIOT:
	git clone https://gitlab.inria.fr/adjih/RIOT -b dev-lethryk
	@#git clone http://github.com/aabadie/RIOT -b tp3-lora-riot

Tutorials:
	git clone https://github.com/RIOT-OS/Tutorials

#---------------------------------------------------------------------------
