//
// Created by zhaoc_h on 2025/12/1.
//

#pragma once
#include<QWidget>
#include<QPainter>
#include<QMouseEvent>
#include "ChessPiece.h"

class ChessLogic;

class ChessBoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoardWidget(ChessLogic* gameLogic, QWidget *parent = nullptr);
    
    void setBoardSize(int size);
    
signals:
    void positionClicked(int row, int col);
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
private:
    ChessLogic* m_gameLogic;
    int m_boardSize;
    int m_cellSize;
    bool m_imagesLoaded;

    QPixmap m_blackPiecePixmap;
    QPixmap m_whitePiecePixmap;

    void loadPieceImages();
    void ensureImagesLoaded();

    void drawBoard(QPainter& painter);
    void drawPieces(QPainter& painter);
    void drawCoordinates(QPainter& painter);
    QPoint boardToPixel(int row, int col) const;
    std::pair<int, int> pixelToBoard(const QPoint& pos) const;
};