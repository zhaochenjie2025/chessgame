//
// Created by zhaoc_h on 2025/12/1.
//

#pragma once

#include <vector>

enum class PieceColor {
    Empty,
    Black,
    White
};

enum class GameMode {
    None,
    Go,
    Gomoku
};

enum class GameResult {
    None,
    BlackWin,
    WhiteWin,
    Draw,
    Resign,
    Timeout
};

enum class GamePhase {
    Playing,
    Scoring,
    Finished
};

struct ChessPiece {
    int row;
    int col;
    PieceColor color;

    ChessPiece():row(0),col(0),color(PieceColor::Empty){}
    ChessPiece(int r, int c, PieceColor pc) : row(r), col(c), color(pc){}
};

struct Move {
    int row;
    int col;
    PieceColor player;
    std::vector<ChessPiece> capturedPieces; // 被提的棋子
    
    Move() : row(-1), col(-1), player(PieceColor::Empty) {}
    Move(int r, int c, PieceColor p) : row(r), col(c), player(p) {}
};

struct KoPoint {
    int row;
    int col;
    PieceColor koColor; // 劫的颜色（被提子的颜色）
    int moveNumber; // 发生劫的步数
    
    KoPoint() : row(-1), col(-1), koColor(PieceColor::Empty), moveNumber(-1) {}
    KoPoint(int r, int c, PieceColor color, int moveNum) 
        : row(r), col(c), koColor(color), moveNumber(moveNum) {}
};

struct GameSettings {
    double komi; // 贴目
    int mainTime; // 主时间（秒）
    int byoYomiTime; // 读秒时间（秒）
    int byoYomiPeriods; // 读秒次数
    
    GameSettings() 
        : komi(6.5), mainTime(1800), byoYomiTime(30), byoYomiPeriods(3) {}
};