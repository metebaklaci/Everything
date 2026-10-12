#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "chessboard.h"

double materialEvaluation(const ChessBoard& board)
{
    double score = 0.0;

    for(int x = 0; x < 8; ++x)
    {
        for(int y = 0; y < 8; ++y)
        {
            switch(board.board[x][y])
            {
                case PAWN:   score += 1.0; break;
                case KNIGHT: score += 3.0; break;
                case BISHOP: score += 3.25; break;
                case ROOK:   score += 5.0; break;
                case QUEEN:  score += 9.0; break;

                case pawn:   score -= 1.0; break;
                case knight: score -= 3.0; break;
                case bishop: score -= 3.25; break;
                case rook:   score -= 5.0; break;
                case queen:  score -= 9.0; break;
            }
        }
    }

    return score;
}

inline int pieceValue(char piece)
{
    switch(piece)
    {
        case PAWN:   return 100;
        case pawn:   return -100;

        case KNIGHT: return 320;
        case knight: return -320;

        case BISHOP: return 330;
        case bishop: return -330;

        case ROOK:   return 500;
        case rook:   return -500;

        case QUEEN:  return 900;
        case queen:  return -900;

        default:     return 0;
    }
}


const int pawnTable[8][8] =
{
    {  0, 50, 10,  5,  0,  5,  5,  0},
    {  0, 50, 10,-10,  0, 10,  5,  0},
    {  0, 50, 20,-10,  0, 10, 10,  0},
    {  0, 50, 30,  0, 20,-20,  0,  0},
    {  0, 50, 30,  0, 20,-20,  0,  0},
    {  0, 50, 20,-10,  0, 10, 10,  0},
    {  0, 50, 10,-10,  0, 10,  5,  0},
    {  0, 50, 10,  5,  0,  5,  5,  0}
};

const int knightTable[8][8] =
{
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

const int bishopTable[8][8] =
{
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

const int rookTable[8][8] =
{
    { 0, 5,-5,-5,-5,-5, 0, 0},
    { 0,10, 0, 0, 0, 0, 0, 0},
    { 0,10, 0, 0, 0, 0, 0, 0},
    { 0,10, 0, 0, 0, 0, 0, 5},
    { 0,10, 0, 0, 0, 0, 0, 5},
    { 0,10, 0, 0, 0, 0, 0, 0},
    { 0,10, 0, 0, 0, 0, 0, 0},
    { 0, 5,-5,-5,-5,-5, 0, 0}
};

const int queenTable[8][8] =
{
    {-20,-10,-10, -5,  0,-10,-10,-20},
    {-10,  0,  0,  0,  0,  5,  0,-10},
    {-10,  0,  5,  5,  5,  5,  5,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {-10,  0,  5,  5,  5,  5,  5,-10},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

const int kingMiddlegameTable[8][8] =
{
    { 20, 20,-10,-20,-30,-30,-30,-30},
    { 30, 20,-20,-30,-40,-40,-40,-40},
    { 10,  0,-20,-30,-40,-40,-40,-40},
    {  0,  0,-20,-40,-50,-50,-50,-50},
    {  0,  0,-20,-40,-50,-50,-50,-50},
    { 10,  0,-20,-30,-40,-40,-40,-40},
    { 30, 20,-20,-30,-40,-40,-40,-40},
    { 20, 20,-10,-20,-30,-30,-30,-30}
};

const int kingEndgameTable[8][8] =
{
    {-50,-30,-30,-30,-30,-30,-30,-50},
    {-30,-20,-10,-10,-10,-10,-20,-30},
    {-30,-10, 20, 30, 30, 20,-10,-30},
    {-30,  0, 30, 40, 40, 30,  0,-30},
    {-30,  0, 30, 40, 40, 30,  0,-30},
    {-30,-10, 20, 30, 30, 20,-10,-30},
    {-30,-20,-10,-10,-10,-10,-20,-30},
    {-50,-30,-30,-30,-30,-30,-30,-50}
};



double advancedEvaluation(const ChessBoard& board){

    int eval = 0;
    int non_pawn_value = 0;

    pair<int,int> white_king_coords;
    pair<int,int> black_king_coords;

    int whiteBishops = 0;
    int blackBishops = 0;

    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){

            char piece = board.board[i][j];

            eval += pieceValue(piece);
            if(piece == PAWN){
                eval += pawnTable[i][j];
                if(board.board[i][j+1] == PAWN) eval -= 15;
            } else if (piece == pawn){
                eval -= pawnTable[i][7-j];
                if(board.board[i][j-1] == PAWN) eval += 15;
            } 
            
            else if (piece == KNIGHT){
                eval += knightTable[i][j];
                non_pawn_value += 3;
            } else if (piece == knight){
                eval -= knightTable[i][7-j];
                non_pawn_value += 3;
            } 
            
            else if (piece == BISHOP){
                eval += bishopTable[i][j];
                non_pawn_value += 3;
                whiteBishops++;
            } else if (piece == bishop){
                eval -= bishopTable[i][7-j];
                non_pawn_value += 3;
                blackBishops++;
            }

            else if (piece == ROOK){
                eval += rookTable[i][j];
                non_pawn_value += 5;
                bool fileopen = true;
                for(int k = 0; k < 8; ++k){
                    if(board.board[i][k] == PAWN){
                        fileopen = false;
                        break;
                    }
                }
                if(fileopen) eval += 20;
            } else if (piece == rook){
                eval -= rookTable[i][7-j];
                non_pawn_value += 5;
                bool fileopen = true;
                for(int k = 0; k < 8; ++k){
                    if(board.board[i][k] == pawn){
                        fileopen = false;
                        break;
                    }
                }
                if(fileopen) eval -= 20;
            }

            else if (piece == QUEEN){
                eval += queenTable[i][j];
                non_pawn_value += 9;
            } else if (piece == queen){
                eval -= queenTable[i][7-j];
                non_pawn_value += 9;
            }

            else if (piece == KING){
                white_king_coords = pair<int,int>(i,j);
            } else if (piece == king){
                black_king_coords = pair<int,int>(i,j);
            }
        }
    }

    if(non_pawn_value >= 26){//Endgame threshold
        eval += kingMiddlegameTable[white_king_coords.first][white_king_coords.second];
        eval -= kingMiddlegameTable[7 - black_king_coords.first][7 - black_king_coords.second];
    } else {
        eval += kingEndgameTable[white_king_coords.first][white_king_coords.second];
        eval -= kingEndgameTable[7 - black_king_coords.first][7 - black_king_coords.second];
    }

    if(whiteBishops >= 2)
        eval += 40;

    if(blackBishops >= 2)
        eval -= 40;

    return eval;

}

#endif