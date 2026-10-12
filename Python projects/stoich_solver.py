#igcse blast furnace 2 Fe2O3 + 3 C + 7 CaCO3 + 7 SiO2 --> 7 CaSiO3 + 4 Fe + 10 CO2
reactants = [["fe",2,"o",3],["si",1,"o",2],["c",1],["ca",1,"c",1,"o",3]]
products = [["ca",1,"si",1,"o",3],["fe",1],["c",1,"o",2]]


#neutralization of phosphoric acid and potassium hydroxide 1 H3PO4 + 3 KOH --> 1 K3PO4 + 3 H2O
#reactants = [["h",3,"p",1,"o",4],["k",1,"o",1,"h",1]]
#products = [["k",3,"p",1,"o",4],["h",2,"o",1]]


#combustion of pentane 1 C5H12 + 8 O2 --> 6 H2O + 5 CO2
#reactants = [["c",5,"h",12],["o",2]]
#products = [["h",2,"o",1],["c",1,"o",2]]


#anaerobic ethanol fermentation 1 C6H12O6 --> 2 C2H6O + 2 CO2
#reactants = [["c",6,"h",12,"o",6]]
#products = [["c",2,"h",6,"o",1],["c",1,"o",2]]


#respiration 1 C6H12O6 + 6 O2 --> 6 H2O + 6 CO2
#reactants = [["c",6,"h",12,"o",6],["o",2]]
#products = [["h",2,"o",1],["c",1,"o",2]]


rcoef = []
pcoef = []

for i in range(len(reactants)):
    rcoef.append(10)
for i in range(len(products)):
    pcoef.append(10)
    
#Each sub-array in the eqns arrays corresponds to a single element. The items of the sub arrays correspond to the coefficients (inverse of the number of atoms in the molecule) of each molecule.   
eqnsleft = []
eqnsright = []

elements = []

def approximate(val, tolerance):
    num = 1
    den = 1
    while (abs(num/den - val) > tolerance):
        if num/den > val:
            den+=1
        else:
            num+=1
    return [num,den]

def exists(arr, item):
    for i in range(len(arr)):
        if (item == arr[i]):
            return True
    return False

for i in range(len(reactants)):
    for j in range(0,len(reactants[i]),2):
        if not exists(elements, reactants[i][j]):
            elements.append(reactants[i][j])
            
for i in range(len(elements)):
    eqnsleft.append([])
    for j in range(len(reactants)):
        if exists(reactants[j],elements[i]):
            k = 0
            while (reactants[j][k] != elements[i]):
                k+=1
            k+=1
            eqnsleft[i].append(reactants[j][k])
        else:
            eqnsleft[i].append(0)
            
for i in range(len(elements)):
    eqnsright.append([])
    for j in range(len(products)):
        if exists(products[j],elements[i]):
            k = 0
            while (products[j][k] != elements[i]):
                k+=1
            k+=1
            eqnsright[i].append(products[j][k])
        else:
            eqnsright[i].append(0)
        
print(eqnsleft)
print(eqnsright)

def difference(left,right,leftc,rightc):
    elementdifferences = []
    for i in range(len(left)):
        elementdifferences.append(0)
        for j in range(len(leftc)):
            elementdifferences[i] += left[i][j] * leftc[j]
        for j in range(len(rightc)):
            elementdifferences[i] -= right[i][j] * rightc[j]
    return elementdifferences

def valid(left,right,leftc,rightc):
    dif = difference(left,right,leftc,rightc)
    for i in range(len(dif)):
        if dif[i] != 0:
            return False
    return True


m = 0
flag = True
while(flag and not valid(eqnsleft,eqnsright,rcoef,pcoef)):
    dif = difference(eqnsleft,eqnsright,rcoef,pcoef)
    for i in range(len(elements)):
        element = elements[i]
        if (dif[i] < 0):
            for j in range(len(rcoef)):
                if (exists(reactants[j],element)):
                    rcoef[j] += 2
            for j in range(len(pcoef)):
                if (exists(products[j],element)):
                    pcoef[j] -= 1
        elif (dif[i] > 0):
            for j in range(len(pcoef)):
                if (exists(products[j],element)):
                    pcoef[j] += 2
            for j in range(len(rcoef)):
                if (exists(reactants[j],element)):
                    rcoef[j] -= 1
                    
    if m==2000:

        flag = False
    
    
    
    print(m)
    m+=1
    
factor = approximate(rcoef[0]/pcoef[0], 0.2)[0] / rcoef[0]
for k in range(len(rcoef)):
    rcoef[k] = approximate(rcoef[k]*factor,0.2)
    rcoef[k] = rcoef[k][0] / rcoef[k][1]
for k in range(len(pcoef)):
    pcoef[k] = approximate(pcoef[k]*factor,0.2)
    pcoef[k] = pcoef[k][0] / pcoef[k][1]

print(rcoef)
print(pcoef)
