#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

import liblethrykmodule as lethryk
from liblethrykmodule import *

#---------------------------------------------------------------------------

FID_to_name = {
    getattr(lethryk,k):k for k in dir(lethryk) if k.startswith("FID_IPv6_")
}

#print(FID_to_name)

#---------------------------------------------------------------------------

def bytes_to_u8array(packet_as_bytes):
    data_size = len(packet_as_bytes)
    data = u8array(data_size)
    for i,x in enumerate(packet_as_bytes):
        data[i] = x
    return data, data_size

def make_buffer_from_bytes(packet_as_bytes):
    data, data_size = bytes_to_u8array(packet_as_bytes)
    data_buffer = buffer_t()
    buffer_init(data_buffer, data.cast(), data_size)
    data_buffer.data_ref = data # avoid garbage collection
    return data_buffer

def make_bit_buffer(lt_buffer):
    bb = bit_buffer_t()
    bit_buffer_init(bb, lt_buffer)
    return bb

def dump_hex(iterable):
    for x in iterable:
        print(" %02x" % x, end="")
    print()

def data_buffer_dump(data_buffer):
    position = data_buffer.position
    print("data_buffer[{}:]:".format(position), end="")
    content = []
    while True:
        x = buffer_get_u8(data_buffer)
        if data_buffer.has_bound_error:
            break
        content.append(x)
    data_buffer.position = position
    data_buffer.has_bound_error = False
    dump_hex(content)

#---------------------------------------------------------------------------

MAX_TOKEN_SIZE = FID_IPv6_COUNT

class Parser:
    def __init__(self):
        self.packet = None
        self.data_buffer= None
        self.token_array = rule_token_array(MAX_TOKEN_SIZE)
        self.token_count = 0

    def parse(self, packet_as_bytes):
        self.packet = packet_as_bytes
        self.data_buffer = make_buffer_from_bytes(self.packet)
        self.data_size = self.data_buffer.capacity
        self.token_count = lethryk_parse_ipv6(
            self.data_buffer, self.token_array, MAX_TOKEN_SIZE)
        return self.token_count

    def dump(self):
        print("Tokens for the rule engine:")
        for i in range(self.token_count):
            token = self.token_array[i]
            if token.field_id_namespace == FID_NAMESPACE_IPV6:
                fid_name = FID_to_name.get(token.field_id, "<unknown>")
                fid_name = fid_name.replace("FID_IPv6_","")
                print("IPv6."+fid_name, end="")
                if token.field_position != 1:
                    print("pos={}".format(token.field_position), end=" ")
                print("[{}]".format(token.target.length), end=" ")
                if token.target.value_ptr == None:
                    print(token.target.value)
                else:
                    assert token.target.length %8 == 0
                    value_u8 = u8array.frompointer(token.target.value_ptr)
                    value = bytes([value_u8[i]
                                   for i in range(token.target.length//8)])
                    dump_hex(value)

#---------------------------------------------------------------------------

class FragmentEngine:
    def __init__(self):
        self.engine = None

    def init_sender(self, packet_as_bytes, rule_id_info, dtag_info,
                    max_frag_size):
        self.engine = frag_engine_t()
        self.data_u8, self.data_size = bytes_to_u8array(packet_as_bytes)
        self.max_frag_size = max_frag_size
        rule_id, rule_id_bitsize = rule_id_info
        dtag_id, dtag_id_bitsize = dtag_info
        frag_sender_prepare_noack(
            self.engine,
            self.data_u8.cast(), self.data_size,
            rule_id, rule_id_bitsize,
            dtag_id, dtag_id_bitsize,
            max_frag_size)

# def XXX

#---------------------------------------------------------------------------
