//
// Created by zhaoc_h on 2025/12/1.
//

#include "ChessGame.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QFont>
#include <QMessageBox>
#include <QApplication>
#include "ChessBoardWidget.h"
#include "ChessLogic.h"

ChessGame::ChessGame(QWidget* parent)
    : QMainWindow(parent)
    , m_currentMode(GameMode::None)
    , m_moveCount(0)
{
    setWindowTitle("棋类");
    resize(1000, 800);
    
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);
    
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ChessGame::updateTimer);
    m_timer->start(1000); // 每秒更新一次
    
    setupMainMenu();
    setupGameInterface();
    
    m_stackedWidget->setCurrentWidget(m_menuWidget);
}

ChessGame::~ChessGame() = default;

void ChessGame::setupMainMenu()
{
    m_menuWidget = new QWidget();
    QVBoxLayout* menuLayout = new QVBoxLayout(m_menuWidget);
    
    // 标题
    m_titleLabel = new QLabel("棋类");
    QFont titleFont;
    titleFont.setPointSize(32);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("color: #2c3e50; margin: 50px;");
    
    // 按钮
    m_goButton = new QPushButton("围棋");
    m_gomokuButton = new QPushButton("五子棋");
    m_exitButton = new QPushButton("退出游戏");
    
    QFont buttonFont;
    buttonFont.setPointSize(16);
    m_goButton->setFont(buttonFont);
    m_gomokuButton->setFont(buttonFont);
    m_exitButton->setFont(buttonFont);
    
    QString buttonStyle = "QPushButton { "
                         "background-color: #3498db; "
                         "color: white; "
                         "border: none; "
                         "padding: 15px 30px; "
                         "border-radius: 8px; "
                         "margin: 10px; "
                         "} "
                         "QPushButton:hover { "
                         "background-color: #2980b9; "
                         "}";
    
    m_goButton->setStyleSheet(buttonStyle);
    m_gomokuButton->setStyleSheet(buttonStyle);
    m_exitButton->setStyleSheet(buttonStyle);
    
    // 布局
    menuLayout->addStretch();
    menuLayout->addWidget(m_titleLabel);
    menuLayout->addStretch();
    menuLayout->addWidget(m_goButton);
    menuLayout->addWidget(m_gomokuButton);
    menuLayout->addWidget(m_exitButton);
    menuLayout->addStretch();
    
    // 连接信号
    connect(m_goButton, &QPushButton::clicked, this, &ChessGame::startGoGame);
    connect(m_gomokuButton, &QPushButton::clicked, this, &ChessGame::startGomokuGame);
    connect(m_exitButton, &QPushButton::clicked, this, &ChessGame::exitGame);
    
    m_stackedWidget->addWidget(m_menuWidget);
}

void ChessGame::setupGameInterface()
{
    m_gameWidget = new QWidget();
    QVBoxLayout* gameLayout = new QVBoxLayout(m_gameWidget);
    
    // 顶部信息栏
    QHBoxLayout* infoLayout = new QHBoxLayout();
    m_currentPlayerLabel = new QLabel("当前出手方：黑方");
    m_moveCountLabel = new QLabel("棋数：0");
    m_capturedLabel = new QLabel("提子：黑0 白0");
    m_koLabel = new QLabel("");
    m_returnMenuButton = new QPushButton("返回主菜单");
    
    QFont infoFont;
    infoFont.setPointSize(12);
    m_currentPlayerLabel->setFont(infoFont);
    m_moveCountLabel->setFont(infoFont);
    m_capturedLabel->setFont(infoFont);
    m_koLabel->setFont(infoFont);
    m_returnMenuButton->setFont(infoFont);
    
    m_currentPlayerLabel->setStyleSheet("color: #2c3e50; padding: 5px;");
    m_moveCountLabel->setStyleSheet("color: #2c3e50; padding: 5px;");
    m_capturedLabel->setStyleSheet("color: #2c3e50; padding: 5px;");
    m_koLabel->setStyleSheet("color: #e74c3c; padding: 5px; font-weight: bold;");
    m_returnMenuButton->setStyleSheet("QPushButton { "
                                     "background-color: #e74c3c; "
                                     "color: white; "
                                     "border: none; "
                                     "padding: 8px 16px; "
                                     "border-radius: 5px; "
                                     "} "
                                     "QPushButton:hover { "
                                     "background-color: #c0392b; "
                                     "}");
    
    infoLayout->addWidget(m_currentPlayerLabel);
    infoLayout->addWidget(m_moveCountLabel);
    infoLayout->addWidget(m_capturedLabel);
    infoLayout->addWidget(m_koLabel);
    infoLayout->addStretch();
    infoLayout->addWidget(m_returnMenuButton);
    
    // 计时显示
    QHBoxLayout* timeLayout = new QHBoxLayout();
    m_blackTimeLabel = new QLabel("黑方: 30:00");
    m_whiteTimeLabel = new QLabel("白方: 30:00");
    m_blackByoYomiLabel = new QLabel("");
    m_whiteByoYomiLabel = new QLabel("");
    
    QFont timeFont;
    timeFont.setPointSize(14);
    timeFont.setBold(true);
    m_blackTimeLabel->setFont(timeFont);
    m_whiteTimeLabel->setFont(timeFont);
    m_blackByoYomiLabel->setFont(infoFont);
    m_whiteByoYomiLabel->setFont(infoFont);
    
    m_blackTimeLabel->setStyleSheet("color: #2c3e50; padding: 5px;");
    m_whiteTimeLabel->setStyleSheet("color: #2c3e50; padding: 5px;");
    m_blackByoYomiLabel->setStyleSheet("color: #e67e22; padding: 5px;");
    m_whiteByoYomiLabel->setStyleSheet("color: #e67e22; padding: 5px;");
    
    timeLayout->addWidget(m_blackTimeLabel);
    timeLayout->addWidget(m_blackByoYomiLabel);
    timeLayout->addStretch();
    timeLayout->addWidget(m_whiteByoYomiLabel);
    timeLayout->addWidget(m_whiteTimeLabel);
    
    // 棋盘和控制按钮
    QHBoxLayout* middleLayout = new QHBoxLayout();
    
    // 左侧控制按钮
    QVBoxLayout* controlLayout = new QVBoxLayout();
    m_passButton = new QPushButton("虚着");
    m_resignButton = new QPushButton("认输");
    m_undoButton = new QPushButton("悔棋");
    m_drawButton = new QPushButton("和棋");
    
    QFont controlFont;
    controlFont.setPointSize(12);
    m_passButton->setFont(controlFont);
    m_resignButton->setFont(controlFont);
    m_undoButton->setFont(controlFont);
    m_drawButton->setFont(controlFont);
    
    QString controlStyle = "QPushButton { "
                          "background-color: #3498db; "
                          "color: white; "
                          "border: none; "
                          "padding: 10px 20px; "
                          "border-radius: 5px; "
                          "margin: 5px; "
                          "} "
                          "QPushButton:hover { "
                          "background-color: #2980b9; "
                          "} "
                          "QPushButton:disabled { "
                          "background-color: #bdc3c7; "
                          "}";
    
    m_passButton->setStyleSheet(controlStyle);
    m_resignButton->setStyleSheet("QPushButton { "
                                 "background-color: #e74c3c; "
                                 "color: white; "
                                 "border: none; "
                                 "padding: 10px 20px; "
                                 "border-radius: 5px; "
                                 "margin: 5px; "
                                 "} "
                                 "QPushButton:hover { "
                                 "background-color: #c0392b; "
                                 "}");
    m_undoButton->setStyleSheet(controlStyle);
    m_drawButton->setStyleSheet(controlStyle);
    
    controlLayout->addWidget(m_passButton);
    controlLayout->addWidget(m_resignButton);
    controlLayout->addWidget(m_undoButton);
    controlLayout->addWidget(m_drawButton);
    controlLayout->addStretch();
    
    // 棋盘
    m_gameLogic = new ChessLogic(this);
    m_boardWidget = new ChessBoardWidget(m_gameLogic, this);
    
    middleLayout->addLayout(controlLayout);
    middleLayout->addWidget(m_boardWidget);
    
    gameLayout->addLayout(infoLayout);
    gameLayout->addLayout(timeLayout);
    gameLayout->addLayout(middleLayout);
    
    // 连接信号
    connect(m_returnMenuButton, &QPushButton::clicked, this, &ChessGame::returnToMainMenu);
    connect(m_passButton, &QPushButton::clicked, this, &ChessGame::onPass);
    connect(m_resignButton, &QPushButton::clicked, this, &ChessGame::onResign);
    connect(m_undoButton, &QPushButton::clicked, this, &ChessGame::onUndo);
    connect(m_drawButton, &QPushButton::clicked, this, &ChessGame::onDraw);
    
    connect(m_boardWidget, &ChessBoardWidget::positionClicked,
            m_gameLogic, &ChessLogic::handleClick);
    connect(m_gameLogic, &ChessLogic::boardUpdated, this, &ChessGame::updateGameInfo);
    connect(m_gameLogic, &ChessLogic::boardUpdated, m_boardWidget, static_cast<void(QWidget::*)()>(&QWidget::update));
    connect(m_gameLogic, &ChessLogic::gameOver, this, &ChessGame::onGameOver);
    connect(m_gameLogic, &ChessLogic::gamePhaseChanged, this, &ChessGame::onGamePhaseChanged);
    connect(m_gameLogic, &ChessLogic::scoreChanged, this, &ChessGame::onScoreChanged);
    connect(m_gameLogic, &ChessLogic::timeUpdated, this, &ChessGame::onTimeUpdated);
    connect(m_gameLogic, &ChessLogic::koOccurred, this, &ChessGame::onKoOccurred);
    
    m_stackedWidget->addWidget(m_gameWidget);
    
    // 胜利界面
    setupVictoryInterface();
    
    // 计分界面
    setupScoringInterface();
}

void ChessGame::setupVictoryInterface()
{
    m_victoryWidget = new QWidget();
    QVBoxLayout* victoryLayout = new QVBoxLayout(m_victoryWidget);
    
    m_victoryLabel = new QLabel("黑方获胜！");
    QFont victoryFont;
    victoryFont.setPointSize(24);
    victoryFont.setBold(true);
    m_victoryLabel->setFont(victoryFont);
    m_victoryLabel->setAlignment(Qt::AlignCenter);
    m_victoryLabel->setStyleSheet("color: #27ae60; margin: 50px;");
    
    m_victoryReturnButton = new QPushButton("返回主菜单");
    QFont returnFont;
    returnFont.setPointSize(16);
    m_victoryReturnButton->setFont(returnFont);
    m_victoryReturnButton->setStyleSheet("QPushButton { "
                                        "background-color: #3498db; "
                                        "color: white; "
                                        "border: none; "
                                        "padding: 15px 30px; "
                                        "border-radius: 8px; "
                                        "margin: 10px; "
                                        "} "
                                        "QPushButton:hover { "
                                        "background-color: #2980b9; "
                                        "}");
    
    victoryLayout->addStretch();
    victoryLayout->addWidget(m_victoryLabel);
    victoryLayout->addWidget(m_victoryReturnButton);
    victoryLayout->addStretch();
    
    connect(m_victoryReturnButton, &QPushButton::clicked, this, &ChessGame::returnToMainMenu);
    
    m_stackedWidget->addWidget(m_victoryWidget);
}

void ChessGame::startGoGame()
{
    m_currentMode = GameMode::Go;
    m_moveCount = 0;
    m_gameLogic->resetGame();
    m_gameLogic->setGameMode(GameMode::Go);
    m_boardWidget->setBoardSize(19); // 围棋使用19x19棋盘
    m_gameLogic->startTimer();
    updateGameInfo();
    updateTimeDisplay();
    
    // 显示围棋相关控件
    m_passButton->setVisible(true);
    m_capturedLabel->setVisible(true);
    m_koLabel->setVisible(true);
    m_blackTimeLabel->setVisible(true);
    m_whiteTimeLabel->setVisible(true);
    m_blackByoYomiLabel->setVisible(true);
    m_whiteByoYomiLabel->setVisible(true);
    
    m_stackedWidget->setCurrentWidget(m_gameWidget);
}

void ChessGame::startGomokuGame()
{
    m_currentMode = GameMode::Gomoku;
    m_moveCount = 0;
    m_gameLogic->resetGame();
    m_gameLogic->setGameMode(GameMode::Gomoku);
    m_boardWidget->setBoardSize(15); // 五子棋使用15x15棋盘
    updateGameInfo();
    
    // 隐藏围棋相关控件
    m_passButton->setVisible(false);
    m_capturedLabel->setVisible(false);
    m_koLabel->setVisible(false);
    m_blackTimeLabel->setVisible(false);
    m_whiteTimeLabel->setVisible(false);
    m_blackByoYomiLabel->setVisible(false);
    m_whiteByoYomiLabel->setVisible(false);
    
    m_stackedWidget->setCurrentWidget(m_gameWidget);
}

void ChessGame::returnToMainMenu()
{
    m_stackedWidget->setCurrentWidget(m_menuWidget);
    m_currentMode = GameMode::None;
}

void ChessGame::exitGame()
{
    QApplication::quit();
}

void ChessGame::onGameOver(PieceColor winner)
{
    QString winnerText = (winner == PieceColor::Black) ? "黑方" : "白方";
    m_victoryLabel->setText(winnerText + "获胜！");
    m_stackedWidget->setCurrentWidget(m_victoryWidget);
}

void ChessGame::setupScoringInterface()
{
    m_scoringWidget = new QWidget();
    QVBoxLayout* scoringLayout = new QVBoxLayout(m_scoringWidget);
    
    m_scoreLabel = new QLabel("黑方: 0.0 目\n白方: 0.0 目\n贴目: 6.5 目");
    QFont scoreFont;
    scoreFont.setPointSize(18);
    scoreFont.setBold(true);
    m_scoreLabel->setFont(scoreFont);
    m_scoreLabel->setAlignment(Qt::AlignCenter);
    m_scoreLabel->setStyleSheet("color: #2c3e50; margin: 50px;");
    
    m_resultLabel = new QLabel("黑方获胜！");
    QFont resultFont;
    resultFont.setPointSize(24);
    resultFont.setBold(true);
    m_resultLabel->setFont(resultFont);
    m_resultLabel->setAlignment(Qt::AlignCenter);
    m_resultLabel->setStyleSheet("color: #27ae60; margin: 20px;");
    
    m_scoringReturnButton = new QPushButton("返回主菜单");
    QFont returnFont;
    returnFont.setPointSize(16);
    m_scoringReturnButton->setFont(returnFont);
    m_scoringReturnButton->setStyleSheet("QPushButton { "
                                        "background-color: #3498db; "
                                        "color: white; "
                                        "border: none; "
                                        "padding: 15px 30px; "
                                        "border-radius: 8px; "
                                        "margin: 10px; "
                                        "} "
                                        "QPushButton:hover { "
                                        "background-color: #2980b9; "
                                        "}");
    
    scoringLayout->addStretch();
    scoringLayout->addWidget(m_scoreLabel);
    scoringLayout->addWidget(m_resultLabel);
    scoringLayout->addWidget(m_scoringReturnButton);
    scoringLayout->addStretch();
    
    connect(m_scoringReturnButton, &QPushButton::clicked, this, &ChessGame::returnToMainMenu);
    
    m_stackedWidget->addWidget(m_scoringWidget);
}

void ChessGame::updateGameInfo()
{
    QString currentPlayerText = (m_gameLogic->getCurrentPlayer() == PieceColor::Black) ? "黑方" : "白方";
    m_currentPlayerLabel->setText("当前出手方：" + currentPlayerText);
    m_moveCountLabel->setText("棋数：" + QString::number(m_moveCount));
    
    // 更新提子数
    if (m_currentMode == GameMode::Go) {
        m_capturedLabel->setText(QString("提子：黑%1 白%2")
                                .arg(m_gameLogic->getCapturedBlack())
                                .arg(m_gameLogic->getCapturedWhite()));
    }
    
    // 更新悔棋按钮状态
    m_undoButton->setEnabled(m_gameLogic->canUndo());
    
    m_moveCount++;
}

// 新增槽函数实现
void ChessGame::onPass()
{
    if (m_currentMode == GameMode::Go) {
        m_gameLogic->pass();
    }
}

void ChessGame::onResign()
{
    m_gameLogic->resign();
}

void ChessGame::onUndo()
{
    m_gameLogic->undo();
    m_moveCount = std::max(0, m_moveCount - 1);
    updateGameInfo();
}

void ChessGame::onDraw()
{
    m_gameLogic->requestDraw();
}

void ChessGame::onGamePhaseChanged(GamePhase phase)
{
    if (phase == GamePhase::Scoring) {
        m_stackedWidget->setCurrentWidget(m_scoringWidget);
    }
}

void ChessGame::onScoreChanged(double blackScore, double whiteScore)
{
    GameSettings settings = m_gameLogic->getGameSettings();
    m_scoreLabel->setText(QString("黑方: %1 目\n白方: %2 目\n贴目: %3 目")
                         .arg(blackScore, 0, 'f', 1)
                         .arg(whiteScore, 0, 'f', 1)
                         .arg(settings.komi, 0, 'f', 1));
    
    GameResult result = m_gameLogic->getGameResult();
    if (result == GameResult::BlackWin) {
        m_resultLabel->setText("黑方获胜！");
        m_resultLabel->setStyleSheet("color: #2c3e50; margin: 20px;");
    } else if (result == GameResult::WhiteWin) {
        m_resultLabel->setText("白方获胜！");
        m_resultLabel->setStyleSheet("color: #7f8c8d; margin: 20px;");
    } else {
        m_resultLabel->setText("和棋！");
        m_resultLabel->setStyleSheet("color: #f39c12; margin: 20px;");
    }
}

void ChessGame::onTimeUpdated(int blackTime, int whiteTime)
{
    m_blackTimeLabel->setText("黑方: " + formatTime(blackTime));
    m_whiteTimeLabel->setText("白方: " + formatTime(whiteTime));
    
    // 更新读秒显示
    if (m_gameLogic->isInByoYomi(PieceColor::Black)) {
        m_blackByoYomiLabel->setText(QString("读秒 %1次").arg(m_gameLogic->getByoYomiPeriods(PieceColor::Black)));
    } else {
        m_blackByoYomiLabel->setText("");
    }
    
    if (m_gameLogic->isInByoYomi(PieceColor::White)) {
        m_whiteByoYomiLabel->setText(QString("读秒 %1次").arg(m_gameLogic->getByoYomiPeriods(PieceColor::White)));
    } else {
        m_whiteByoYomiLabel->setText("");
    }
}

void ChessGame::onKoOccurred(int row, int col)
{
    m_koLabel->setText(QString("劫争: %1").arg(getCoordinateString(row, col)));
}

void ChessGame::updateTimer()
{
    if (m_currentMode == GameMode::Go && m_gameLogic) {
        m_gameLogic->updateTimer();
    }
}

QString ChessGame::formatTime(int seconds) const
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}

QString ChessGame::getCoordinateString(int row, int col) const
{
    QString colChar = QChar('A' + col);
    QString rowNum = QString::number(19 - row); // 围棋坐标从下往上
    return colChar + rowNum;
}

void ChessGame::updateTimeDisplay()
{
    if (m_currentMode == GameMode::Go) {
        GameSettings settings = m_gameLogic->getGameSettings();
        m_blackTimeLabel->setText("黑方: " + formatTime(settings.mainTime));
        m_whiteTimeLabel->setText("白方: " + formatTime(settings.mainTime));
    }
}

void ChessGame::updateScoreDisplay()
{
    if (m_gameLogic->getGamePhase() == GamePhase::Scoring) {
        double blackScore = m_gameLogic->getBlackScore();
        double whiteScore = m_gameLogic->getWhiteScore();
        GameSettings settings = m_gameLogic->getGameSettings();
        
        m_scoreLabel->setText(QString("黑方: %1 目\n白方: %2 目\n贴目: %3 目")
                             .arg(blackScore, 0, 'f', 1)
                             .arg(whiteScore, 0, 'f', 1)
                             .arg(settings.komi, 0, 'f', 1));
    }
}

