//
// Created by zhaoc_h on 2025/12/1.
//

#include "ChessGame.h"
#include <QVBoxLayout>
#include <QWidget>
#include "ChessBoardWidget.h"
#include "ChessLogic.h"

ChessGame::ChessGame(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("棋类游戏");
    resize(600, 600);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    m_gameLogic = new ChessLogic(this);
    m_boardWidget = new ChessBoardWidget(m_gameLogic, this);

    layout->addWidget(m_boardWidget);

    connect(m_boardWidget, &ChessBoardWidget::positionClicked,
            m_gameLogic, &ChessLogic::handleClick);
}

ChessGame::~ChessGame() = default;

