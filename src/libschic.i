//-----------------------------------------------------------------*- c++ -*-
// SWIG wrapper for libschic
// March 2018: parts copied from
//   ~/HgRep/NC-iotlab/ns-3.16-hana/nctools/src/libsew.i
//---------------------------------------------------------------------------
// Author: Cedric Adjih
// Copyright 2013-2018 Inria
// All rights reserved. Distributed only with permission.
//---------------------------------------------------------------------------

%module libschicmodule
%include "stdint.i"
%include "cdata.i"
%include "carrays.i"
 //%include "typemaps.i"
 //%include "cpointer.i"

 //%pointer_functions(uint8_t,  u8ptr)
 //%pointer_functions(uint16_t, u16ptr)
 //%array_functions(uint8_t, u8block)

//---------------------------------------------------------------------------

%{

#include "buffer.h"
#include "schic.h"

#include "buffer.c" // XXX - should be in libxxx.so
#include "parser_ipv6.c" // XXX - should be in libxxx.so
#include "fragment.c" // XXX
#include "rule_engine.c" // XXX
#include "rule_engine_repr.c" // XXX

%}

%include "buffer.h"
%include "parser_ipv6.h"
%include "schic.h"
%include "fragment.h"
%include "rule_engine.h"

//---------------------------------------------------------------------------

%array_class(uint8_t, u8array)
%array_class(rule_token_t, rule_token_array)

%inline %{

    void rule_engine_print(rule_engine_t* engine)
    {
        rule_engine_repr(engine, stdout);
    }

%}

//---------------------------------------------------------------------------
