/// ChessBoardWidget.cpp
#include "ChessBoardWidget.h"
#include "ChessLogic.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

ChessBoardWidget::ChessBoardWidget(ChessLogic* gameLogic, QWidget* parent)
    : QWidget(parent)
    , m_gameLogic(gameLogic)
    , m_boardSize(15) // 默认15x15棋盘（五子棋）
    , m_cellSize(30)
    , m_imagesLoaded(false)
{
    setMinimumSize((m_boardSize + 2) * m_cellSize, (m_boardSize + 2) * m_cellSize);
    setMouseTracking(true);
    // 延迟加载图片，在第一次绘制时加载
}

void ChessBoardWidget::setBoardSize(int size)
{
    m_boardSize = size;
    // 根据棋盘大小调整格子大小
    if (size == 19) {
        m_cellSize = 25; // 围棋棋盘格子稍小
    } else {
        m_cellSize = 30; // 五子棋棋盘格子
    }
    setMinimumSize((m_boardSize + 2) * m_cellSize, (m_boardSize + 2) * m_cellSize);
    // 重新加载图片以适应新的棋子大小
    m_imagesLoaded = false;
    update();
}

void ChessBoardWidget::ensureImagesLoaded()
{
    if (!m_imagesLoaded) {
        loadPieceImages();
        m_imagesLoaded = true;
    }
}

void ChessBoardWidget::loadPieceImages()
{
    // 创建黑色棋子
    int pieceSize = m_cellSize - 4;
    m_blackPiecePixmap = QPixmap(pieceSize, pieceSize);
    m_blackPiecePixmap.fill(Qt::transparent);
    QPainter blackPainter(&m_blackPiecePixmap);
    blackPainter.setRenderHint(QPainter::Antialiasing);
    blackPainter.setBrush(Qt::black);
    blackPainter.setPen(Qt::NoPen);
    blackPainter.drawEllipse(2, 2, pieceSize-4, pieceSize-4);
    blackPainter.end();
    
    // 创建白色棋子
    m_whitePiecePixmap = QPixmap(pieceSize, pieceSize);
    m_whitePiecePixmap.fill(Qt::transparent);
    QPainter whitePainter(&m_whitePiecePixmap);
    whitePainter.setRenderHint(QPainter::Antialiasing);
    whitePainter.setBrush(Qt::white);
    whitePainter.setPen(Qt::black);
    whitePainter.drawEllipse(2, 2, pieceSize-4, pieceSize-4);
    whitePainter.end();
}

void ChessBoardWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 确保图片已加载
    ensureImagesLoaded();

    // 绘制棋盘背景
    painter.fillRect(rect(), QColor(220, 179, 92)); // 木质黄色背景

    drawBoard(painter);
    drawCoordinates(painter);
    drawPieces(painter);
}

// 修正后的drawBoard函数
void ChessBoardWidget::drawBoard(QPainter& painter)
{
    painter.setPen(QPen(Qt::black, 1));
    int boardOffset = m_cellSize + m_cellSize/2;

    for (int i = 0; i < m_boardSize; ++i) {
        // 水平线
        painter.drawLine(boardOffset, boardOffset + i * m_cellSize,
                        boardOffset + (m_boardSize - 1) * m_cellSize, boardOffset + i * m_cellSize);
        // 垂直线
        painter.drawLine(boardOffset + i * m_cellSize, boardOffset,
                        boardOffset + i * m_cellSize, boardOffset + (m_boardSize - 1) * m_cellSize);
    }

    // 绘制棋盘上的星位
    painter.setBrush(Qt::black);
    if (m_boardSize == 15) {
        // 五子棋星位
        int starPoints[] = {3, 7, 11};
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                QPoint center = boardToPixel(starPoints[i], starPoints[j]);
                painter.drawEllipse(center, 3, 3);
            }
        }
    } else if (m_boardSize == 19) {
        // 围棋星位
        int starPoints[] = {3, 9, 15};
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                // 跳过天元以外的位置
                if ((i == 1 && j == 1) || (i == 0 && j == 0) || 
                    (i == 0 && j == 2) || (i == 2 && j == 0) || (i == 2 && j == 2)) {
                    QPoint center = boardToPixel(starPoints[i], starPoints[j]);
                    painter.drawEllipse(center, 3, 3);
                }
            }
        }
    }
}

void ChessBoardWidget::drawCoordinates(QPainter& painter)
{
    painter.setPen(QPen(Qt::black, 1));
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    
    int boardOffset = m_cellSize + m_cellSize/2;
    
    // 绘制字母坐标 (A, B, C...)
    for (int i = 0; i < m_boardSize; ++i) {
        QString letter = QChar('A' + i);
        int x = boardOffset + i * m_cellSize;
        painter.drawText(x - 5, boardOffset - 10, letter);
        painter.drawText(x - 5, boardOffset + (m_boardSize - 1) * m_cellSize + 20, letter);
    }
    
    // 绘制数字坐标 (1, 2, 3...)
    for (int i = 0; i < m_boardSize; ++i) {
        QString number = QString::number(m_boardSize - i);
        int y = boardOffset + i * m_cellSize;
        painter.drawText(boardOffset - 20, y + 5, number);
        painter.drawText(boardOffset + (m_boardSize - 1) * m_cellSize + 10, y + 5, number);
    }
}

void ChessBoardWidget::drawPieces(QPainter& painter)
{
    painter.setRenderHint(QPainter::Antialiasing);
    
    for (int row = 0; row < m_boardSize; ++row) {
        for (int col = 0; col < m_boardSize; ++col) {
            PieceColor piece = m_gameLogic->getPieceAt(row, col);
            if (piece != PieceColor::Empty) {
                QPoint center = boardToPixel(row, col);
                int pieceSize = m_cellSize - 4;
                
                if (piece == PieceColor::Black) {
                    painter.drawPixmap(center.x() - pieceSize/2, center.y() - pieceSize/2,
                                     m_blackPiecePixmap);
                } else if (piece == PieceColor::White) {
                    painter.drawPixmap(center.x() - pieceSize/2, center.y() - pieceSize/2,
                                     m_whitePiecePixmap);
                }
            }
        }
    }
}

void ChessBoardWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        auto [row, col] = pixelToBoard(event->pos());
        if (row >= 0 && row < m_boardSize && col >= 0 && col < m_boardSize) {
            emit positionClicked(row, col);
        }
    }
}

QPoint ChessBoardWidget::boardToPixel(int row, int col) const
{
    int boardOffset = m_cellSize + m_cellSize/2;
    int x = boardOffset + col * m_cellSize;
    int y = boardOffset + row * m_cellSize;
    return QPoint(x, y);
}

std::pair<int, int> ChessBoardWidget::pixelToBoard(const QPoint& pos) const
{
    int boardOffset = m_cellSize + m_cellSize/2;
    int col = (pos.x() - boardOffset + m_cellSize/2) / m_cellSize;
    int row = (pos.y() - boardOffset + m_cellSize/2) / m_cellSize;
    return std::make_pair(row, col);
}