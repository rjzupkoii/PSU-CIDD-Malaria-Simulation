import inspect, re

def varname(p):
  for line in inspect.getframeinfo(inspect.currentframe().f_back)[3]:
    m = re.search(r'\bvarname\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)', line)
    if m:
      return m.group(1)

population_size = [500000]
beta = [0.065]
f = [0.5]
strategy = [9, 10]

print("%s\t%s\t%s\t%s" % ("population_size", "beta", "f","strategy"))
for ps in population_size:
    for b in beta:
        for ft in f:
            for s in strategy:
                print("%d\t%f\t%f\t%d" %(ps,b,ft,s))
