
What it is: beginnings of an implementation of the SCHC.

Basic tests (parse IPv6, frag no-ack):
  cd src && make test
  
Testing rule bytecode compiler (JSON -> SCHIC bytecode) & loading:
  make schc-test && cd src && make test-bytecode

