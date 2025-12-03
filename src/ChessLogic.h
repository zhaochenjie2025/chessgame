//
// Created by zhaoc_h on 2025/12/1.
//

#pragma once

#include <QObject>
#include <vector>
#include <stack>
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
    int getCapturedBlack() const { return m_capturedBlack; }
    int getCapturedWhite() const { return m_capturedWhite; }
    
    void setGameMode(GameMode mode);
    void resetGame();
    
    // 新增功能
    void pass(); // 虚着
    void resign(); // 认输
    void undo(); // 悔棋
    bool canUndo() const;
    void requestDraw(); // 请求和棋
    
    // 劫相关
    bool isKoPoint(int row, int col) const;
    KoPoint getCurrentKo() const { return m_currentKo; }
    
    // 终局相关
    GamePhase getGamePhase() const { return m_gamePhase; }
    void enterScoringPhase();
    void calculateScore();
    GameResult getGameResult() const { return m_gameResult; }
    double getBlackScore() const { return m_blackScore; }
    double getWhiteScore() const { return m_whiteScore; }
    
    // 设置
    void setGameSettings(const GameSettings& settings) { m_settings = settings; }
    GameSettings getGameSettings() const { return m_settings; }
    
    // 计时相关
    void startTimer();
    void pauseTimer();
    void resumeTimer();
    void updateTimer();
    int getBlackTime() const { return m_blackTime; }
    int getWhiteTime() const { return m_whiteTime; }
    bool isInByoYomi(PieceColor color) const;
    int getByoYomiPeriods(PieceColor color) const;

    signals:
        void boardUpdated();
        void gameOver(PieceColor winner);
        void gamePhaseChanged(GamePhase phase);
        void scoreChanged(double blackScore, double whiteScore);
        void timeUpdated(int blackTime, int whiteTime);
        void koOccurred(int row, int col);

private:
    static const int BOARD_SIZE = 19; // 围棋使用19x19棋盘
    PieceColor m_board[BOARD_SIZE][BOARD_SIZE];
    PieceColor m_currentPlayer;
    bool m_gameOver;
    GameMode m_gameMode;
    GamePhase m_gamePhase;
    GameResult m_gameResult;
    
    // 提子数
    int m_capturedBlack; // 被提的黑子数
    int m_capturedWhite; // 被提的白子数
    
    // 游戏记录
    std::vector<Move> m_moveHistory;
    int m_moveCount;
    int m_consecutivePasses; // 连续虚着次数
    
    // 劫相关
    KoPoint m_currentKo;
    std::vector<KoPoint> m_koHistory;
    
    // 计分
    double m_blackScore;
    double m_whiteScore;
    
    // 设置
    GameSettings m_settings;
    
    // 计时
    int m_blackTime;
    int m_whiteTime;
    int m_blackByoYomiPeriods;
    int m_whiteByoYomiPeriods;
    bool m_timerActive;

    bool checkWin(int row, int col);
    bool checkGomokuWin(int row, int col);
    bool checkGoWin(int row, int col);
    
    // 围棋相关方法
    void captureStones(int row, int col);
    bool hasLiberties(int row, int col, PieceColor color);
    bool hasLibertiesHelper(int row, int col, PieceColor color, bool visited[][BOARD_SIZE]);
    void findGroup(int row, int col, PieceColor color, std::vector<std::pair<int, int>>& group);
    void removeGroup(const std::vector<std::pair<int, int>>& group);
    bool isSuicide(int row, int col, PieceColor color);
    bool wouldBeSuicide(int row, int col, PieceColor color) const;
    
    // 劫相关方法
    void checkAndSetKo(int row, int col);
    bool isKoViolation(int row, int col) const;
    
    // 终局计算
    void countTerritory();
    void markDeadStones();
    bool isGroupAlive(int row, int col, PieceColor color);
    
    void switchPlayer();
};