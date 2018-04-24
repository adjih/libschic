#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

import sys
import argparse
import struct
import warnings

from sschc_util import make_default_schc_context, SSCHCManager
import libschic

#---------------------------------------------------------------------------

MAX_BITSIZE_U32 = 32

def ensure_max32(value):
    if value > MAX_BITSIZE_U32:
        raise ValueError("bitsize > 32 bits", value)

def ensure_fit_bitsize(value, value_max_bitsize):
    if value >= (1 << value_max_bitsize):
        raise ValueError("value bitsize > max bitsize",
                         value, value_max_bitsize)

class RuleBytecodeManager:
    def __init__(self):
        pass

    def read_rule_file(self, context, compress_list, frag_list):
        sschc = SSCHCManager()
        rule_info = sschc.read_json(context, compress_list, frag_list)
        context, compress_list, frag_list = rule_info
        self.context = sschc.convert_context(context)
        self.frag_list = [sschc.convert_frag(frag) for frag in frag_list]
        self.compress_list = [sschc.convert_compress(c) for c in compress_list]

    def write_rule_file(self, filename, format="schic"):
        if format != "schic":
            raise ValueError("output format not implemented", format)

        r = b""

        # SCHIC header
        r += struct.pack(b"!I", libschic.SCHIC_BYTECODE_MAGIC)
        r += struct.pack(b"!I", libschic.SCHIC_BYTECODE_VERSION_BASIC)

        # Context
        c = self.context
        r += struct.pack(b"!B", libschic.SCHIC_CONTEXT_ID_BASIC)
        ensure_max32(c["rule_id_bitsize"])
        r += struct.pack(b"!B", c["rule_id_bitsize"])
        ensure_fit_bitsize(c["default_rule_id"], c["rule_id_bitsize"])
        r += struct.pack(b"!I", c["default_rule_id"])
        mic_type = libschic.encode(c["mic_type"])
        r += struct.pack(b"!B", mic_type)

        # Number of rules
        r += struct.pack(b"!I", len(self.frag_list))
        r += struct.pack(b"!I", len(self.compress_list))

        # Fragmentation rule
        for rule in self.frag_list:
            ensure_fit_bitsize(rule["rule_id"], c["rule_id_bitsize"])
            ensure_fit_bitsize(rule["default_dtag"], rule["dtag_bitsize"])
            ensure_max32(rule["dtag_bitsize"])
            ensure_max32(rule["fcn_bitsize"])
            rr  = struct.pack(b"!I", rule["rule_id"])
            rr += struct.pack(b"!B", libschic.encode(rule["ack_mode"]))
            rr += struct.pack(b"!B", rule["dtag_bitsize"])
            rr += struct.pack(b"!B", rule["fcn_bitsize"])
            rr += struct.pack(b"!I", rule["default_dtag"])
            r += rr

        bc = b""
        # Compression rule
        for rule in self.compress_list:
            #print(rule)
            rbc_position = len(bc)

            all_action_bc = b""
            for action in rule["action_list"]:
                abc_position = len(bc)
                warnings.warn("XXX:not including CDA TV data") # XXX:
                bc_data = b""
                bc += bc_data
                abc_size = 0

                abc  = struct.pack(b"!B", libschic.FID_NAMESPACE_IPV6)
                abc += struct.pack(b"!B", libschic.encode(action["field_id"]))
                abc += struct.pack(b"!B", action["field_position"])
                abc += struct.pack(b"!B", libschic.encode(
                    action["dir_indicator"]))
                abc += struct.pack(b"!B", libschic.encode(
                    action["matching_operator"]))
                abc += struct.pack(b"!B", libschic.encode(
                    action["action"]))
                abc += struct.pack(b"!I", abc_position)
                abc += struct.pack(b"!I", abc_size)
                all_action_bc += abc

            rbc_size = len(all_action_bc)
            bc += all_action_bc

            rr  = struct.pack(b"!I", rule["rule_id"])
            rr += struct.pack(b"!B", len(rule["action_list"]))
            rr += struct.pack(b"!I", rbc_position)
            rr += struct.pack(b"!I", rbc_size)

            r += rr

        r += bc

        # Write in binary file
        with open(filename, "wb") as f:
            f.write(r)

    def read_rule_file_pyrule(self):
        #assert format == "python" # XXX: other input formats not implemented
        #with open(filename) as f:
        #    rule_list = eval(f.read())
        pass

#---------------------------------------------------------------------------

def cmd_compile(args):
    print(args)
    manager = RuleBytecodeManager()
    manager.read_rule_file(args.context_file,
                           args.compress_rule, args.frag_rule)
    manager.write_rule_file(args.output)

def run_as_program():
    parser = argparse.ArgumentParser(prog="rule compiler")
    subparser = parser.add_subparsers(dest="command", help="sub-command help")

    parser_compile = subparser.add_parser("compile")
    parser_compile.add_argument("context_file")
    parser_compile.add_argument("--frag-rule", nargs='*', default=[])
    parser_compile.add_argument("--compress-rule", nargs='*', default=[])
    parser_compile.add_argument("--output")

    args = parser.parse_args()
    if args.command == "compile":
        cmd_compile(args)
    else:
        raise RuntimeError("invalid command", command)

#---------------------------------------------------------------------------

if __name__ == "__main__":
    run_as_program()

#---------------------------------------------------------------------------
