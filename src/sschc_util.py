#---------------------------------------------------------------------------
# Utility functions for Soichi's SCHC code
# http://github.com/tanupoo/schc-test
#
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

import sys
sys.path.append("../schc-test") # XXX: as a module

import schc_compress_ruledb
import schc_fragment_ruledb
import schc_ruledb
import schc_ruledb_tag as tag

import pprint

#---------------------------------------------------------------------------


def parse_convertion_table(s):
    result = []
    while len(s) > 0:
        pos = s.find("\n")
        line = s[:pos]
        line = line.strip()
        if len(line) == 0:
            s = s[pos+1:]
            continue
        p1 = line.find("|")
        p2 = line.find("|", p1+1)

        if p2 < 0:
            k1 = line[:p1].strip()
            k2 = line[p1+1:].strip()
            #print(k1, k2, p1, p2, line)
            result.append((k1, k2))
            s = s[pos:]
        else:
            p3 = s.find("[", p2+1)
            p4 = s.find("]", p3+1)
            assert p3 > 0 and p4 > 0

            k1 = line[:p1].strip()
            k2 = line[p1+1:p2].strip()
            k3 = line[p2+1:p3].strip()
            vl = s[p3+1:p4].strip()

            #print(k1, k2, k3, vl)
            assert len(k3) == 0

            data_convertion = [v.strip().split(":") for v in  vl.split(",")]
            data_convertion = [ (v1.strip(), v2.strip())
                                for (v1,v2) in data_convertion ]
            #print(data_convertion)
            result.append((k1, k2, data_convertion))

            s = s[p4+1:]
    return result


def reverse_convertion(convertion_table):
    result = []
    for convertion in convertion_table:
        if len(convertion) == 2:
            k1, k2 = convertion
            result.append((k2,k1))
        elif len(convertion) == 3:
            k1, k2, v_list = convertion
            result.append((k2,k1,[(v2,v1) for (v1,v2) in v_list]))
        else: raise ValueError("bad tuple size", convertion)
    return result

def convert_dict(data, convertion_table):
    result = data.copy()
    for convertion in convertion_table:
        if len(convertion) == 2:
            k1, k2, data_convertion = convertion + ([],)
        elif len(convertion) == 3:
            k1, k2, data_convertion = convertion
        else: raise ValueError("bad tuple size", convertion)
        if k1 in result:
            assert k2 not in result
            convert_data = dict(data_convertion)
            if len(convert_data) > 0:
                result[k2] = convert_data.get(result[k1], result[k1])
            else: result[k2] = result[k1]
            del result[k1]
    return result

#---------------------------------------------------------------------------

CONTEXT_CONVERTION_STR = """

context_id      | CID
rule_id_bitsize | RID_SIZE
default_rule_id | DEFAULT_RID
mic_type        | MIC_FUNC    | [MIC_TYPE_CRC32:CRC32]

"""

FRAG_CONVERTION_STR = """

rule_id      | RID
dtag_bitsize | DTAG_SIZE
fcn_bitsize  | FCN_SIZE
default_dtag | DEFAULT_DTAG
ack_mode     | MODE | [
  ACK_NO_ACK:NO-ACK,
  ACK_ALWAYS:ACK-ALWAYS,
  ACK_ON_ERROR:ACK-ON-ERROR ]

"""

COMPRESS_CONVERTION_STR = """

rule_id     | RID
action_list | RULE_SET

"""

COMPRESS_RULE_CONVERTION_STR = """

field_id          | FID | [
  FID_IPv6_version                    : IPV6.VER,
  FID_IPv6_Traffic_class              : IPV6.TC,
  FID_IPv6_Flow_label                 : IPV6.FL,
  FID_IPv6_Payload_Length             : IPV6.LEN,
  FID_IPv6_Next_Header                : IPV6.NXT,
  FID_IPv6_Hop_Limit                  : IPV6.HOP_LMT,
  FID_IPv6_Source_Address_Prefix      : IPV6.DEV_PREFIX,
  FID_IPv6_Source_Address_IID         : IPV6.DEV_IID,
  FID_IPv6_Destination_Address_Prefix : IPV6.APP_PREFIX,
  FID_IPv6_Destination_Address_IID    : IPV6.APP_IID,

  FID_IPv6_UDP_Source_Port            : UDP.DEV_PORT,
  FID_IPv6_UDP_Destination_Port       : UDP.APP_PORT,
  FID_IPv6_UDP_Length                 : UDP.LEN,
  FID_IPv6_UDP_Checksum               : UDP.CKSUM
]

field_bitsize     | FL
field_position    | FP
dir_indicator     | DI | [ DI_UP:UP, DI_DW:DW, DI_BI:BI ]

matching_operator | MO | [
  MO_EQUAL         : EQUAL,
  MO_IGNORE        : IGNORE,
  MO_MATCH_MAPPING : MATCH-MAPPING
]

action            | CDA | [
    CDA_NOT_SENT         : NOT-SENT,
    CDA_VALUE_SENT       : VALUE-SENT,
    CDA_MAPPING_SENT     : MAPPING-SENT,
    CDA_LSB              : LSB,
    CDA_COMPUTE_LENGTH   : COMP-LENGTH,
    CDA_COMPUTE_CHECKSUM : COMP-CHK,
    CDA_DEVIID           : DEVIID,
    CDA_APPIID           : APPIID
]

"""

CONTEXT_CONVERTION = parse_convertion_table(CONTEXT_CONVERTION_STR)
#pprint.pprint(CONTEXT_CONVERTION)

FRAG_CONVERTION = parse_convertion_table(FRAG_CONVERTION_STR)
#pprint.pprint(FRAG_CONVERTION)

COMPRESS_CONVERTION = parse_convertion_table(COMPRESS_CONVERTION_STR)
#pprint.pprint(COMPRESS_CONVERTION)

COMPRESS_RULE_CONVERTION = parse_convertion_table(COMPRESS_RULE_CONVERTION_STR)
#pprint.pprint(COMPRESS_RULE_CONVERTION)

#---------------------------------------------------------------------------

class SSCHCManager:
    def __init__(self):
        pass

    def read_compress_file(self, filename):
        schc_context, schc_info = make_default_schc_context()
        rule_db = schc_compress_ruledb.schc_compress_ruledb()
        rule_data = rule_db.load_json_file(schc_context, filename)
        return rule_db

    def read_compress_file(self, context_filename, compress_filename_list):
        rule_db = schc_compress_ruledb.schc_compress_ruledb()
        context_id = rule_db.load_context_json_file(context_filename)
        context = rule_db.get_context(cid=context_id)
        context = context.copy()
        rule_id_list = rule_db.load_json_file(
            context_id, compress_filename_list)
        compress_rule_list = ([rule_db.get_rule(context_id, rule_id)
                               for rule_id in rule_id_list])
        return rule_db, context, compress_rule_list

    def read_frag_file(self, context_filename, frag_filename_list):
        rule_db = schc_fragment_ruledb.schc_fragment_ruledb()
        context_id = rule_db.load_context_json_file(context_filename)
        context = rule_db.get_context(cid=context_id)
        context = context.copy()
        rule_id_list = rule_db.load_json_file(
            context_id, frag_filename_list)
        frag_rule_list = ([rule_db.get_rule(context_id, rule_id)
                           for rule_id in rule_id_list])
        return rule_db, context, frag_rule_list

    def read_json(self, context_filename,
                  compress_filename_list, frag_filename_list):
        crdb, context, compress_rule_list = self.read_compress_file(
            context_filename, compress_filename_list)
        frdb, context, frag_rule_list = self.read_frag_file(
            context_filename, frag_filename_list)
        return context, compress_rule_list, frag_rule_list

    def convert_context(self, context):
        return convert_dict(context, reverse_convertion(CONTEXT_CONVERTION))

    def convert_frag(self, rule):
        return convert_dict(rule, reverse_convertion(FRAG_CONVERTION))

    def convert_compress(self, rule):
        rule_convertion = reverse_convertion(COMPRESS_RULE_CONVERTION)
        rule["RULE_SET"] = [ convert_dict(rule, rule_convertion)
                             for rule in rule["RULE_SET"] ]
        return convert_dict(rule, reverse_convertion(COMPRESS_CONVERTION))

#---------------------------------------------------------------------------

def make_default_schc_context():
    info = {
        tag.TAG_CID: 0,
        tag.TAG_RID_SIZE: 4,
        tag.TAG_DEFAULT_RID: 0xf,
        tag.TAG_MIC_FUNC: "CRC32"
    }
    context = schc_ruledb.schc_runtime_context(info)
    return context, info

#---------------------------------------------------------------------------
