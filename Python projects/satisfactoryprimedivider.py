factors = []

def test(n):
    
    if n == 1:
        return "Valid"
    
    if n % 2 == 0:
        factors.append(2)
        return test(n/2)
    
    if n % 3 == 0:
        factors.append(3)
        return test(n/3)
    
    return "Invalid"
    
def leastfeas(n):
    
    i = n
    global factors
    while test(i) == "Invalid":
        factors = []
        i+=1
    return i

def behelpful():
    target = int(input("Output fraction desired: "))
    feasible = leastfeas(target)
    print('Instructions: ')
    print('Make a load balancer that divides into '+str(feasible))
    print('For that you will need to: ')
    i = 0
    if factors[-1] == 2:
        i = len(factors)
    else:
        while factors[i] == 2:
            i+=1
    print('Divide by 2 for a total of '+str(i)+' times')
    print('Divide by 3 for a total of '+str(len(factors) - i)+' times')
    print('')
    print('To establish the desired fraction, feed '+str(feasible - target)+' of the resulting branches back to main branch.')
    
def shame():
    print("bros funny")
    
    
    
    
    
    
    
try:
    behelpful()
except:
    shame()
    

    
    
