#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

all: rfc8200-ref.txt extra

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

draft-schc-16-ref.txt: draft-ietf-lpwan-ipv6-static-context-hc-16-lines.txt
	python ../src/compactdraft.py $< $@ \
                 "Minaburo, et al." "LPWAN SCHC"

#---------------------------------------------------------------------------

draft-ietf-lpwan-ipv6-static-context-hc.txt:
	wget https://tools.ietf.org/html/$@

#---------------------------------------------------------------------------

#https://www.ietf.org/id/draft-barthel-icmpv6-schc-00.txt
#https://tools.ietf.org/id/draft-lagos-lpwan-icmpv6-static-context-hc-01.txt

draft-%.txt:
	wget https://tools.ietf.org/id/$@

extra: draft-barthel-icmpv6-schc-00-lines.txt \
  draft-lagos-lpwan-icmpv6-static-context-hc-01-lines.txt \
  draft-ietf-lpwan-ipv6-static-context-hc-16-lines.txt \
  draft-schc-16-ref.txt

#enscript -o a.ps -r -2 draft-barthel-icmpv6-schc-00-lines.txt 
