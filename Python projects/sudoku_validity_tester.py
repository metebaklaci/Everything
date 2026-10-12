import copy
board = [[],[],[],[],[],[],[],[],[]]

# sample sudoku
board[0] = [0,4,0,0,6,0,0,0,2]
board[1] = [2,0,0,0,0,0,7,0,5]
board[2] = [0,0,9,0,0,0,0,0,6]
board[3] = [0,0,3,0,8,0,4,0,0]
board[4] = [0,0,0,1,0,0,0,0,0]
board[5] = [0,0,0,0,0,0,8,0,1]
board[6] = [3,0,0,2,1,0,0,0,0]
board[7] = [0,0,8,0,0,0,0,7,0]
board[8] = [0,0,4,0,0,0,0,0,0]

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

print(validate(board))