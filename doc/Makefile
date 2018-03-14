#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

all: rfc8200-ref.txt

#---------------------------------------------------------------------------

rfc8200.txt:
	wget https://tools.ietf.org/rfc/rfc8200.txt

%-lines.txt:
	make $*.txt
	cat -n $*.txt | col -b | cut -b3- > $@

rfc8200-ref.txt:
	make rfc8200-lines.txt
	python ../src/compactdraft.py rfc8200-lines.txt $@ \
                "Deering & Hinden" "IPv6 Specification"

unused:
	python ../src/compactdraft.py whatever.txt $@ \
                 "Minaburo, et al." "LPWAN SCHC"

#---------------------------------------------------------------------------

draft-ietf-lpwan-ipv6-static-context-hc.txt:
	wget https://tools.ietf.org/html/$@

#---------------------------------------------------------------------------

