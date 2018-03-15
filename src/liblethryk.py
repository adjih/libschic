#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

import liblethrykmodule as lethryk
from liblethrykmodule import *

#---------------------------------------------------------------------------

FID_to_name = {
    getattr(lethryk,k):k for k in dir(lethryk) if k.startswith("FID_IPv6_")
}

print(FID_to_name)

#---------------------------------------------------------------------------

def bytes_to_u8array(packet_as_bytes):
    data_size = len(packet_as_bytes)
    data = u8array(data_size)
    for i,x in enumerate(packet_as_bytes):
        print(x, end=" ")
        data[i] = x
    print()
    return data, data_size

def make_buffer_from_bytes(packet_as_bytes):
    data, data_size = bytes_to_u8array(packet_as_bytes)
    data_buffer = buffer_t()
    buffer_init(data_buffer, data.cast(), data_size)
    data_buffer.data_ref = data # avoid garbage collection
    return data_buffer

def dump_hex(iterable):
    for x in iterable:
        print(" %02x" % x, end="")
    print()

def data_buffer_dump(data_buffer):
    print("data_buffer:", end="")
    content = []
    while True:
        x = buffer_get_u8(data_buffer)
        if data_buffer.has_bound_error:
            break
        content.append(x)
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
