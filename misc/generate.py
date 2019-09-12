#!/usr/bin/env python3

f = open("cells.yml", "w")
count = 0
for ndx in range(175):
	for ndy in range(128):
		f.write("[{}, {}, {}],".format(count, ndx, ndy))
		count = count + 1

print("Done!")