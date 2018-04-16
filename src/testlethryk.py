#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

import liblethryk as lt
import argparse

#---------------------------------------------------------------------------

packet_str_list = [
    "60 00 00 00 00 18 11 40 fe 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00 00 01_c0 00 f0 b1 00 18 28 9d_3c 70 61 63 6b 65 74 20 63 6f 6e 74 65 6e 74 3e",
    "60 00 00 00 00 08 3a ff fe 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00 00 02_85 00 7d 37 00 00 00 00",    
    "60 00 00 00 00 18 11 40 fe 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00 00 01_c0 11 f0 b1 00 18 28 8c_3c 70 61 63 6b 65 74 20 63 6f 6e 74 65 6e 74 3e",
    "60 00 00 00 00 18 11 40 fe 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00 00 01_c0 22 f0 b1 00 18 28 7b_3c 70 61 63 6b 65 74 20 63 6f 6e 74 65 6e 74 3e",
    "60 00 00 00 00 18 11 40 fe 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00 00 01_c0 33 f0 b1 00 18 28 6a_3c 70 61 63 6b 65 74 20 63 6f 6e 74 65 6e 74 3e"
]

packet_list = []
for packet_str in packet_str_list:
    packet_str = packet_str.replace("_", " ")
    packet = bytes([ eval("0x"+x) for x in packet_str.split() ])
    packet_list.append(packet)
    
#---------------------------------------------------------------------------

def test_packet():
    parser = lt.Parser()
    parser.parse(packet_list[0])
    parser.dump()

#---------------------------------------------------------------------------

def test_bit_buffer():
    lt_buffer1 = lt.make_buffer_from_bytes(bytes(range(0x100)))
    bb1 = lt.make_bit_buffer(lt_buffer1)
    lt.data_buffer_dump(lt_buffer1)

    lt_buffer2 = lt.make_buffer_from_bytes(bytes([0]*0x100))
    bb2 = lt.make_bit_buffer(lt_buffer2)
    lt.data_buffer_dump(lt_buffer2)

    total_bit = 0x100 * 8
    bit_size = 4
    while not bb1.buffer.has_bound_error:
        for i in range(8):
            if bit_size == 0:
                bit_block = lt.bit_buffer_get_bit(bb1)
            else:
                bit_block = lt.bit_buffer_get_several(bb1, bit_size)
            if not bb1.buffer.has_bound_error:
                bit_str = bin(bit_block).replace("0b","")
                bit_str = (bit_size - len(bit_str))*"0" + bit_str
                print(bit_str, end=" ")
                lt.bit_buffer_put_several(bb2, bit_block, bit_size)
        print()

    lt_buffer2.position = 0
    lt.data_buffer_dump(lt_buffer2)

    dont_garbage_collect_before = [lt_buffer1, lt_buffer2]

#---------------------------------------------------------------------------

def test_frag():
    max_frag_size = 8
    #packet = packet_list[0]
    packet = bytes(range(10))
    #packet = bytes([0])
    sender = lt.FragmentEngine()

    rule_id_bitsize = 4
    dtag_bitsize = 2
    rule_id_info = (0b1001, rule_id_bitsize)
    dtag_info = (0b11, dtag_bitsize)
    
    sender.init_sender(packet, rule_id_info, dtag_info, max_frag_size)
    print("nb fragments: {}".format(sender.engine.frag_count))
    packet_list = sender.generate_all()
    for i,packet in enumerate(packet_list):
        print("FRAGMENT #{}:".format(i), packet, len(packet))

    receiver = lt.FragmentEngine()
    #XXX: we need a pre-parser that extract rule_id, dtag, and match it
    #to the good FragmentEngine
    receiver.init_receiver(rule_id_bitsize, dtag_bitsize, max_data_size=100)
    recv_packet_list = packet_list[:]
    while True:
        packet = recv_packet_list.pop(0)
        result = receiver.process(packet)
        if result == 0:
            pass # ok, continue
        elif result == 1:
            break # finished    
        elif result == -1:
            raise RuntimeError("process packet")
        else: raise RuntimeError("bad return code", result)
        
    assert len(recv_packet_list) == 0
    print("RECEIVED: ", receiver.get_recv_data())

#---------------------------------------------------------------------------

test_frag()
#test_bit_buffer()

#---------------------------------------------------------------------------
