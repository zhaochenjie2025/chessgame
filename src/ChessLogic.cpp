//
// Created by zhaoc_h on 2025/12/1.
//

// ChessLogic.cpp
#include "ChessLogic.h"

ChessLogic::ChessLogic(QObject* parent)
    : QObject(parent)
    , m_currentPlayer(PieceColor::Black)
    , m_gameOver(false)
{
    // 初始化棋盘
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            m_board[i][j] = PieceColor::Empty;
        }
    }
}

void ChessLogic::handleClick(int row, int col)
{
    if (m_gameOver || !isValidMove(row, col)) {
        return;
    }

    placePiece(row, col);

    if (checkWin(row, col)) {
        m_gameOver = true;
        emit gameOver(m_currentPlayer);
    } else {
        // 切换玩家
        m_currentPlayer = (m_currentPlayer == PieceColor::Black) ?
                          PieceColor::White : PieceColor::Black;
        emit boardUpdated();
    }
}

bool ChessLogic::isValidMove(int row, int col) const
{
    return (row >= 0 && row < BOARD_SIZE &&
            col >= 0 && col < BOARD_SIZE &&
            m_board[row][col] == PieceColor::Empty);
}

void ChessLogic::placePiece(int row, int col)
{
    m_board[row][col] = m_currentPlayer;
}

bool ChessLogic::checkWin(int row, int col)
{
    PieceColor player = m_board[row][col];
    if (player == PieceColor::Empty) return false;

    // 检查四个方向是否有连续5个棋子
    int directions[4][2] = {{0,1}, {1,0}, {1,1}, {1,-1}};

    for (auto& dir : directions) {
        int count = 1; // 包含当前棋子

        // 正向检查
        for (int i = 1; i <= 4; ++i) {
            int r = row + dir[0] * i;
            int c = col + dir[1] * i;
            if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE &&
                m_board[r][c] == player) {
                count++;
            } else {
                break;
            }
        }

        // 反向检查
        for (int i = 1; i <= 4; ++i) {
            int r = row - dir[0] * i;
            int c = col - dir[1] * i;
            if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE &&
                m_board[r][c] == player) {
                count++;
            } else {
                break;
            }
        }

        if (count >= 5) {
            return true;
        }
    }

    return false;
}

PieceColor ChessLogic::getPieceAt(int row, int col) const
{
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
        return m_board[row][col];
    }
    return PieceColor::Empty;
}