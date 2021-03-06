#---------------------------------------------------------------------------
# Cedric Adjih - Inria - 2018
#---------------------------------------------------------------------------

import re
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("input_file")
parser.add_argument("output_file")
parser.add_argument("author_text")
parser.add_argument("title_text")
args = parser.parse_args()

f = open(args.input_file)
line_list = f.readlines()
f.close()

def detabify(line):
    while True:
        pos = line.find("\t")
        if pos < 0:
            break
        actual_pos = ((pos+8)//8) * 8
        line = line[:pos]+" "*(actual_pos-pos)+line[pos+1:]
    return line
    
#line_list = [ line.replace("\t", 8*" ") for line in line_list ]
line_list = [ detabify(line) for line in line_list ]

#print("".join(line_list))

is_space = {}
for line in line_list:
    for i,x in enumerate(line):
        if x == "\n":
            continue
        is_space[i] = is_space.get(i, True) and (x == " ")

i = min(is_space.keys())
while is_space[i] == False:
    i += 1

i0 = i
while is_space[i] == True:
    i += 1

i1 = i

line_list = [ line[:i0] + line[:-1][i0:i1][0:1]+ line[i1:-1]+"\n"
              for line in line_list ]


i = 0
while i+100 < len(line_list):
    if (True
        and re.match("[0-9]+$", line_list[i].strip())
        and re.match("[0-9]+$", line_list[i+1].strip())
        and re.match("[0-9]+$", line_list[i+2].strip())
        and (line_list[i+3].find(args.author_text) > 0)
        and re.match("[0-9]+$", line_list[i+4].strip())
        and (line_list[i+5].find(args.title_text) > 0)
        and re.match("[0-9]+$", line_list[i+6].strip())
        and re.match("[0-9]+$", line_list[i+7].strip())):
        # Remove headers
        del line_list[i:i+8]
    else:
        i += 1

f = open(args.output_file, "w")
f.write("".join(line_list))
f.close()

#---------------------------------------------------------------------------
