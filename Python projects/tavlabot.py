#TODO: Test whether moving into collecting works by modifying start position to position appropriate for collecting pieces.
#TODO: Possible move functionality works if above works, get to position evaluation function and then do various depth searches
#Move search seems to be running quite dang fast, could use it more liberally

import random
import copy

board = []
for i in range(24):
    board.append(0)
    
#Board setup
board[0] = 2
board[5] = -5
board[7] = -2
board[11] = 5
board[12] = -5
board[16] = 3
board[18] = 5
board[23] = -2

collectedw = 0
collectedb = 0

brokenw = 0
brokenb = 0

def sign(x):
    if x==0:
        return 0
    return x/abs(x)

def play(pos, dice):
    global board, brokenb, brokenw, collectedb, collectedw
    
    if sign(board[pos]) == 1:
        if pos + dice <= 23:
            if sign(board[pos+dice]) == 1 or board[pos+dice] == 0:
                board[pos] -= 1
                board[pos+dice] += 1
            else:
                if board[pos+dice] == -1:
                    board[pos] -= 1
                    board[pos+dice] = 1
                    brokenb += 1
                else:
                    None
                    print("DEBUG: white piece tried to move to a position occupied by opponent")
        else:
            total = 0
            for i in range(pos):
                total+=max(0,board[i])
            if total == 0 and brokenw == 0:
                board[pos] -= 1
                collectedw += 1
            else:
                None
                print("DEBUG: white piece attempted to move past last position illegally")
                
    elif sign(board[pos]) == -1:
        if pos - dice >= 0:
            if sign(board[pos-dice]) == -1 or board[pos-dice] == 0:
                board[pos] -= -1
                board[pos-dice] += -1
            else:
                if board[pos-dice] == 1:
                    board[pos] -= -1
                    board[pos-dice] = -1
                    brokenw += 1
                else:
                    None
                    print("DEBUG: black piece tried to move to a position occupied by opponent")
        else:
            total = 0
            for i in range(23-pos):
                total+=min(0,board[23-i])
            if total == 0 and brokenb == 0:
                board[pos] -= 1
                collectedb += 1
            else:
                None
                print("DEBUG: black piece attempted to move past last position illegally")
                
def positions(team, givenboard):
    #Team is 1 for white, -1 for black, 0 for empty
    pos = []
    for i in range(24):
        if sign(givenboard[i]) == team:
            pos.append(i)
    return pos
                
def possiblemoves(dice1, dice2, team):
    #Team is 1 for white, -1 for black
    #This piece of code relies on the assumption that trying to play illegal moves using play() will not change board state. If that is discovered to not be the case, it must be resolved.
    global board
    movepairs = []
    
    if dice1 != dice2: #heterogenous dice
        #dice1 first, dice2 second
        sources1 = positions(team, board)
        archived1 = copy.deepcopy(board)
        for i in range(len(sources1)):
            board = copy.deepcopy(archived1)
            play(sources1[i],dice1)
            if board != archived1:
                sources2 = positions(team, board)
                archived2 = copy.deepcopy(board)
                for j in range(len(sources2)):
                    board = copy.deepcopy(archived2)
                    play(sources2[j],dice2)
                    if board != archived2:
                        movepairs.append([sources1[i],dice1,sources2[j],dice2])
        
        board = copy.deepcopy(archived1)
        
        #dice2 first, dice1 second
        sources1 = positions(team, board)
        archived1 = copy.deepcopy(board)
        for i in range(len(sources1)):
            board = copy.deepcopy(archived1)
            play(sources1[i],dice2)
            if board != archived1:
                sources2 = positions(team, board)
                archived2 = copy.deepcopy(board)
                for j in range(len(sources2)):
                    board = copy.deepcopy(archived2)
                    play(sources2[j],dice1)
                    if board != archived2:
                        movepairs.append([sources1[i],dice2,sources2[j],dice1])
        
        board = copy.deepcopy(archived1)
    
    else: #if a double is rolled
        sources1 = positions(team, board)
        archived1 = copy.deepcopy(board)
        for i in range(len(sources1)):
            board = copy.deepcopy(archived1)
            play(sources1[i],dice1)
            if board != archived1:
                sources2 = positions(team, board)
                archived2 = copy.deepcopy(board)
                for j in range(len(sources2)):
                    board = copy.deepcopy(archived2)
                    play(sources2[j],dice1)
                    if board != archived2:
                        sources3 = positions(team, board)
                        archived3 = copy.deepcopy(board)
                        for k in range(len(sources3)):
                            board = copy.deepcopy(archived3)
                            play(sources3[k],dice1)
                            if board != archived3:
                                sources4 = positions(team, board)
                                archived4 = copy.deepcopy(board)
                                for l in range(len(sources4)):
                                    board = copy.deepcopy(archived4)
                                    play(sources4[l],dice1)
                                    if board != archived4:
                                        movepairs.append([sources1[i],dice1,sources2[j],dice1,sources3[k],dice1,sources4[l],dice1])
        
        board = copy.deepcopy(archived1)
    return movepairs

print(board)
print(possiblemoves(6,6,-1))