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

draft-schc-17-ref.txt: draft-ietf-lpwan-ipv6-static-context-hc-27-lines.txt
	python ../src/compactdraft.py $< $@ \
                 "Minaburo, et al." "LPWAN SCHC"

draft-ietf-lpwan-ipv6-static-context-hc-17-ref.txt: \
            draft-ietf-lpwan-ipv6-static-context-hc-27-lines.txt
	python ../src/compactdraft.py $< $@ \
                 "Minaburo, et al." "LPWAN SCHC"

draft-schc-lorawan-02-ref.txt: draft-petrov-lpwan-ipv6-schc-over-lorawan-02-lines.txt
	python ../src/compactdraft.py $< $@ \
	    	"Sornin, et al." "SCHC-over-LoRaWAN"	

#---------------------------------------------------------------------------

draft-ietf-lpwan-ipv6-static-context-hc.txt:
	wget https://tools.ietf.org/html/$@

#---------------------------------------------------------------------------

#https://www.ietf.org/id/draft-barthel-icmpv6-schc-00.txt
#https://tools.ietf.org/id/draft-lagos-lpwan-icmpv6-static-context-hc-01.txt

draft-%.txt:
	wget https://tools.ietf.org/id/$@

extra: draft-ietf-lpwan-ipv6-static-context-hc-17-ref.txt \
  draft-barthel-icmpv6-schc-00-lines.txt \
  draft-lagos-lpwan-icmpv6-static-context-hc-01-lines.txt \
  draft-ietf-lpwan-ipv6-static-context-hc-16-lines.txt \
  draft-ietf-lpwan-ipv6-static-context-hc-17-lines.txt \
  draft-schc-17-ref.txt \
  draft-petrov-lpwan-ipv6-schc-over-lorawan-02-lines.txt \
  draft-schc-lorawan-02-ref.txt


a: draft-schc-17-ref.txt
	enscript -o a.ps -r -2 draft-schc-17-ref.txt
	ps2pdf14 $< 
