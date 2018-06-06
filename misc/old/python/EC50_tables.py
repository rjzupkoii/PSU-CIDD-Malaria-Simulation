number_of_drugs = 5

max_id = 1<<5
string_length = len("{0:#b}".format(max_id-1)) -2

EC50_for_0 = 0.75
EC50_for_1 = 1.3

f = lambda x: EC50_for_0 if x=='0' else EC50_for_1

print("[\n", end="")
for x in range(0,max_id):
    gene_string = ("{0:0"+str(string_length)+"b}").format(x)
    ec50 = list(map(f, list(gene_string)))
    print(ec50, end="")
    if(x != max_id-1) :
        print(",")
    else:
        print("")
print("]")
