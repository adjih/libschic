/*---------------------------------------------------------------------------*/

#ifdef ESP_PLATFORM
/* avoid conflicting definitions of buffer_t */
#define _BUFFER_H_
#endif

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/binary.h"
//#include "portmodules.h"

#include "schic.h"
#include "buffer.h"
#include "fragment.h"

#include "parser_ipv6.h"

/*---------------------------------------------------------------------------*/

/* internals: engine object */
typedef struct _schic_engine_obj_t {
    mp_obj_base_t base;
    uint8_t value;
} schic_engine_obj_t;

const mp_obj_type_t schic_engine_type;

mp_obj_t schic_engine_make_new(const mp_obj_type_t *type,
			       size_t n_args,
			       size_t n_kw,
			       const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    schic_engine_obj_t *self = m_new_obj(schic_engine_obj_t);
    self->base.type = &schic_engine_type;
    self->value = 0;
    return MP_OBJ_FROM_PTR(self);
}

STATIC void schic_engine_print(const mp_print_t *print,
			       mp_obj_t self_in,
			       mp_print_kind_t kind)
{
  schic_engine_obj_t *self = MP_OBJ_TO_PTR(self_in);
  printf ("engine-value=%u\n", self->value);
}

/*---------------------------------------------------------------------------*/

/* table of global members */
STATIC const mp_rom_map_elem_t schic_engine_locals_dict_table[] = {
};

STATIC MP_DEFINE_CONST_DICT(schic_engine_locals_dict,
                            schic_engine_locals_dict_table);

/* class-object itself */
const mp_obj_type_t schic_engine_type = {
    { &mp_type_type },
    .name = MP_QSTR_engine,
    .print = schic_engine_print,
    .make_new = schic_engine_make_new,
    .getiter = NULL,
    .iternext = NULL,
    .protocol = NULL,
    .locals_dict = (mp_obj_dict_t*)&schic_engine_locals_dict,
};

/*---------------------------------------------------------------------------*/

STATIC const mp_rom_map_elem_t schic_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_schic) },
    { MP_ROM_QSTR(MP_QSTR_engine), MP_ROM_PTR(&schic_engine_type) },
};

STATIC MP_DEFINE_CONST_DICT (
    mp_module_schic_globals,
    schic_globals_table
);

const mp_obj_module_t mp_module_schic = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_schic_globals,
};

/*---------------------------------------------------------------------------*/
