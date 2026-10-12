#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

const char WHITE = 1;
const char BLACK = -1;

const char PAWN = 'P';
const char KNIGHT = 'N';
const char BISHOP = 'B';
const char ROOK = 'R';
const char QUEEN = 'Q';
const char KING = 'K';

const char pawn = 'p';
const char knight = 'n';
const char bishop = 'b';
const char rook = 'r';
const char queen = 'q';
const char king = 'k';

const char EMPTY = 'E';

const char a = 0;
const char b = 1;
const char c = 2;
const char d = 3;
const char e = 4;
const char f = 5;
const char g = 6;
const char h = 7;

class ChessBoard {

    public:

    vector<vector<char>> board = vector<vector<char>>(8, vector<char>(8, EMPTY));

    bool white_O_O;
    bool white_O_O_O;
    bool black_O_O;
    bool black_O_O_O;

    vector<bool> white_enpassantable;
    vector<bool> black_enpassantable;
    
    /*
     * Default constructor.
     * Initializes the chessboard to its starting state.
     * 
     */
    ChessBoard()
    {
        board = {
            {ROOK,   PAWN,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  pawn,   rook  },

            {KNIGHT, PAWN,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  pawn,   knight},

            {BISHOP, PAWN,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  pawn,   bishop},

            {QUEEN,  PAWN,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  pawn,   queen },

            {KING,   PAWN,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  pawn,   king  },

            {BISHOP, PAWN,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  pawn,   bishop},

            {KNIGHT, PAWN,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  pawn,   knight},

            {ROOK,   PAWN,  EMPTY,  EMPTY,  EMPTY,  EMPTY,  pawn,   rook  }
        };

        white_O_O = true;
        white_O_O_O = true;
        black_O_O = true;
        black_O_O_O = true;

        white_enpassantable = vector<bool>(8, false);
        black_enpassantable = vector<bool>(8, false);

    }

    /*
     * Tests whether the king belonging to player is in check.
     * Returns a boolean accordingly.
     *
     */
    bool kingInCheck(char player) const
    {

        char myKing;
        char enemyPawn, enemyKnight, enemyBishop, enemyRook, enemyQueen, enemyKing;

        if(player == WHITE){
            myKing = KING;

            enemyPawn   = pawn;
            enemyKnight = knight;
            enemyBishop = bishop;
            enemyRook   = rook;
            enemyQueen  = queen;
            enemyKing   = king;
        }
        else{
            myKing = king;

            enemyPawn   = PAWN;
            enemyKnight = KNIGHT;
            enemyBishop = BISHOP;
            enemyRook   = ROOK;
            enemyQueen  = QUEEN;
            enemyKing   = KING;
        }

        // Find king
        int kx = -1, ky = -1;

        for(int x = 0; x < 8; x++){
            for(int y = 0; y < 8; y++){
                if(board[x][y] == myKing){
                    kx = x;
                    ky = y;
                    break;
                }
            }
            if(kx != -1) break;
        }

        if(kx == -1) return false; // king not found

        //--------------------
        // Pawn attacks
        //--------------------

        if(player == WHITE){
            // Black pawns attack downwards (towards decreasing y)
            if(kx > 0 && ky < 7 && board[kx-1][ky+1] == pawn) return true;
            if(kx < 7 && ky < 7 && board[kx+1][ky+1] == pawn) return true;
        }
        else{
            // White pawns attack upwards (towards increasing y)
            if(kx > 0 && ky > 0 && board[kx-1][ky-1] == PAWN) return true;
            if(kx < 7 && ky > 0 && board[kx+1][ky-1] == PAWN) return true;
        }

        //--------------------
        // Knight attacks
        //--------------------

        int knightMoves[8][2] = {
            {-2,-1},{-2,1},
            {-1,-2},{-1,2},
            {1,-2},{1,2},
            {2,-1},{2,1}
        };

        for(auto &m : knightMoves){
            int x = kx + m[0];
            int y = ky + m[1];

            if(x >= 0 && x < 8 && y >= 0 && y < 8){
                if(board[x][y] == enemyKnight)
                    return true;
            }
        }

        //--------------------
        // Bishop / Queen
        //--------------------

        int bishopDirs[4][2] = {
            {1,1},
            {1,-1},
            {-1,1},
            {-1,-1}
        };

        for(auto &d : bishopDirs){
            int x = kx + d[0];
            int y = ky + d[1];

            while(x >= 0 && x < 8 && y >= 0 && y < 8){

                if(board[x][y] != EMPTY){
                    if(board[x][y] == enemyBishop || board[x][y] == enemyQueen)
                        return true;
                    break;
                }

                x += d[0];
                y += d[1];
            }
        }

        //--------------------
        // Rook / Queen
        //--------------------

        int rookDirs[4][2] = {
            {1,0},
            {-1,0},
            {0,1},
            {0,-1}
        };

        for(auto &d : rookDirs){
            int x = kx + d[0];
            int y = ky + d[1];

            while(x >= 0 && x < 8 && y >= 0 && y < 8){

                if(board[x][y] != EMPTY){
                    if(board[x][y] == enemyRook || board[x][y] == enemyQueen)
                        return true;
                    break;
                }

                x += d[0];
                y += d[1];
            }
        }

        //--------------------
        // Enemy king
        //--------------------

        for(int dx = -1; dx <= 1; dx++){
            for(int dy = -1; dy <= 1; dy++){

                if(dx == 0 && dy == 0)
                    continue;

                int x = kx + dx;
                int y = ky + dy;

                if(x >= 0 && x < 8 && y >= 0 && y < 8){
                    if(board[x][y] == enemyKing)
                        return true;
                }
            }
        }

        return false;
    }


    private:

    inline bool isWhitePiece(char p) const
    {
        switch (p)
        {
            case PAWN:
            case KNIGHT:
            case BISHOP:
            case ROOK:
            case QUEEN:
            case KING:
                return true;
            default:
                return false;
        }
    }

    inline bool isBlackPiece(char p) const
    {
        switch (p)
        {
            case pawn:
            case knight:
            case bishop:
            case rook:
            case queen:
            case king:
                return true;
            default:
                return false;
        }
    }

    string makeMove(char piece, int fromX, int fromY, int toX, int toY) const
    {
        string move;

        move += piece;
        move += char('a' + fromX);
        move += char('1' + fromY);
        move += char('a' + toX);
        move += char('1' + toY);

        return move;
    }

    void getPawnMoves(int x, int y, vector<string>& moves) const
    {
        char piece = board[x][y];
        bool white = (piece == PAWN);

        int direction      = white ? 1 : -1;
        int startRank      = white ? 1 : 6;
        int promotionRank  = white ? 6 : 1;
        int enPassantRank  = white ? 4 : 3;

        const vector<bool>& enemyEnPassant =
            white ? black_enpassantable : white_enpassantable;

        char promotions[4] =
        {
            white ? QUEEN  : queen,
            white ? ROOK   : rook,
            white ? BISHOP : bishop,
            white ? KNIGHT : knight
        };

        //-------------------------
        // One square forward
        //-------------------------

        if(board[x][y + direction] == EMPTY)
        {
            if(y == promotionRank)
            {
                for(char promotion : promotions)
                    moves.push_back(makeMove(piece, x, y, x, y + direction) + promotion);
            }
            else
            {
                moves.push_back(makeMove(piece, x, y, x, y + direction));
            }

            //-------------------------
            // Two squares forward
            //-------------------------

            if(y == startRank &&
            board[x][y + 2 * direction] == EMPTY)
            {
                moves.push_back(makeMove(piece, x, y, x, y + 2 * direction));
            }
        }

        //-------------------------
        // Captures
        //-------------------------

        for(int dx : {-1, 1})
        {
            int nx = x + dx;
            int ny = y + direction;

            if(nx < 0 || nx > 7)
                continue;

            if((white && isBlackPiece(board[nx][ny])) ||
            (!white && isWhitePiece(board[nx][ny])))
            {
                if(y == promotionRank)
                {
                    for(char promotion : promotions)
                        moves.push_back(makeMove(piece, x, y, nx, ny) + promotion);
                }
                else
                {
                    moves.push_back(makeMove(piece, x, y, nx, ny));
                }
            }

            //-------------------------
            // En passant
            //-------------------------

            if(y == enPassantRank && enemyEnPassant[nx])
            {
                moves.push_back(makeMove(piece, x, y, nx, ny));
            }
        }
    }
    void getKnightMoves(int x, int y, vector<string>& moves) const
    {
        char piece = board[x][y];
        bool white = isWhitePiece(piece);

        static const int offsets[8][2] = {
            {-2, -1}, {-2,  1},
            {-1, -2}, {-1,  2},
            { 1, -2}, { 1,  2},
            { 2, -1}, { 2,  1}
        };

        for (const auto& offset : offsets)
        {
            int nx = x + offset[0];
            int ny = y + offset[1];

            // Out of bounds
            if (nx < 0 || nx > 7 || ny < 0 || ny > 7)
                continue;

            // Can't capture your own piece
            if ((white && isWhitePiece(board[nx][ny])) ||
                (!white && isBlackPiece(board[nx][ny])))
                continue;

            moves.push_back(makeMove(piece, x, y, nx, ny));
        }
    }
    void getBishopMoves(int x, int y, vector<string>& moves) const
    {
        char piece = board[x][y];
        bool white = isWhitePiece(piece);

        static const int directions[4][2] =
        {
            { 1,  1},
            { 1, -1},
            {-1,  1},
            {-1, -1}
        };

        for(const auto& direction : directions)
        {
            int nx = x + direction[0];
            int ny = y + direction[1];

            while(nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
            {
                if(board[nx][ny] == EMPTY)
                {
                    moves.push_back(makeMove(piece, x, y, nx, ny));
                }
                else
                {
                    // Friendly piece blocks the ray
                    if((white && isWhitePiece(board[nx][ny])) ||
                    (!white && isBlackPiece(board[nx][ny])))
                    {
                        break;
                    }

                    // Enemy piece can be captured
                    moves.push_back(makeMove(piece, x, y, nx, ny));
                    break;
                }

                nx += direction[0];
                ny += direction[1];
            }
        }
    }
    void getRookMoves(int x, int y, vector<string>& moves) const
    {
        char piece = board[x][y];
        bool white = isWhitePiece(piece);

        static const int directions[4][2] =
        {
            { 1,  0},
            {-1,  0},
            { 0,  1},
            { 0, -1}
        };

        for(const auto& direction : directions)
        {
            int nx = x + direction[0];
            int ny = y + direction[1];

            while(nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
            {
                if(board[nx][ny] == EMPTY)
                {
                    moves.push_back(makeMove(piece, x, y, nx, ny));
                }
                else
                {
                    // Friendly piece blocks the ray
                    if((white && isWhitePiece(board[nx][ny])) ||
                    (!white && isBlackPiece(board[nx][ny])))
                    {
                        break;
                    }

                    // Enemy piece can be captured
                    moves.push_back(makeMove(piece, x, y, nx, ny));
                    break;
                }

                nx += direction[0];
                ny += direction[1];
            }
        }
    }
    void getQueenMoves(int x, int y, vector<string>& moves) const
    {
        getBishopMoves(x, y, moves);
        getRookMoves(x, y, moves);
    }
    void getKingMoves(int x, int y, vector<string>& moves) const
    {
        char piece = board[x][y];
        bool white = isWhitePiece(piece);

        static const int offsets[8][2] =
        {
            {-1,-1}, {-1,0}, {-1,1},
            { 0,-1},         { 0,1},
            { 1,-1}, {1,0},  {1,1}
        };

        //-------------------------
        // Normal king moves
        //-------------------------

        for(const auto& offset : offsets)
        {
            int nx = x + offset[0];
            int ny = y + offset[1];

            if(nx < 0 || nx > 7 || ny < 0 || ny > 7)
                continue;

            if((white && isWhitePiece(board[nx][ny])) ||
            (!white && isBlackPiece(board[nx][ny])))
                continue;

            moves.push_back(makeMove(piece, x, y, nx, ny));
        }

        //-------------------------
        // Castling
        //-------------------------

        if(white)
        {
            // Kingside
            if(white_O_O &&
            board[f][0] == EMPTY &&
            board[g][0] == EMPTY)
            {
                moves.push_back("Ke1g1");
            }

            // Queenside
            if(white_O_O_O &&
            board[d][0] == EMPTY &&
            board[c][0] == EMPTY &&
            board[b][0] == EMPTY)
            {
                moves.push_back("Ke1c1");
            }
        }
        else
        {
            // Kingside
            if(black_O_O &&
            board[f][7] == EMPTY &&
            board[g][7] == EMPTY)
            {
                moves.push_back("ke8g8");
            }

            // Queenside
            if(black_O_O_O &&
            board[d][7] == EMPTY &&
            board[c][7] == EMPTY &&
            board[b][7] == EMPTY)
            {
                moves.push_back("ke8c8");
            }
        }
    }

    public:

    /*
     * Expects a fully disambiguated move (Pe2e4, pe7e5 etc.)
     * Pawn promotions are denoted such as Pe7e8Q or pg2f1n
     *
     */
    ChessBoard executeMove(string move) const
    {
        ChessBoard next = *this;

        char piece = move[0];

        int fromX = move[1] - 'a';
        int fromY = move[2] - '1';

        int toX = move[3] - 'a';
        int toY = move[4] - '1';

        bool whiteMove = (piece >= 'A' && piece <= 'Z');

        // Reset en-passant flags
        next.white_enpassantable.assign(8, false);
        next.black_enpassantable.assign(8, false);

        //--------------------------------------------------
        // Castling
        //--------------------------------------------------

        if(piece == KING && fromX == e && fromY == 0)
        {
            next.white_O_O = false;
            next.white_O_O_O = false;

            if(toX == g)
            {
                // O-O
                next.board[h][0] = EMPTY;
                next.board[f][0] = ROOK;
            }
            else if(toX == c)
            {
                // O-O-O
                next.board[a][0] = EMPTY;
                next.board[d][0] = ROOK;
            }
        }

        if(piece == king && fromX == e && fromY == 7)
        {
            next.black_O_O = false;
            next.black_O_O_O = false;

            if(toX == g)
            {
                next.board[h][7] = EMPTY;
                next.board[f][7] = rook;
            }
            else if(toX == c)
            {
                next.board[a][7] = EMPTY;
                next.board[d][7] = rook;
            }
        }

        //--------------------------------------------------
        // Rook movement -> lose castling rights
        //--------------------------------------------------

        if(piece == ROOK)
        {
            if(fromX == a && fromY == 0)
                next.white_O_O_O = false;
            else if(fromX == h && fromY == 0)
                next.white_O_O = false;
        }

        if(piece == rook)
        {
            if(fromX == a && fromY == 7)
                next.black_O_O_O = false;
            else if(fromX == h && fromY == 7)
                next.black_O_O = false;
        }

        //--------------------------------------------------
        // Capturing a rook also removes castling rights
        //--------------------------------------------------

        char captured = next.board[toX][toY];

        if(captured == ROOK)
        {
            if(toX == a && toY == 0)
                next.white_O_O_O = false;
            else if(toX == h && toY == 0)
                next.white_O_O = false;
        }

        if(captured == rook)
        {
            if(toX == a && toY == 7)
                next.black_O_O_O = false;
            else if(toX == h && toY == 7)
                next.black_O_O = false;
        }

        //--------------------------------------------------
        // En-passant capture
        //--------------------------------------------------

        if(piece == PAWN)
        {
            if(fromX != toX && captured == EMPTY)
            {
                next.board[toX][toY - 1] = EMPTY;
            }

            if(fromY == 1 && toY == 3)
                next.white_enpassantable[toX] = true;
        }

        if(piece == pawn)
        {
            if(fromX != toX && captured == EMPTY)
            {
                next.board[toX][toY + 1] = EMPTY;
            }

            if(fromY == 6 && toY == 4)
                next.black_enpassantable[toX] = true;
        }

        //--------------------------------------------------
        // Move piece
        //--------------------------------------------------

        next.board[fromX][fromY] = EMPTY;
        next.board[toX][toY] = piece;

        //--------------------------------------------------
        // Promotion
        //--------------------------------------------------

        if(move.length() == 6)
        {
            next.board[toX][toY] = move[5];
        }

        return next;
    }

    /*
     * Returns the list of all moves that player can make in the position, IGNORING CHECKS.
     * Returns fully disambiguated moves (Pe2e4, pe7e5 etc.)
     * Pawn promotions are denoted such as Pe7e8Q or pg2f1n
     *
     */
    vector<string> getPseudoValidMoves(char player) const
    {
        vector<string> moves;

        for (int x = 0; x < 8; ++x)
        {
            for (int y = 0; y < 8; ++y)
            {
                char piece = board[x][y];

                if (piece == EMPTY)
                    continue;

                if (player == WHITE && !isWhitePiece(piece))
                    continue;

                if (player == BLACK && !isBlackPiece(piece))
                    continue;

                switch (piece)
                {
                    case PAWN:
                    case pawn:
                        getPawnMoves(x, y, moves);
                        break;

                    case KNIGHT:
                    case knight:
                        getKnightMoves(x, y, moves);
                        break;

                    case BISHOP:
                    case bishop:
                        getBishopMoves(x, y, moves);
                        break;

                    case ROOK:
                    case rook:
                        getRookMoves(x, y, moves);
                        break;

                    case QUEEN:
                    case queen:
                        getQueenMoves(x, y, moves);
                        break;

                    case KING:
                    case king:
                        getKingMoves(x, y, moves);
                        break;
                }
            }
        }

        return moves;
    }

    /*
     * Returns the list of all valid moves that player can make in the position, considering checks.
     * Returns fully disambiguated moves (Pe2e4, pe7e5 etc.)
     * Pawn promotions are denoted such as Pe7e8Q or pg2f1n
     *
     */
    vector<string> getValidMoves(char player) const
    {
        vector<string> validMoves;

        for(const string& move : getPseudoValidMoves(player))
        {
            // Additional castling legality checks
            if(move == "Ke1g1")
            {
                if(kingInCheck(WHITE))
                    continue;

                ChessBoard temp = executeMove("Ke1f1");
                if(temp.kingInCheck(WHITE))
                    continue;
            }
            else if(move == "Ke1c1")
            {
                if(kingInCheck(WHITE))
                    continue;

                ChessBoard temp = executeMove("Ke1d1");
                if(temp.kingInCheck(WHITE))
                    continue;
            }
            else if(move == "ke8g8")
            {
                if(kingInCheck(BLACK))
                    continue;

                ChessBoard temp = executeMove("ke8f8");
                if(temp.kingInCheck(BLACK))
                    continue;
            }
            else if(move == "ke8c8")
            {
                if(kingInCheck(BLACK))
                    continue;

                ChessBoard temp = executeMove("ke8d8");
                if(temp.kingInCheck(BLACK))
                    continue;
            }

            ChessBoard next = executeMove(move);

            if(!next.kingInCheck(player))
                validMoves.push_back(move);
        }

        return validMoves;
    }

    bool isCheckmate(char player) const
    {
        return kingInCheck(player) && getValidMoves(player).empty();
    }

    bool isStalemate(char player) const
    {
        return !kingInCheck(player) && getValidMoves(player).empty();
    }

    void print() const
    {
        cout << "\n";

        for(int y = 7; y >= 0; --y)
        {
            cout << y + 1 << " ";

            for(int x = 0; x < 8; ++x)
            {
                char piece = board[x][y];

                if(piece == EMPTY)
                    cout << ". ";
                else
                    cout << piece << " ";
            }

            cout << "\n";
        }

        cout << "  a b c d e f g h\n\n";

        cout << "White: ";
        if(white_O_O)   cout << "O-O ";
        if(white_O_O_O) cout << "O-O-O ";
        if(!white_O_O && !white_O_O_O) cout << "-";
        cout << "\n";

        cout << "Black: ";
        if(black_O_O)   cout << "O-O ";
        if(black_O_O_O) cout << "O-O-O ";
        if(!black_O_O && !black_O_O_O) cout << "-";
        cout << "\n";

        cout << "White EP:";
        for(int i = 0; i < 8; ++i)
            if(white_enpassantable[i])
                cout << " " << char('a' + i);

        cout << "\n";

        cout << "Black EP:";
        for(int i = 0; i < 8; ++i)
            if(black_enpassantable[i])
                cout << " " << char('a' + i);

        cout << "\n\n";
    }

};

#endif