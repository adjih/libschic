
#---------------------------------------------------------------------------

packet_list = [
    "60 00 00 00 00 18 11 40 fe 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00 00 01_c0 00 f0 b1 00 18 28 9d_3c 70 61 63 6b 65 74 20 63 6f 6e 74 65 6e 74 3e",
    "60 00 00 00 00 08 3a ff fe 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00 00 02_85 00 7d 37 00 00 00 00",    
    "60 00 00 00 00 18 11 40 fe 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00 00 01_c0 11 f0 b1 00 18 28 8c_3c 70 61 63 6b 65 74 20 63 6f 6e 74 65 6e 74 3e",
    "60 00 00 00 00 18 11 40 fe 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00 00 01_c0 22 f0 b1 00 18 28 7b_3c 70 61 63 6b 65 74 20 63 6f 6e 74 65 6e 74 3e",
    "60 00 00 00 00 18 11 40 fe 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00 00 01_c0 33 f0 b1 00 18 28 6a_3c 70 61 63 6b 65 74 20 63 6f 6e 74 65 6e 74 3e"
]

#---------------------------------------------------------------------------

if False:
    for packet in packet_list:
        packet_str = packet.replace("_", " ")
        packet = "".join([ chr(eval("0x"+x)) for x in packet_str.split() ])
        print(repr(packet))
        break

#---------------------------------------------------------------------------

import liblethrykmodule as lt

data = lt.u8array(100)
data[0] = 111
data[1] = 1
data[2] = 2
data_buffer = lt.buffer_t()
lt.buffer_init(data_buffer, data.cast(), 100)
print(lt.buffer_get_u8(data_buffer))
print(lt.buffer_get_u16(data_buffer))

#---------------------------------------------------------------------------
