import copy
board = [[],[],[],[],[],[],[],[],[]]

# sample sudoku
board[0] = [0,8,0,0,0,0,3,7,0]
board[1] = [0,2,6,4,0,0,0,0,1]
board[2] = [0,0,0,0,1,9,0,6,0]
board[3] = [0,0,2,0,0,0,0,0,0]
board[4] = [0,4,0,0,0,0,9,0,7]
board[5] = [0,0,0,6,0,0,0,0,0]
board[6] = [0,0,0,0,3,5,1,4,8]
board[7] = [0,0,3,0,9,4,5,0,0]
board[8] = [0,0,0,1,0,0,0,3,0]

# board[0] = [0,0,0,0,0,0,0,0,0]
# board[1] = [0,0,0,0,0,0,0,0,0]
# board[2] = [0,0,0,0,0,0,0,0,0]
# board[3] = [0,0,0,0,0,0,0,0,0]
# board[4] = [0,0,0,0,0,0,0,0,0]
# board[5] = [0,0,0,0,0,0,0,0,0]
# board[6] = [0,0,0,0,0,0,0,0,0]
# board[7] = [0,0,0,0,0,0,0,0,0]
# board[8] = [0,0,0,0,0,0,0,0,0]

possible = []

bruteattempts = 0
totalbruteattempts = 1

for i in range(9):
    possible.append([])
    for j in range(9):
        possible[i].append([])
        for k in range(9):
            possible[i][j].append(True)

for i in range(9):
    for j in range(9):
        if board[i][j] != 0:
            for k in range(9):
                possible[i][j][k] = False
            possible[i][j][board[i][j]-1] = True
            
def simplecancel():
    global board, possible
    futureboard = copy.deepcopy(board)
    futurepossible = copy.deepcopy(possible)
    
    for i in range(9):
        for j in range(9):
            if board[i][j] != 0:
                
#                 Cancel out row
                for a in range(9):
                    if a != j:
                        futurepossible[i][a][board[i][j]-1] = False
#                 Cancel out column
                for a in range(9):
                    if a != i:
                        futurepossible[a][j][board[i][j]-1] = False
#                 Cancel out 3x3 square
                sqy = i//3
                sqx = j//3
                for a in range(3):
                    for b in range(3):
                        if 3*sqy+a != i or 3*sqx+b != j:
                            futurepossible[3*sqy+a][3*sqx+b][board[i][j]-1] = False
                            
    for i in range(9):
        for j in range(9):
            count = 0
            value = 0
            for k in range(9):
                if futurepossible[i][j][k] == True:
                    count += 1
                    value = k+1
            if count == 1:
                futureboard[i][j] = value
            
    board = copy.deepcopy(futureboard)
    possible = copy.deepcopy(futurepossible)
    
def solvesimplecancel():
    prev_possible = None
    while prev_possible != possible:
        prev_possible = copy.deepcopy(possible)
        simplecancel()
    
def hiddensingle():
    global board, possible
    futureboard = copy.deepcopy(board)
    futurepossible = copy.deepcopy(possible)

    # Examine rows
    for i in range(9):
        for a in range(9):
            location = 0
            count = 0
            for j in range(9):
                if possible[i][j][a] and board[i][j] == 0:
                    count += 1
                    location = j
            if count == 1:
                futureboard[i][location] = a + 1
                futurepossible[i][location] = [False] * 9
                futurepossible[i][location][a] = True
                # Update global board and possible to run simplecancel()
                board = copy.deepcopy(futureboard)
                possible = copy.deepcopy(futurepossible)
                solvesimplecancel()
                # Update futureboard and futurepossible after cancellation for next iterations
                futureboard = copy.deepcopy(board)
                futurepossible = copy.deepcopy(possible)

    # Examine columns
    for j in range(9):
        for a in range(9):
            location = 0
            count = 0
            for i in range(9):
                if possible[i][j][a] and board[i][j] == 0:
                    count += 1
                    location = i
            if count == 1:
                futureboard[location][j] = a + 1
                futurepossible[location][j] = [False] * 9
                futurepossible[location][j][a] = True
                # Update global board and possible to run simplecancel()
                board = copy.deepcopy(futureboard)
                possible = copy.deepcopy(futurepossible)
                solvesimplecancel()
                # Update futureboard and futurepossible after cancellation for next iterations
                futureboard = copy.deepcopy(board)
                futurepossible = copy.deepcopy(possible)

    # Box hidden singles
    for box_y in range(3):
        for box_x in range(3):
            for a in range(9):
                count = 0
                location = (-1, -1)
                for dy in range(3):
                    for dx in range(3):
                        y = 3 * box_y + dy
                        x = 3 * box_x + dx
                        if possible[y][x][a] and board[y][x] == 0:
                            count += 1
                            location = (y, x)
                if count == 1:
                    y, x = location
                    futureboard[y][x] = a + 1
                    futurepossible[y][x] = [False] * 9
                    futurepossible[y][x][a] = True
                    # Update global board and possible to run simplecancel()
                    board = copy.deepcopy(futureboard)
                    possible = copy.deepcopy(futurepossible)
                    solvesimplecancel()
                    # Update futureboard and futurepossible after cancellation for next iterations
                    futureboard = copy.deepcopy(board)
                    futurepossible = copy.deepcopy(possible)

    board = copy.deepcopy(futureboard)
    possible = copy.deepcopy(futurepossible)

def solvehiddensingle():
    prev_possible = None
    while prev_possible != possible:
        prev_possible = copy.deepcopy(possible)
        hiddensingle()


def r7_8845hs(x,y,final,t,passboard):
    global bruteattempts, totalbruteattempts
    cpuboard = copy.deepcopy(passboard)
    print(x,y)
    if not final:
        for i in range(9):
            if possible[x][y][i]:
                cpuboard[x][y] = i+1
                if x == 8 and y == 8:
                    if validate(passboard):
                        r7_8845hs(0,0,True,t,cpuboard)
                else:
                    if y == 8:
                        if validate(passboard):
                            r7_8845hs(x+1,0,False,t,cpuboard)
                    else:
                        if validate(passboard):
                            r7_8845hs(x,y+1,False,t,cpuboard)
    else:
        bruteattempts += 1
        print(cpuboard)
        print()
        if validate(cpuboard):
            print("Valid solution:")
            print()
            for i in range(9):
                print(cpuboard[i])
            print()
        else:
            print(cpuboard)
            
            
        if bruteattempts % 1000000 == 0:
            print(str(bruteattempts*100/t)+"%")
                    
                


def prettyprintboard():
    global board
    print()
    for i in range(9):
        print(board[i])
    print()
    
def solve():
    global possible
    prev_board = None
    while prev_board != board:
        prev_board = copy.deepcopy(board)
        solvesimplecancel()  # updates board & possible
        solvehiddensingle()  # updates board & possible
        
    prettyprintboard()
    totalbruteattempts = 1
    for i in range(9):
        for j in range(9):
            current = 0
            for k in range(9):
                if possible[i][j][k]:
                    current += 1
            totalbruteattempts *= current
    print(totalbruteattempts)
    
    r7_8845hs(0,0,False,totalbruteattempts,board)
        
def validate(board):
    for i in range(9):
        row_nums = set()
        col_nums = set()
        box_nums = set()
        for j in range(9):
            # Check row
            if board[i][j] != 0:
                if board[i][j] in row_nums:
                    return False
                row_nums.add(board[i][j])
            # Check column
            if board[j][i] != 0:
                if board[j][i] in col_nums:
                    return False
                col_nums.add(board[j][i])
            # Check 3x3 box
            box_row = 3 * (i // 3) + j // 3
            box_col = 3 * (i % 3) + j % 3
            if board[box_row][box_col] != 0:
                if board[box_row][box_col] in box_nums:
                    return False
                box_nums.add(board[box_row][box_col])
    return True

        
prettyprintboard()
        
solve()

prettyprintboard()

print(validate(board))