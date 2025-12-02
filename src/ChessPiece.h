//
// Created by zhaoc_h on 2025/12/1.
//

#pragma once

enum class PieceColor {
    Empty,
    Black,
    White
};

struct ChessPiece {
    int row;
    int col;
    PieceColor color;

    ChessPiece():row(0),col(0),color(PieceColor::Empty){}
    ChessPiece(int r, int c, PieceColor pc) : row(r), col(c), color(pc){}
};