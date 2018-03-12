#---------------------------------------------------------------------------
# Setup file for compiling SWIG module for libnc
#---------------------------------------------------------------------------
# Author: Cedric Adjih
# Copyright 2018 Inria
# All rights reserved. Distributed only with permission.
#---------------------------------------------------------------------------

import distutils

from distutils.core import setup, Extension

extension = Extension("liblethrykmodule", ["liblethrykmodule.c"])
#extension.undef_macros.append("NDEBUG")

setup(name="liblethrykmodule", version="0.1",
      ext_modules = [ extension ])

#---------------------------------------------------------------------------
