//
// Created by zhaoc_h on 2025/12/1.
//

#pragma once

#include <QObject>
#include <vector>
#include "ChessPiece.h"

class ChessLogic : public QObject {
    Q_OBJECT

public:
    explicit ChessLogic(QObject *parent = nullptr);

    void handleClick(int row, int col);
    bool isValidMove(int row, int col) const;
    void placePiece(int row, int col);
    PieceColor getCurrentPlayer() const { return m_currentPlayer; }
    PieceColor getPieceAt(int row, int col) const;

    signals:
        void boardUpdated();
    void gameOver(PieceColor winner);

private:
    static const int BOARD_SIZE = 15;
    PieceColor m_board[BOARD_SIZE][BOARD_SIZE];
    PieceColor m_currentPlayer;
    bool m_gameOver;

    bool checkWin(int row, int col);
};