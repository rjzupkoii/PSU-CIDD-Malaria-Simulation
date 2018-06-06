import inspect, re

def varname(p):
  for line in inspect.getframeinfo(inspect.currentframe().f_back)[3]:
    m = re.search(r'\bvarname\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)', line)
    if m:
      return m.group(1)

population_size = [500000]
beta = [0.065]
f = [0.5, 0.6, 0.7, 0.8, 0.9]
daily_cost_of_resistance = [0.0001, 0.0005, 0.001]
mutation_propbability = [0.0005, 0.001, 0.002]
fraction_non_art_replacement = [0.333333, 0.66666666, 1.0]

print("%s\t%s\t%s\t%s\t%s\t%s" % ("population_size", "beta", "f","daily_cost_of_resistance", "mutation_propbability", "fraction_non_art_replacement"))
for ps in population_size:
    for b in beta:
        for ft in f:
            for dcor in daily_cost_of_resistance:
                for mp in mutation_propbability:
                    for fnar in fraction_non_art_replacement:
                        print("%d\t%f\t%f\t%f\t%f\t%f" %(ps,b,ft,dcor,mp, fnar))
