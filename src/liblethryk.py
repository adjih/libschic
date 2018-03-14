#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

from liblethrykmodule import *

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
    
#---------------------------------------------------------------------------

MAX_TOKEN_SIZE = 10

class Parser:
    def __init__(self):
        self.packet = None
        self.data_buffer= None
        self.token_array = rule_token_array(MAX_TOKEN_SIZE)
        self.token_count = 0

    def parse(self, packet_as_bytes):
        self.packet = packet_as_bytes
        self.data_buffer = make_buffer_from_bytes(self.packet)

        print("data_buffer:", end="")
        while True:
            x = buffer_get_u8(self.data_buffer)
            if self.data_buffer.has_bound_error:
                break
            print(" %02x" % x, end="")
        print()
        
        self.data_buffer = make_buffer_from_bytes(self.packet)
        self.data_size = self.data_buffer.capacity
        self.token_count = lethryk_parse_ipv6(
            self.data_buffer, self.token_array, MAX_TOKEN_SIZE)
        return self.token_count

    def dump(self):
        for i in range(self.token_count):
            token = self.token_array[i]
            print(token.field_id_namespace, token.field_id,
                  token.field_position,
                  token.target.value, token.target.length)

#---------------------------------------------------------------------------
