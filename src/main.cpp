// main.cpp
#include <QApplication>
#include "ChessGame.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ChessGame game;
    game.show();

    return app.exec();
}
