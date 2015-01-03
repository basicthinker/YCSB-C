#!/usr/bin/python

import sys
import os
import numpy

# Main

if len(sys.argv) != 2:
    print "Usage: %s FileName" % sys.argv[0]
    sys.exit(-1)

path = sys.argv[1]

lines = [line.strip().split('\t') for line in open(path)]

results = { }

db_index = set() # db name
tn_index = set() # thread number

for line in lines:
    if line[0][0] == '#':
        continue
    db_name = line[0]
    db_index.add(db_name)

    dir_name, workload = os.path.split(line[1])
    workload = os.path.splitext(workload)[0]

    num_threads = int(line[2])
    tn_index.add(num_threads)

    throughput = float(line[3])
    if not results.has_key(workload):
        results[workload] = { }
    if not results[workload].has_key(db_name):
        results[workload][db_name] = { }
    if not results[workload][db_name].has_key(num_threads):
        results[workload][db_name][num_threads] = [ ]
    results[workload][db_name][num_threads].append(throughput)

db_index = sorted(db_index)
tn_index = sorted(tn_index)

for wl in sorted(results.keys()):
    out_file = open(os.path.join(dir_name, wl + ".result"), 'w+')
    # Prints header
    line = "#"
    for db in db_index:
        line += '\t' + db
    out_file.write(line + '\n')
    # Prints results
    for tn in tn_index:
        line = str(tn)
        for db in db_index:
            data = results[wl][db][tn]
            line += '\t' + str(numpy.median(numpy.array(data)))
        out_file.write(line + '\n')

