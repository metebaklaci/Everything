import random

while True:
    l=10
    w=10
    minecount = 20

    numberlookup = ["⓿","❶","❷","❸","❹","❺","❻","❼","❽","❾"]
    mines = []

    for i in range(0,l):
        mines.append([])
        for j in range(0,w):
            mines[i].append("O")
            
    for i in range(0,minecount):
        ro=random.randint(0,l-1)
        co=random.randint(0,w-1)
        while mines[ro][co]=="*":
            ro=random.randint(0,l-1)
            co=random.randint(0,w-1)
        mines[ro][co]="*"
        
    field = []

    for i in range(0,l):
        field.append([])
        for j in range(0,w):
            field[i].append("🟩")
            
    def printstate():
        for i in range(0,40):
            print("")
        print("     ", end="")
        for i in range(0,w):
            print((i+1)%10, end=" ")
        print("")
        print("")
        for i in range(0,l):
            print((i+1)%10, end="   ")
            for j in range(0,w):
                try:
                    print(numberlookup[int(field[i][j])], end=" ")
                except:
                    print(field[i][j], end="")
            print("   "+str((i+1)%10), end="   ")
            print("")
        print("")
        print("     ", end="")
        for i in range(0,w):
            print((i+1)%10, end=" ")
        print("")
        print("")
            
    def state(row,col):
        global mines
        try:
            assert (row+1)/abs(row+1) == 1
            assert (col+1)/abs(col+1) == 1
            if mines[row][col]=="*":
                return 1
        except:
            None
        return 0

    def trypop(row,col):
        try:
            assert (row+1)/abs(row+1) == 1
            assert (col+1)/abs(col+1) == 1
            pop(row,col)
        except:
            None

    def pop(row,col):
        global field
        if field[row][col] == "🟩":
            count=state(row-1,col-1)+state(row,col-1)+state(row+1,col-1)
            count+=state(row-1,col)+state(row+1,col)
            count+=state(row-1,col+1)+state(row,col+1)+state(row+1,col+1)
            field[row][col] = str(count)
            if count==0:
                trypop(row-1,col-1)
                trypop(row,col-1)
                trypop(row+1,col-1)
                trypop(row-1,col)
                trypop(row+1,col)
                trypop(row-1,col+1)
                trypop(row,col+1)
                trypop(row+1,col+1)
                
    printstate()

    targetrow = int(input("Row: "))
    targetcol = int(input("Column: "))

    countinit=state(targetrow-1,targetcol-1)+state(targetrow,targetcol-1)+state(targetrow+1,targetcol-1)
    countinit+=state(targetrow-1,targetcol)+state(targetrow,targetcol)+state(targetrow+1,targetcol)
    countinit+=state(targetrow-1,targetcol+1)+state(targetrow,targetcol+1)+state(targetrow+1,targetcol+1)
    while countinit != 0:
        mines = []
        
        for i in range(0,l):
            mines.append([])
            for j in range(0,w):
                mines[i].append("O")
                
        for i in range(0,minecount):
            ro=random.randint(0,l-1)
            co=random.randint(0,w-1)
            while mines[ro][co]=="*":
                ro=random.randint(0,l-1)
                co=random.randint(0,w-1)
            mines[ro][co]="*"
            
        countinit=state(targetrow-1,targetcol-1)+state(targetrow,targetcol-1)+state(targetrow+1,targetcol-1)
        countinit+=state(targetrow-1,targetcol)+state(targetrow,targetcol)+state(targetrow+1,targetcol)
        countinit+=state(targetrow-1,targetcol+1)+state(targetrow,targetcol+1)+state(targetrow+1,targetcol+1)

    pop(targetrow,targetcol)

    while True:
        printstate()
        targetrow = int(input("Row: "))-1
        targetcol = int(input("Column: "))-1
        preicon=field[targetrow][targetcol]
        field[targetrow][targetcol] = "🟧"
        printstate()
        mode = input("Dig = 1 flag = 2 retarget = 3: ")
        field[targetrow][targetcol] = preicon
        while mode == "3":
            printstate()
            targetrow = int(input("Row: "))-1
            targetcol = int(input("Column: "))-1
            preicon=field[targetrow][targetcol]
            field[targetrow][targetcol] = "🟧"
            printstate()
            mode = input("Dig = 1 flag = 2 retarget = 3: ")
            field[targetrow][targetcol] = preicon
            

        if mode == "1":
            if mines[targetrow][targetcol] == "*":
                print("GAME OVER")
                break
            else:
                pop(targetrow,targetcol)

        elif mode == "2":
            if field[targetrow][targetcol] == "🟩":
                field[targetrow][targetcol] = "🚩"
            elif field[targetrow][targetcol] == "🚩":
                field[targetrow][targetcol] = "🟩"
        
        elif mode == "debug":
            print("  ", end="")
            for i in range(0,w):
                print(str(i)+" ", end="")
            print("")
            for i in range(0,l):
                print(str(i)+" ", end="")
                for j in range(0,w):
                    print(mines[i][j]+" ", end="")
                print("")
    input("Enter to restart")
