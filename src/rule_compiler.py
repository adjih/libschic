#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

import sys
import argparse
import struct

from sschc_util import make_default_schc_context, SSCHCManager
import libschic

#---------------------------------------------------------------------------

class RuleBytecodeManager:
    def __init__(self):
        pass

    def read_rule_file(self, filename, format="json"):
        #assert format == "python" # XXX: other input formats not implemented
        #with open(filename) as f:
        #    rule_list = eval(f.read())
        assert format == "json" # XXX: other input formats not implemented
        sschc = SSCHCManager()
        sschc.read_frag_file(filename)
        print(rule_data)
        print(rule_db.ruledb)
        
    def write_rule_file(self, filename, format="schic"):
        assert format == "schic" # XXX: other output formats not implemented

#---------------------------------------------------------------------------

def cmd_compile(args):
    manager = RuleBytecodeManager()
    manager.read_rule_file(args.input_rule_file, "json")


def run_as_program():        
    parser = argparse.ArgumentParser(prog="rule compiler")
    subparser = parser.add_subparsers(dest="command", help="sub-command help")
    parser_compile = subparser.add_parser("compile")

    parser.add_argument("input_rule_file")
    parser.add_argument("output_rule_file")

    args = parser.parse_args()
    if args.command == "compile":
        cmd_compile(args)
    else:
        raise RuntimeError("invalid command", command)

#---------------------------------------------------------------------------

if __name__ == "__main__":
    run_as_program()

#---------------------------------------------------------------------------    
