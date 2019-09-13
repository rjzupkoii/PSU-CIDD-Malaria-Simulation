#!/usr/bin/env python3

PATH = "/mnt/c/GIS/Burkina_Faso/data/Burkina_Faso.asc"

nodata = None

count = 0
ndy = -1

input = open(PATH, "r")
output = open("cells.yml", "w")
for line in input.readlines():
	# If this is the NODATA_value line then we need to grab the flag
	# and indicate we are going to start scanning data
	if (line.startswith("NODATA_value")):
		data = list(filter(None, line.strip().split(" ")))
		nodata = data[1]
		continue
	
	# Still scanning the header, press on
	if (nodata is None): continue

	# Start of data, split it and generate the YAML
	ndx = 0
	ndy = ndy + 1
	data = line.strip().split(" ")
	for item in data:
		ndx = ndx + 1
		if (item == nodata): continue
		count = count + 1
		output.write("[{}, {}, {}],".format(count, ndx, ndy))

input.close()
output.close()