//
// Created by zhaoc_h on 2025/12/1.
//

#pragma once
#include "ChessLogic.h"
#include "ChessBoardWidget.h"
#include <memory>
#include <QMainWindow>

class ChessGame:public QMainWindow {
    Q_OBJECT
public:
    ChessGame(QWidget* parent = nullptr);
    ~ChessGame();
private:
    ChessBoardWidget* m_boardWidget;
    ChessLogic* m_gameLogic;
};