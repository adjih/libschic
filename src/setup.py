#---------------------------------------------------------------------------
# Setup file for compiling SWIG module for libnc
#---------------------------------------------------------------------------
# Author: Cedric Adjih
# Copyright 2018 Inria
# All rights reserved. Distributed only with permission.
#---------------------------------------------------------------------------

import distutils

from distutils.core import setup, Extension

extension = Extension("libschicmodule",
                      ["libschicmodule.c"],
                      extra_compile_args = ["-O0", "-g3"])
extension.undef_macros.append("NDEBUG")

setup(name="libschicmodule", version="0.1",
      ext_modules = [ extension ])

#---------------------------------------------------------------------------
