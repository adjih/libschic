#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

import libschicmodule as schic
from libschicmodule import *

#---------------------------------------------------------------------------

FID_to_name = {
    getattr(schic,k):k for k in dir(schic) if k.startswith("FID_IPv6_")
}

#print(FID_to_name)

#---------------------------------------------------------------------------

def bytes_to_u8array(packet_as_bytes):
    data_size = len(packet_as_bytes)
    data = u8array(data_size)
    for i,x in enumerate(packet_as_bytes):
        data[i] = x
    return data, data_size

def get_data_u8(data_u8, data_size):
    return bytes([data_u8[i] for i in range(data_size)])

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

def data_buffer_get(data_buffer):
    if data_buffer.has_bound_error:
        return None
    position = data_buffer.position
    content = []
    data_buffer.position = 0
    for i in range(position):
        x = buffer_get_u8(data_buffer)
        if data_buffer.has_bound_error:
            break
        content.append(x)
    assert(data_buffer.position == position)
    assert(data_buffer.has_bound_error == False)
    return bytes(content)

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
        self.token_count = schic_parse_ipv6(
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

#XXX: TODO: free memory of buffers

class FragmentEngine:
    def __init__(self):
        self.engine = frag_engine_t()
        self.data_u8 = None
        self.data_size = None

    def _init_engine(self, rule_id_bitsize, dtag_bitsize):
        frag_engine_init(self.engine, self.data_u8.cast(), self.data_size,
                         rule_id_bitsize, dtag_bitsize)

    def init_sender(self, packet_as_bytes, rule_id_info, dtag_info,
                    max_frag_size):
        rule_id, rule_id_bitsize = rule_id_info
        dtag, dtag_bitsize = dtag_info
        self.data_u8, self.data_size = bytes_to_u8array(packet_as_bytes)
        self._init_engine(rule_id_bitsize, dtag_bitsize)
        frag_sender_prepare_noack(self.engine, rule_id, dtag, max_frag_size)

    def generate(self):
        frag_max_size= self.engine.frag_size
        frag_data = u8array(frag_max_size)
        frag_size = frag_engine_generate(
            self.engine, frag_data.cast(), frag_max_size)
        if frag_size > 0:
            result = get_data_u8(frag_data, frag_size)
        elif frag_size == 0:
            result = None
        else:
            raise RuntimeError("generate", frag_size)
        return result

    def generate_all(self):
        result = []
        while True:
            packet = self.generate()
            if packet == None:
                break
            result.append(packet)
        return result

    def init_receiver(self, rule_id_bitsize, dtag_bitsize, max_data_size):
        self.data_u8, self.data_size = bytes_to_u8array(b'\0'*max_data_size)#XXX
        self._init_engine(rule_id_bitsize, dtag_bitsize)
        frag_receiver_prepare_noack(self.engine)

    def process(self, packet_as_bytes):
        packet_u8, packet_size = bytes_to_u8array(packet_as_bytes)
        return frag_engine_process(self.engine, packet_u8.cast(), packet_size)

    def get_recv_data(self):
        return data_buffer_get(self.engine.data)
        
        
#---------------------------------------------------------------------------
