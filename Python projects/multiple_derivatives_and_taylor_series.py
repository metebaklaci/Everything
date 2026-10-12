#ctrl+t to run code on terminal window
#This code takes input a function and computes multiple derivatives of it.
#It also computes each coefficient for the next Taylor series term as it computes derivatives.
#The code only allows for Taylor series around 0 to be computed (for now!)
#However to compute Taylor series around some k the input function can be modified to shift the function by k units
#The resulting coefficient of degree n can then be applied to (x - k)^n rather than x^n

function = input("Enter function: ")
taylor = input("Taylor series? (disable if working with unknowns) (disable if non differentiable at 0) 1=on 0=off: ")
import math

def differentiate(exp):
    if exp == "x":
        return "1"
    elif exp[0:4] == "add(":
        expression = exp[4:-1]
        i = 1
        openparentheses = 0
        while not(expression[i] == "," and openparentheses == 0):
            if expression[i] == "(":
                openparentheses+=1
            if expression[i] == ")":
                 openparentheses+=-1
            i+=1
        arg1 = differentiate(expression[0:i])
        arg2 = differentiate(expression[i+1:])
        return "add("+arg1+","+arg2+")"
    elif exp[0:5] == "mult(":
        expression = exp[5:-1]
        i = 1
        openparentheses = 0
        while not(expression[i] == "," and openparentheses == 0):
            if expression[i] == "(":
                openparentheses+=1
            if expression[i] == ")":
                openparentheses+=-1
            i+=1
        return "add(mult("+expression[0:i]+","+differentiate(expression[i+1:])+"),mult("+differentiate(expression[0:i])+","+expression[i+1:]+"))"
    elif exp[0:4] == "pow(":
        expression = exp[4:-1]
        i = 1
        openparentheses = 0
        while not(expression[i] == "," and openparentheses == 0):
            if expression[i] == "(":
                openparentheses+=1
            if expression[i] == ")":
                openparentheses+=-1
            i+=1
        arg1 = expression[0:i]
        arg2 = expression[i+1:]
        return "mult(pow("+arg1+",add("+arg2+",-1)),add(mult("+arg2+","+differentiate(arg1)+"),mult(mult("+arg1+",ln("+arg1+")),"+differentiate(arg2)+")))"
    elif exp[0:3] == "ln(":
        expression = exp[3:-1]
        return "mult(rec("+expression+")," + differentiate(expression) + ")"
    elif exp[0:4] == "rec(":
        expression = exp[4:-1]
        return "mult(mult(-1,"+differentiate(expression)+"),rec(pow("+expression+",2)))"
    elif exp[0:4] == "sin(":
        expression = exp[4:-1]
        return "mult(cos("+expression+"),"+differentiate(expression)+")"
    elif exp[0:4] == "cos(":
        expression = exp[4:-1]
        return "mult(mult(sin("+expression+"),"+differentiate(expression)+"),-1)"
    else:
        return "0"

############################################################################


def simplify1step(exp):
    
    
    #If numerically simplifying is possible
    
    if exp[0:4] == "add(":
        try:
            expression = exp[4:-1]
            i = 1
            openparentheses = 0
            while not(expression[i] == "," and openparentheses == 0):
                if expression[i] == "(":
                    openparentheses+=1
                if expression[i] == ")":
                    openparentheses+=-1
                i+=1
            targ1 = expression[0:i]
            targ2 = expression[i+1:]
            return simplify1step(str(float(targ1) + float(targ2)))
        except:
            None

    if exp[0:5] == "mult(":
        try:
            expression = exp[5:-1]
            i = 1
            openparentheses = 0
            while not(expression[i] == "," and openparentheses == 0):
                if expression[i] == "(":
                    openparentheses+=1
                if expression[i] == ")":
                    openparentheses+=-1
                i+=1
            targ1 = expression[0:i]
            targ2 = expression[i+1:]
            return simplify1step(str(float(targ1) * float(targ2)))
        except:
            None
            
    if exp[0:4] == "pow(":
        try:
            expression = exp[4:-1]
            i = 1
            openparentheses = 0
            while not(expression[i] == "," and openparentheses == 0):
                if expression[i] == "(":
                    openparentheses+=1
                if expression[i] == ")":
                    openparentheses+=-1
                i+=1
            targ1 = expression[0:i]
            targ2 = expression[i+1:]
            return simplify1step(str(float(targ1) ** float(targ2)))
        except:
            None
            
    if exp[0:3] == "ln(":
        try:
            return simplify1step(str(math.log(float(exp[3:-1]))))
        except:
            None
            
    if exp[0:4] == "rec(":
        try:
            return simplify1step(str(1 / float(exp[4:-1])))
        except:
            None
            
    if exp[0:4] == "sin(":
        try:
            return simplify1step(str(math.sin(float(exp[4:-1]))))
        except:
            None
    
    if exp[0:4] == "cos(":
        try:
            return simplify1step(str(math.cos(float(exp[4:-1]))))
        except:
            None
            
            
    #If it is an identity
    
    
    if exp[0:4] == "add(" and exp[-3:] == ",0)":
        return simplify1step(exp[4:-3])
    elif exp[0:4] == "add(" and exp[-5:] == ",0.0)":
        return simplify1step(exp[4:-5])
    elif exp[0:4] == "add(" and exp[4:6] =="0,":
        return simplify1step(exp[6:-1])
    elif exp[0:4] == "add(" and exp[4:8] =="0.0,":
        return simplify1step(exp[8:-1])
    
    elif exp[0:5] == "mult(" and exp[-3:] == ",0)":
        return "0"
    elif exp[0:5] == "mult(" and exp[-5:] == ",0.0)":
        return "0"
    elif exp[0:5] == "mult(" and exp[5:7] =="0,":
        return "0"
    elif exp[0:5] == "mult(" and exp[5:9] =="0.0,":
        return "0"
    elif exp[0:5] == "mult(" and exp[-3:] == ",1)":
        return simplify1step(exp[5:-3])
    elif exp[0:5] == "mult(" and exp[-5:] == ",1.0)":
        return simplify1step(exp[5:-5])
    elif exp[0:5] == "mult(" and exp[5:7] =="1,":
        return simplify1step(exp[7:-1])
    elif exp[0:5] == "mult(" and exp[5:9] =="1.0,":
        return simplify1step(exp[9:-1])
    
    elif exp[0:4] == "pow(" and (exp[-3:] == ",0)" or exp[-5:] == ",0.0)"):
        return "1"
    elif exp[0:4] == "pow(" and (exp[4:6] =="0," or exp[4:8] == "0.0,"):
        return "1"
    elif exp[0:4] == "pow(" and exp[-3:] == ",1)":
        return simplify1step(exp[4:-3])
    elif exp[0:4] == "pow(" and exp[-5:] == ",1.0)":
        return simplify1step(exp[4:-5])
    elif exp[0:4] == "pow(" and exp[4:6] =="1,":
        return simplify1step(exp[6:-1])
    elif exp[0:4] == "pow(" and exp[4:8] =="1.0,":
        return simplify1step(exp[8:-1])
    
    
    
    #Recurse to next layer if there is a function and above steps failed
    
    
    if exp[0:4] == "add(":
        expression = exp[4:-1]
        i = 1
        openparentheses = 0
        while not(expression[i] == "," and openparentheses == 0):
            if expression[i] == "(":
                openparentheses+=1
            if expression[i] == ")":
                openparentheses+=-1
            i+=1
        arg1 = expression[0:i]
        arg2 = expression[i+1:]
        return "add(" + simplify1step(arg1) + "," + simplify1step(arg2) + ")"
    
    elif exp[0:5] == "mult(":
        expression = exp[5:-1]
        i = 1
        openparentheses = 0
        while not(expression[i] == "," and openparentheses == 0):
            if expression[i] == "(":
                openparentheses+=1
            if expression[i] == ")":
                openparentheses+=-1
            i+=1
        arg1 = expression[0:i]
        arg2 = expression[i+1:]
        return "mult(" + simplify1step(arg1) + "," + simplify1step(arg2) + ")"
    
    elif exp[0:4] == "pow(":
        expression = exp[4:-1]
        i = 1
        openparentheses = 0
        while not(expression[i] == "," and openparentheses == 0):
            if expression[i] == "(":
                openparentheses+=1
            if expression[i] == ")":
                openparentheses+=-1
            i+=1
        arg1 = expression[0:i]
        arg2 = expression[i+1:]
        return "pow(" + simplify1step(arg1) + "," + simplify1step(arg2) + ")"
    
    if exp[0:3] == "ln(":
        return "ln(" + simplify1step(exp[3:-1]) + ")"
            
    if exp[0:4] == "rec(":
        return "rec(" + simplify1step(exp[4:-1]) + ")"
            
    if exp[0:4] == "sin(":
        return "sin(" + simplify1step(exp[4:-1]) + ")"
    
    if exp[0:4] == "cos(":
        return "cos(" + simplify1step(exp[4:-1]) + ")"
    
    

    
    
    
    #If what remains is not a function
    
    
    
    return exp


def simplify(exp):
    prev = exp
    now = simplify1step(prev)
    while prev != now:
        prev = now
        now = simplify1step(prev)
    return now

#unsimplified = differentiate(function)
#print(unsimplified)
#print(simplify(unsimplified))

interest=function
factorial=1
i = 0
while True:
    substituted=interest
    if taylor == "1":
        for j in range(0,len(substituted)):
            if substituted[j]=="x":
                substituted=substituted[:j]+"0"+substituted[j+1:]
        substituted=simplify(substituted)
        print("Taylor series coefficient of term with degree "+str(i)+": "+str(float(substituted)/factorial))
    print("Derivative number "+str(i)+": "+interest)
    interest=simplify(differentiate(interest))
    factorial=factorial*(i+1)
    input("Enter to proceed")
    i+=1
    