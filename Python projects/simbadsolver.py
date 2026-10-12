import random

gridwidth = 5
gridheight = 5
board = []

for i in range(gridwidth):
    board.append([])
    for j in range(gridheight):
        board[i].append(str(i*5+j+1))

board[gridwidth-1][gridheight-1] = "0"
print(board)

gapx = gridwidth
gapy = gridheight

def shuffleonce():
    global gapx, gapy, gridwidth, gridheight, board
    direction = random.randint(0,3)
    if direction == 0:
        if gapy == 1:
            shuffleonce()
        else:
            temp = board[gapy-2][gapx-1]
            board[gapy-2][gapx-1] = "0"
            board[gapy-1][gapx-1] = temp
            gapy += -1
    
    elif direction == 1:
        if gapx == 1:
            shuffleonce()
        else:
            temp = board[gapy-1][gapx-2]
            board[gapy-1][gapx-2] = "0"
            board[gapy-1][gapx-1] = temp
            gapx += -1
            
    elif direction == 2:
        if gapy == gridheight:
            shuffleonce()
        else:
            temp = board[gapy][gapx-1]
            board[gapy][gapx-1] = "0"
            board[gapy-1][gapx-1] = temp
            gapy += 1
    
    elif direction == 3:
        if gapx == gridwidth:
            shuffleonce()
        else:
            temp = board[gapy-1][gapx]
            board[gapy-1][gapx] = "0"
            board[gapy-1][gapx-1] = temp
            gapx += 1
    
    else:
        print("debug")
        print(direction)
            

def searchfor(n):
    global gapx, gapy, gridwidth, gridheight, board
    for i in range(gridwidth):
        for j in range(gridheight):
            if board[i][j] == n:
                return [j+1,i+1]
            
def shift(x,y):
    global gapx, gapy, gridwidth, gridheight, board
    try:
        if board[y-2][x-1] == "0":
            board[y-2][x-1] = board[y-1][x-1]
            board[y-1][x-1] = "0"
            gapy += -1
            
    except:
        None
    try:
        if board[y-1][x] == "0":
            board[y-1][x] = board[y-1][x-1]
            board[y-1][x-1] = "0"
            gapx += 1
            
    except:
        None
    try:
        if board[y][x-1] == "0":
            board[y][x-1] = board[y-1][x-1]
            board[y-1][x-1] = "0"
            gapy += 1
            
    except:
        None
    try:
        if board[y-1][x-2] == "0":
            board[y-1][x-2] = board[y-1][x-1]
            board[y-1][x-1] = "0"
            gapx += -1
    
    except:
        None

def scootright(x,y):
    global gapx, gapy
    shift(x,y)
    shift(x,y+1)
    shift(x+1,y+1)
    shift(x+2,y+1)
    shift(x+2,y)
    
def scootleft(x,y):
    global gapx, gapy
    shift(x,y)
    shift(x,y+1)
    shift(x-1,y+1)
    shift(x-2,y+1)
    shift(x-2,y)
    
def scootup(x,y):
    global gapx, gapy
    shift(x,y)
    shift(x+1,y)
    shift(x+1,y-1)
    shift(x+1,y-2)
    shift(x,y-2)

def scootto(n,x,y):
    global gapx, gapy, gridwidth, gridheight, board
    coords = searchfor(n)
    xcoord = coords[0]
    ycoord = coords[1]
    print(xcoord)
    print(ycoord)
    
    
    
    if x == xcoord and y == ycoord:
        return
    
    
    
    elif x == xcoord:
        
        if gapx == xcoord:
            if gapx == gridwidth:
                shift(gapx-1, gapy)
            else:
                shift(gapx+1, gapy)
        
        if gapy >= ycoord:
            while gapy >= ycoord:
                shift(gapx,gapy-1)
        elif gapy < ycoord and (ycoord - gapy) > 1:
            while (ycoord - gapy) > 1:
                shift(gapx,gapy+1)
        
        if gapx > xcoord:
            while gapx > xcoord:
                shift(gapx-1,gapy)
        elif gapx < xcoord:
            while gapx < xcoord:
                shift(gapx+1,gapy)
            
        for i in range(ycoord-y-1):
            scootup(gapx,gapy+1)
            
        shift(gapx,gapy+1)
        
   
scootto("6",1,1)
print(board)
            