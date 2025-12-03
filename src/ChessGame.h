//
// Created by zhaoc_h on 2025/12/1.
//

#pragma once
#include "ChessLogic.h"
#include "ChessBoardWidget.h"
#include "ChessPiece.h"
#include <memory>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>

class ChessGame:public QMainWindow {
    Q_OBJECT
public:
    ChessGame(QWidget* parent = nullptr);
    ~ChessGame();

private slots:
    void startGoGame();
    void startGomokuGame();
    void returnToMainMenu();
    void exitGame();
    void onGameOver(PieceColor winner);
    
    // 新增槽函数
    void onPass();
    void onResign();
    void onUndo();
    void onDraw();
    void onGamePhaseChanged(GamePhase phase);
    void onScoreChanged(double blackScore, double whiteScore);
    void onTimeUpdated(int blackTime, int whiteTime);
    void onKoOccurred(int row, int col);
    void updateTimer();

private:
    void setupMainMenu();
    void setupGameInterface();
    void setupVictoryInterface();
    void setupScoringInterface();
    void updateGameInfo();
    void updateTimeDisplay();
    void updateScoreDisplay();
    QString formatTime(int seconds) const;
    QString getCoordinateString(int row, int col) const;
    
    QStackedWidget* m_stackedWidget;
    QTimer* m_timer;
    
    // 主菜单界面
    QWidget* m_menuWidget;
    QLabel* m_titleLabel;
    QPushButton* m_goButton;
    QPushButton* m_gomokuButton;
    QPushButton* m_exitButton;
    
    // 游戏界面
    QWidget* m_gameWidget;
    ChessBoardWidget* m_boardWidget;
    ChessLogic* m_gameLogic;
    QLabel* m_currentPlayerLabel;
    QLabel* m_moveCountLabel;
    QLabel* m_capturedLabel;
    QLabel* m_koLabel;
    
    // 控制按钮
    QPushButton* m_passButton;
    QPushButton* m_resignButton;
    QPushButton* m_undoButton;
    QPushButton* m_drawButton;
    QPushButton* m_returnMenuButton;
    
    // 计时显示
    QLabel* m_blackTimeLabel;
    QLabel* m_whiteTimeLabel;
    QLabel* m_blackByoYomiLabel;
    QLabel* m_whiteByoYomiLabel;
    
    // 胜利界面
    QWidget* m_victoryWidget;
    QLabel* m_victoryLabel;
    QPushButton* m_victoryReturnButton;
    
    // 计分界面
    QWidget* m_scoringWidget;
    QLabel* m_scoreLabel;
    QLabel* m_resultLabel;
    QPushButton* m_scoringReturnButton;
    
    GameMode m_currentMode;
    int m_moveCount;
};