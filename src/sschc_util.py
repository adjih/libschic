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

#---------------------------------------------------------------------------

class SSCHCManager:
    def __init__(self):
        pass

    def read_compress_file(self, filename):
        schc_context, schc_info = make_default_schc_context()
        rule_db = schc_compress_ruledb.schc_compress_ruledb()
        rule_data = rule_db.load_json_file(schc_context, filename)
        return rule_db

    def read_frag_file(self, filename):
        schc_context, schc_info = make_default_schc_context()
        rule_db = schc_fragment_ruledb.schc_fragment_ruledb()
        rule_data = rule_db.load_json_file(schc_context, filename)
        return rule_db

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
