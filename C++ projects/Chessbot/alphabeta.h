#ifndef ALPHABETA_H
#define ALPHABETA_H

#include "chessboard.h"

#include <functional>
#include <limits>
#include <algorithm>

using EvaluationFunction = std::function<double(const ChessBoard&)>;

constexpr double INF  = std::numeric_limits<double>::infinity();
constexpr double MATE = 1e9;

double maxValue(
    const ChessBoard& board,
    int depth,
    double alpha,
    double beta,
    EvaluationFunction evaluate);

double minValue(
    const ChessBoard& board,
    int depth,
    double alpha,
    double beta,
    EvaluationFunction evaluate);

inline double alphaBeta(
    const ChessBoard& board,
    int depth,
    double alpha,
    double beta,
    char player,
    EvaluationFunction evaluate)
{
    if(player == WHITE)
        return maxValue(board, depth, alpha, beta, evaluate);

    return minValue(board, depth, alpha, beta, evaluate);
}

inline double maxValue(
    const ChessBoard& board,
    int depth,
    double alpha,
    double beta,
    EvaluationFunction evaluate)
{



    if(depth == 0)
        return evaluate(board);

    std::vector<std::string> moves = board.getValidMoves(WHITE);

    if(moves.empty())
    {
        if(board.kingInCheck(WHITE))
            return -MATE;

        return 0.0;
    }

    double value = -INF;

    for(const std::string& move : moves)
    {
        ChessBoard child = board.executeMove(move);

        value = std::max(
            value,
            minValue(child,
                     depth - 1,
                     alpha,
                     beta,
                     evaluate));

        alpha = std::max(alpha, value);

        if(alpha >= beta)
            break;
    }

    return value;
}

inline double minValue(
    const ChessBoard& board,
    int depth,
    double alpha,
    double beta,
    EvaluationFunction evaluate)
{


    if(depth == 0)
        return evaluate(board);

    std::vector<std::string> moves = board.getValidMoves(BLACK);

    if(moves.empty())
    {
        if(board.kingInCheck(BLACK))
            return MATE;

        return 0.0;
    }

    double value = INF;

    for(const std::string& move : moves)
    {
        ChessBoard child = board.executeMove(move);

        value = std::min(
            value,
            maxValue(child,
                     depth - 1,
                     alpha,
                     beta,
                     evaluate));

        beta = std::min(beta, value);

        if(alpha >= beta)
            break;
    }

    return value;
}

inline std::string getBestMove(
    const ChessBoard& board,
    int depth,
    char player,
    EvaluationFunction evaluate)
{
    std::vector<std::string> moves = board.getValidMoves(player);

    if(moves.empty())
        return "";

    std::string bestMove = moves.front();

    if(player == WHITE)
    {
        double bestScore = -INF;

        for(const std::string& move : moves)
        {
            ChessBoard child = board.executeMove(move);

            double score = minValue(
                child,
                depth - 1,
                -INF,
                INF,
                evaluate);

            if(score > bestScore)
            {
                bestScore = score;
                bestMove = move;
            }
        }
    }
    else
    {
        double bestScore = INF;

        for(const std::string& move : moves)
        {
            ChessBoard child = board.executeMove(move);

            double score = maxValue(
                child,
                depth - 1,
                -INF,
                INF,
                evaluate);

            if(score < bestScore)
            {
                bestScore = score;
                bestMove = move;
            }
        }
    }

    return bestMove;
}

#endif