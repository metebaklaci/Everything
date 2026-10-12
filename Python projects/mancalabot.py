import copy
board = [4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0] #6 is our, 13 is opponent's mancala

#  [4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0]   Starting board
#  [6, 5, 4, 3, 2, 1, 0, 4, 4, 4, 4, 4, 4, 0]   Test board to stress-test possible() function, even under stress-test this seems to run very fast


def reconstruct1ce(pos, move):
    position = copy.deepcopy(pos)
    piececount = position[move] #initial number of pieces in pit
    lastpiece = False
    slot = move
    if move < 6: #If we are playing
        position[move] = 0
        while piececount != 0:
            slot += 1
            slot = slot % 14
            if slot != 13: #Skip opponent's mancala
                piececount -= 1
                position[slot] += 1
        if slot == 6: #If last piece lands in our mancala
            lastpiece = True
        if position[slot] == 1 and slot < 6:
            position[6] += position[12-slot] + 1
            position[12-slot] = 0
            position[slot] = 0

    
    elif move > 6 and move != 13: #If opponent is playing
        position[move] = 0
        while piececount != 0:
            slot += 1
            slot = slot % 14
            if slot != 6: #Skip our mancala
                piececount -= 1
                position[slot] += 1
        if slot == 13: #If last piece lands in opponent's mancala
            lastpiece = True
        if position[slot] == 1 and slot > 6 and slot != 13:
            position[13] += position[12-slot] + 1
            position[12-slot] = 0
            position[slot] = 0
    
    return [position, lastpiece] #Zeroth element is updated position, first element is whether or not last piece landed in mancala
    
def reconstruct(pos, move):
    position = copy.deepcopy(pos)
    for i in range(len(move)):
        position = reconstruct1ce(position, move[i])[0]
    return position

def evalboard(position):
    return position[6] - position[13]

debugcounter = 0

def explore(position, team, path): #team 1 = us, team 2 = opponent
    global debugcounter
    moves = []
    
    if team == 1:
        for i in range(0,6):
            if position[i] != 0:
                report = reconstruct1ce(position, i)
                if report[1]:
                    moves = moves + explore(report[0],team, path + [i])
                else:
                    moves.append(path + [i])
        
        if debugcounter % 100 == 0:
            print("DEBUG: ",debugcounter)
        debugcounter += 1
        return moves
    
    elif team == 2:
        for i in range(7,13):
            if position[i] != 0:
                report = reconstruct1ce(position, i)
                if report[1]:
                    moves = moves + explore(report[0],team, path + [i])
                else:
                    moves.append(path + [i])
        return moves
    
def possible(position, team): #team 1 = us, team 2 = opponent
    return explore(position, team, [])



def minimax(position, currentdepth, maxdepth): #Return type can be a floating point number that returns the node value (if depth between 0 and max); the evaluation from the evaluation function (depth = maxdepth); or a list that has the best move and the evaluation (depth = 0)
    if maxdepth == currentdepth:
        evaluate = evalboard(position)
        return evaluate
    else:
        moves1 = possible(position, 1)
        maxvalue = -99999999999    #This must be smaller than what you'd get for a loss
        maxindex = 0
        for i in range(len(moves1)):
            board1 = reconstruct(position, moves1[i])
            moves2 = possible(board1, 2)
            minvalue = 99999999999    #This must be larger than what you'd get for a win
            minindex = 0
            for j in range(len(moves2)):
                board2 = reconstruct(board1, moves2[j])
                nodevalue = minimax(board2, currentdepth+1, maxdepth) #Recursive call
                if nodevalue < minvalue:
                    minvalue = nodevalue
                    minindex = j
            if minvalue > maxvalue:
                maxvalue = minvalue
                maxindex = i
        if currentdepth != 0:
            return maxvalue
        else:
            return [moves1[maxindex],maxvalue]
            
            

        
def bestmove(position, depth):
    return minimax(position, 0, depth)
    
b3 = bestmove(board, 3)
print(b3, debugcounter)
