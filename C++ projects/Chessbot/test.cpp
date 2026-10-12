#include "chessboard.h"
#include "alphabeta.h"
#include "heuristics.h"

#include <bits/stdc++.h>

using namespace std;

int main()
{
    ChessBoard board;

    char choice;

    while(true)
    {
        cout << "Play as White or Black? (w/b): ";
        cin >> choice;

        choice = tolower(choice);

        if(choice == 'w' || choice == 'b')
            break;

        cout << "Please enter 'w' or 'b'.\n";
    }

    char humanPlayer  = (choice == 'w') ? WHITE : BLACK;
    char enginePlayer = (humanPlayer == WHITE) ? BLACK : WHITE;

    char sideToMove = WHITE;

    while(true)
    {
        board.print();

        if(board.isCheckmate(WHITE))
        {
            cout << "Checkmate! Black wins.\n";
            break;
        }

        if(board.isCheckmate(BLACK))
        {
            cout << "Checkmate! White wins.\n";
            break;
        }

        if(board.isStalemate(WHITE) || board.isStalemate(BLACK))
        {
            cout << "Stalemate.\n";
            break;
        }

        if(sideToMove == humanPlayer)
        {
            //-------------------------
            // Human move
            //-------------------------

            vector<string> legal = board.getValidMoves(humanPlayer);

            string move;

            while(true)
            {
                cout << "Your move: ";
                cin >> move;

                if(find(legal.begin(), legal.end(), move) != legal.end())
                    break;

                cout << "Illegal move.\n";
            }

            board = board.executeMove(move);
        }
        else
        {
            //-------------------------
            // Engine move
            //-------------------------

            cout << "Thinking...\n";

            string bestMove =
                getBestMove(board, 4, enginePlayer, advancedEvaluation);

            cout << "Engine plays: " << bestMove << "\n";

            board = board.executeMove(bestMove);
        }

        sideToMove = (sideToMove == WHITE) ? BLACK : WHITE;
    }

    return 0;
}