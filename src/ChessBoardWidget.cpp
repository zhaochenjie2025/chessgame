// ChessBoardWidget.cpp
#include "ChessBoardWidget.h"
#include "ChessLogic.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <QDebug>

ChessBoardWidget::ChessBoardWidget(ChessLogic* gameLogic, QWidget* parent)
    : QWidget(parent)
    , m_gameLogic(gameLogic)
    , m_boardSize(15) // 修正为15x15棋盘，与注释一致
    , m_cellSize(30)
{
    setMinimumSize((m_boardSize + 1) * m_cellSize, (m_boardSize + 1) * m_cellSize);
    setMouseTracking(true);
    loadPieceImages();
}

void ChessBoardWidget::loadPieceImages()
{
    QStringList blackPaths = {
        ":/images/black_piece.png",
        "resources/images/black_piece.png",
        "images/black_piece.png"
    };

    QStringList whitePaths = {
        ":/images/white_piece.png",
        "resources/images/white_piece.png",
        "images/white_piece.png"
    };

    // 尝试加载黑棋图片
    for (const QString& path : blackPaths) {
        if (m_blackPiecePixmap.load(path)) {
            qDebug() << "Loaded black piece from:" << path;
            break;
        }
    }

    // 尝试加载白棋图片
    for (const QString& path : whitePaths) {
        if (m_whitePiecePixmap.load(path)) {
            qDebug() << "Loaded white piece from:" << path;
            break;
        }
    }

    // 缩放图片
    int pieceSize = m_cellSize - 6; // 稍小的棋子
    if (!m_blackPiecePixmap.isNull()) {
        m_blackPiecePixmap = m_blackPiecePixmap.scaled(pieceSize, pieceSize,
                                                     Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if (!m_whitePiecePixmap.isNull()) {
        m_whitePiecePixmap = m_whitePiecePixmap.scaled(pieceSize, pieceSize,
                                                     Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}

void ChessBoardWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制棋盘背景
    painter.fillRect(rect(), QColor(220, 179, 92)); // 木质黄色背景

    drawBoard(painter);
    drawPieces(painter);
}

// 修正后的drawBoard函数
void ChessBoardWidget::drawBoard(QPainter& painter)
{
    painter.setPen(QPen(Qt::black, 1));
    int boardOffset = m_cellSize;

    for (int i = 0; i < m_boardSize; ++i) {
        // 水平线
        painter.drawLine(boardOffset, boardOffset + i * m_cellSize,
                        boardOffset + (m_boardSize - 1) * m_cellSize, boardOffset + i * m_cellSize);
        // 垂直线
        painter.drawLine(boardOffset + i * m_cellSize, boardOffset,
                        boardOffset + i * m_cellSize, boardOffset + (m_boardSize - 1) * m_cellSize);
    }

    // 绘制棋盘上的星位（可选）
    if (m_boardSize == 15 || m_boardSize == 19) {
        painter.setBrush(Qt::black);
        int starPoints[] = {3, 7, 11}; // 15x15棋盘的星位
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                QPoint center = boardToPixel(starPoints[i], starPoints[j]);
                painter.drawEllipse(center, 3, 3);
            }
        }
    }
}