//-----------------------------------------------------------------*- c++ -*-
// SWIG wrapper for liblethryk
// March 2018: parts copied from
//   ~/HgRep/NC-iotlab/ns-3.16-hana/nctools/src/libsew.i
//---------------------------------------------------------------------------
// Author: Cedric Adjih
// Copyright 2013-2018 Inria
// All rights reserved. Distributed only with permission.
//---------------------------------------------------------------------------

%module liblethrykmodule
%include "stdint.i"
 //%include "cdata.i"
%include "carrays.i"
 //%include "typemaps.i"
 //%include "cpointer.i"

%array_class(uint8_t, u8array)
 //%array_class(uint16_t, u16array)
 //%pointer_functions(uint8_t,  u8ptr)
 //%pointer_functions(uint16_t, u16ptr)
 //%array_functions(uint8_t, u8block)

//---------------------------------------------------------------------------

%{

#include "buffer.h"
//#include "buffer.c"
#include "lethryk.h"
    
%}

%include "buffer.h"
%include "parser_ipv6.h"
%include "lethryk.h"

//---------------------------------------------------------------------------

%inline %{

%}

//---------------------------------------------------------------------------
