//
// Created by zhaoc_h on 2025/12/1.
//

// ChessLogic.cpp
#include "ChessLogic.h"
#include <QTimer>

ChessLogic::ChessLogic(QObject* parent)
    : QObject(parent)
    , m_currentPlayer(PieceColor::Black)
    , m_gameOver(false)
    , m_gameMode(GameMode::Gomoku) // 默认五子棋
    , m_gamePhase(GamePhase::Playing)
    , m_gameResult(GameResult::None)
    , m_capturedBlack(0)
    , m_capturedWhite(0)
    , m_moveCount(0)
    , m_consecutivePasses(0)
    , m_blackScore(0.0)
    , m_whiteScore(0.0)
    , m_blackTime(0)
    , m_whiteTime(0)
    , m_blackByoYomiPeriods(m_settings.byoYomiPeriods)
    , m_whiteByoYomiPeriods(m_settings.byoYomiPeriods)
    , m_timerActive(false)
{
    // 初始化棋盘
    resetGame();
}

void ChessLogic::setGameMode(GameMode mode)
{
    m_gameMode = mode;
    resetGame();
}

void ChessLogic::resetGame()
{
    m_currentPlayer = PieceColor::Black;
    m_gameOver = false;
    m_gamePhase = GamePhase::Playing;
    m_gameResult = GameResult::None;
    m_capturedBlack = 0;
    m_capturedWhite = 0;
    m_moveCount = 0;
    m_consecutivePasses = 0;
    m_blackScore = 0.0;
    m_whiteScore = 0.0;
    m_blackTime = m_settings.mainTime;
    m_whiteTime = m_settings.mainTime;
    m_blackByoYomiPeriods = m_settings.byoYomiPeriods;
    m_whiteByoYomiPeriods = m_settings.byoYomiPeriods;
    m_timerActive = false;
    
    m_moveHistory.clear();
    m_koHistory.clear();
    m_currentKo = KoPoint();
    
    // 初始化棋盘
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            m_board[i][j] = PieceColor::Empty;
        }
    }
}

void ChessLogic::handleClick(int row, int col)
{
    if (m_gameOver || m_gamePhase != GamePhase::Playing || !isValidMove(row, col)) {
        return;
    }

    // 重置连续虚着计数
    m_consecutivePasses = 0;
    
    // 记录移动
    Move move(row, col, m_currentPlayer);
    
    placePiece(row, col);
    
    // 围棋模式：提子
    if (m_gameMode == GameMode::Go) {
        captureStones(row, col);
        checkAndSetKo(row, col);
    }
    
    // 添加到历史记录
    m_moveHistory.push_back(move);
    m_moveCount++;

    if (checkWin(row, col)) {
        m_gameOver = true;
        m_gamePhase = GamePhase::Finished;
        emit gameOver(m_currentPlayer);
    } else {
        switchPlayer();
        emit boardUpdated();
    }
}

bool ChessLogic::isValidMove(int row, int col) const
{
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE ||
        m_board[row][col] != PieceColor::Empty) {
        return false;
    }
    
    // 围棋模式：检查劫和自杀手
    if (m_gameMode == GameMode::Go) {
        if (isKoViolation(row, col)) {
            return false; // 劫争违规
        }
        return !wouldBeSuicide(row, col, m_currentPlayer);
    }
    
    return true;
}

void ChessLogic::placePiece(int row, int col)
{
    m_board[row][col] = m_currentPlayer;
}

bool ChessLogic::checkWin(int row, int col)
{
    if (m_gameMode == GameMode::Gomoku) {
        return checkGomokuWin(row, col);
    } else if (m_gameMode == GameMode::Go) {
        return checkGoWin(row, col);
    }
    return false;
}

bool ChessLogic::checkGomokuWin(int row, int col)
{
    PieceColor player = m_board[row][col];
    if (player == PieceColor::Empty) return false;

    // 检查四个方向是否有连续5个棋子
    int directions[4][2] = {{0,1}, {1,0}, {1,1}, {1,-1}};

    for (auto& dir : directions) {
        int count = 1; // 包含当前棋子

        // 正向检查
        for (int i = 1; i <= 4; ++i) {
            int r = row + dir[0] * i;
            int c = col + dir[1] * i;
            if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE &&
                m_board[r][c] == player) {
                count++;
            } else {
                break;
            }
        }

        // 反向检查
        for (int i = 1; i <= 4; ++i) {
            int r = row - dir[0] * i;
            int c = col - dir[1] * i;
            if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE &&
                m_board[r][c] == player) {
                count++;
            } else {
                break;
            }
        }

        if (count >= 5) {
            return true;
        }
    }

    return false;
}

bool ChessLogic::checkGoWin(int row, int col)
{
    // 围棋的胜利不是通过提子数判定，而是通过终局数子
    // 这里只检查是否进入终局阶段，实际胜负在calculateScore中确定
    return false; // 围棋模式下不通过checkWin判定胜负
}

// 围棋相关方法实现
void ChessLogic::captureStones(int row, int col)
{
    if (m_moveHistory.empty()) return;
    
    PieceColor opponent = (m_currentPlayer == PieceColor::Black) ? 
                         PieceColor::White : PieceColor::Black;
    
    // 检查四个方向的对手棋子
    int directions[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
    
    for (auto& dir : directions) {
        int r = row + dir[0];
        int c = col + dir[1];
        
        if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE &&
            m_board[r][c] == opponent) {
            
            std::vector<std::pair<int, int>> group;
            findGroup(r, c, opponent, group);
            
            if (!hasLiberties(r, c, opponent)) {
                // 记录被提的棋子到当前移动中
                for (auto& pos : group) {
                    m_moveHistory.back().capturedPieces.push_back(
                        ChessPiece(pos.first, pos.second, opponent));
                }
                
                removeGroup(group);
                if (opponent == PieceColor::Black) {
                    m_capturedBlack += group.size();
                } else {
                    m_capturedWhite += group.size();
                }
            }
        }
    }
}

bool ChessLogic::hasLiberties(int row, int col, PieceColor color)
{
    bool visited[BOARD_SIZE][BOARD_SIZE] = {false};
    return hasLibertiesHelper(row, col, color, visited);
}

bool ChessLogic::hasLibertiesHelper(int row, int col, PieceColor color, bool visited[][BOARD_SIZE])
{
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        return false;
    }
    
    if (visited[row][col]) {
        return false;
    }
    
    visited[row][col] = true;
    
    if (m_board[row][col] == PieceColor::Empty) {
        return true;
    }
    
    if (m_board[row][col] != color) {
        return false;
    }
    
    // 检查四个方向
    int directions[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
    for (auto& dir : directions) {
        int r = row + dir[0];
        int c = col + dir[1];
        if (hasLibertiesHelper(r, c, color, visited)) {
            return true;
        }
    }
    
    return false;
}

void ChessLogic::findGroup(int row, int col, PieceColor color, std::vector<std::pair<int, int>>& group)
{
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        return;
    }
    
    if (m_board[row][col] != color) {
        return;
    }
    
    // 检查是否已经在组中
    for (auto& pos : group) {
        if (pos.first == row && pos.second == col) {
            return;
        }
    }
    
    group.push_back({row, col});
    
    // 检查四个方向
    int directions[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
    for (auto& dir : directions) {
        findGroup(row + dir[0], col + dir[1], color, group);
    }
}

void ChessLogic::removeGroup(const std::vector<std::pair<int, int>>& group)
{
    for (auto& pos : group) {
        m_board[pos.first][pos.second] = PieceColor::Empty;
    }
}

bool ChessLogic::isSuicide(int row, int col, PieceColor color)
{
    // 临时放置棋子
    m_board[row][col] = color;
    
    // 检查是否有气
    bool hasLiberty = hasLiberties(row, col, color);
    
    // 恢复
    m_board[row][col] = PieceColor::Empty;
    
    return !hasLiberty;
}

bool ChessLogic::wouldBeSuicide(int row, int col, PieceColor color) const
{
    // 创建临时棋盘副本
    PieceColor tempBoard[BOARD_SIZE][BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            tempBoard[i][j] = m_board[i][j];
        }
    }
    
    // 临时放置棋子
    tempBoard[row][col] = color;
    
    // 检查是否有气（简化版本）
    int directions[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
    for (auto& dir : directions) {
        int r = row + dir[0];
        int c = col + dir[1];
        if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE &&
            tempBoard[r][c] == PieceColor::Empty) {
            return false; // 有气，不是自杀
        }
    }
    
    return true; // 无气，是自杀
}

PieceColor ChessLogic::getPieceAt(int row, int col) const
{
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
        return m_board[row][col];
    }
    return PieceColor::Empty;
}

// 新增功能实现
void ChessLogic::pass()
{
    if (m_gamePhase != GamePhase::Playing) return;
    
    m_consecutivePasses++;
    
    // 双方连续虚着则进入终局
    if (m_consecutivePasses >= 2) {
        enterScoringPhase();
    } else {
        switchPlayer();
        emit boardUpdated();
    }
}

void ChessLogic::resign()
{
    if (m_gamePhase != GamePhase::Playing) return;
    
    m_gameOver = true;
    m_gamePhase = GamePhase::Finished;
    m_gameResult = (m_currentPlayer == PieceColor::Black) ? GameResult::WhiteWin : GameResult::BlackWin;
    emit gameOver((m_currentPlayer == PieceColor::Black) ? PieceColor::White : PieceColor::Black);
}

void ChessLogic::undo()
{
    if (m_moveHistory.empty() || m_gamePhase != GamePhase::Playing) return;
    
    // 撤销上一步
    Move lastMove = m_moveHistory.back();
    m_moveHistory.pop_back();
    m_moveCount--;
    
    // 恢复棋盘状态
    m_board[lastMove.row][lastMove.col] = PieceColor::Empty;
    
    // 恢复被提的棋子
    for (const auto& piece : lastMove.capturedPieces) {
        m_board[piece.row][piece.col] = piece.color;
    }
    
    // 恢复玩家
    m_currentPlayer = lastMove.player;
    
    // 恢复劫状态
    if (!m_koHistory.empty()) {
        m_currentKo = m_koHistory.back();
        m_koHistory.pop_back();
    } else {
        m_currentKo = KoPoint();
    }
    
    emit boardUpdated();
}

bool ChessLogic::canUndo() const
{
    return !m_moveHistory.empty() && m_gamePhase == GamePhase::Playing;
}

void ChessLogic::requestDraw()
{
    if (m_gamePhase != GamePhase::Playing) return;
    
    m_gameOver = true;
    m_gamePhase = GamePhase::Finished;
    m_gameResult = GameResult::Draw;
    emit gameOver(PieceColor::Empty); // Empty表示和棋
}

// 劫相关实现
bool ChessLogic::isKoPoint(int row, int col) const
{
    return m_currentKo.row == row && m_currentKo.col == col;
}

void ChessLogic::checkAndSetKo(int row, int col)
{
    // 检查是否形成劫
    PieceColor opponent = (m_currentPlayer == PieceColor::Black) ? 
                         PieceColor::White : PieceColor::Black;
    
    int directions[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
    
    for (auto& dir : directions) {
        int r = row + dir[0];
        int c = col + dir[1];
        
        if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE &&
            m_board[r][c] == opponent) {
            
            std::vector<std::pair<int, int>> group;
            findGroup(r, c, opponent, group);
            
            // 如果只有一个棋子被提，且这个棋子能立即提回刚下的子，则形成劫
            if (group.size() == 1 && !hasLiberties(r, c, opponent)) {
                m_currentKo = KoPoint(r, c, opponent, m_moveCount);
                emit koOccurred(r, c);
                return;
            }
        }
    }
    
    // 清除当前劫
    m_currentKo = KoPoint();
}

bool ChessLogic::isKoViolation(int row, int col) const
{
    return isKoPoint(row, col) && 
           m_currentKo.moveNumber == m_moveCount - 1 && // 上一步形成的劫
           m_currentKo.koColor == m_currentPlayer; // 当前玩家不能立即提回
}



void ChessLogic::enterScoringPhase()
{
    m_gamePhase = GamePhase::Scoring;
    markDeadStones(); // 先标记死子
    calculateScore();
    emit gamePhaseChanged(GamePhase::Scoring);
}

void ChessLogic::calculateScore()
{
    if (m_gameMode != GameMode::Go) return;
    
    // 中国规则：子+目=总子数
    m_blackScore = 0;
    m_whiteScore = m_settings.komi; // 贴目
    
    // 计算棋盘上的棋子数
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (m_board[i][j] == PieceColor::Black) {
                m_blackScore++;
            } else if (m_board[i][j] == PieceColor::White) {
                m_whiteScore++;
            }
        }
    }
    
    // 加上提子数
    m_blackScore += m_capturedWhite;
    m_whiteScore += m_capturedBlack;
    
    // 计算领地
    countTerritory();
    
    // 判断胜负
    if (m_blackScore > m_whiteScore) {
        m_gameResult = GameResult::BlackWin;
    } else if (m_whiteScore > m_blackScore) {
        m_gameResult = GameResult::WhiteWin;
    } else {
        m_gameResult = GameResult::Draw;
    }
    
    emit scoreChanged(m_blackScore, m_whiteScore);
}

void ChessLogic::countTerritory()
{
    bool visited[BOARD_SIZE][BOARD_SIZE] = {false};
    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (m_board[i][j] == PieceColor::Empty && !visited[i][j]) {
                // 使用BFS找到连通的空区域
                std::vector<std::pair<int, int>> territory;
                bool touchesBlack = false, touchesWhite = false;
                
                findTerritory(i, j, visited, territory, touchesBlack, touchesWhite);
                
                // 只有被一种颜色包围的空点才算作该方的领地
                if (touchesBlack && !touchesWhite) {
                    m_blackScore += territory.size();
                } else if (touchesWhite && !touchesBlack) {
                    m_whiteScore += territory.size();
                }
                // 如果同时接触黑白双方，则为中立区域，不计分
            }
        }
    }
}

void ChessLogic::findTerritory(int row, int col, bool visited[][BOARD_SIZE], 
                              std::vector<std::pair<int, int>>& territory,
                              bool& touchesBlack, bool& touchesWhite)
{
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || visited[row][col]) {
        return;
    }
    
    visited[row][col] = true;
    
    if (m_board[row][col] == PieceColor::Empty) {
        territory.push_back({row, col});
        
        // 检查四个方向
        int directions[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
        for (auto& dir : directions) {
            int newRow = row + dir[0];
            int newCol = col + dir[1];
            findTerritory(newRow, newCol, visited, territory, touchesBlack, touchesWhite);
        }
    } else if (m_board[row][col] == PieceColor::Black) {
        touchesBlack = true;
    } else if (m_board[row][col] == PieceColor::White) {
        touchesWhite = true;
    }
}

// 计时相关
void ChessLogic::startTimer()
{
    m_timerActive = true;
    // 这里需要QTimer来实际更新时间
}

void ChessLogic::pauseTimer()
{
    m_timerActive = false;
}

void ChessLogic::resumeTimer()
{
    m_timerActive = true;
}

void ChessLogic::updateTimer()
{
    if (!m_timerActive || m_gamePhase != GamePhase::Playing) return;
    
    if (m_currentPlayer == PieceColor::Black) {
        if (m_blackTime > 0) {
            m_blackTime--;
        } else if (m_blackByoYomiPeriods > 0) {
            // 读秒阶段
        } else {
            // 超时判负
            m_gameResult = GameResult::WhiteWin;
            m_gameOver = true;
            m_gamePhase = GamePhase::Finished;
            emit gameOver(PieceColor::White);
        }
    } else {
        if (m_whiteTime > 0) {
            m_whiteTime--;
        } else if (m_whiteByoYomiPeriods > 0) {
            // 读秒阶段
        } else {
            // 超时判负
            m_gameResult = GameResult::BlackWin;
            m_gameOver = true;
            m_gamePhase = GamePhase::Finished;
            emit gameOver(PieceColor::Black);
        }
    }
    
    emit timeUpdated(m_blackTime, m_whiteTime);
}

bool ChessLogic::isInByoYomi(PieceColor color) const
{
    if (color == PieceColor::Black) {
        return m_blackTime <= 0;
    } else {
        return m_whiteTime <= 0;
    }
}

int ChessLogic::getByoYomiPeriods(PieceColor color) const
{
    if (color == PieceColor::Black) {
        return m_blackByoYomiPeriods;
    } else {
        return m_whiteByoYomiPeriods;
    }
}

void ChessLogic::markDeadStones()
{
    // 简化的死子标记：移除无法做出两眼的棋块
    bool processed[BOARD_SIZE][BOARD_SIZE] = {false};
    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (m_board[i][j] != PieceColor::Empty && !processed[i][j]) {
                PieceColor color = m_board[i][j];
                std::vector<std::pair<int, int>> group;
                findGroup(i, j, color, group);
                
                // 标记整个棋块为已处理
                for (auto& pos : group) {
                    processed[pos.first][pos.second] = true;
                }
                
                // 如果棋块是死的，从棋盘上移除
                if (!isGroupAlive(group[0].first, group[0].second, color)) {
                    for (auto& pos : group) {
                        m_board[pos.first][pos.second] = PieceColor::Empty;
                        // 增加对方的提子数
                        if (color == PieceColor::Black) {
                            m_capturedBlack++;
                        } else {
                            m_capturedWhite++;
                        }
                    }
                }
            }
        }
    }
}

bool ChessLogic::isGroupAlive(int row, int col, PieceColor color)
{
    // 简化的活棋判断：检查是否有两个眼
    return hasTwoEyes(row, col, color);
}

bool ChessLogic::hasTwoEyes(int row, int col, PieceColor color)
{
    std::vector<std::pair<int, int>> group;
    findGroup(row, col, color, group);
    
    int eyeCount = 0;
    bool checked[BOARD_SIZE][BOARD_SIZE] = {false};
    
    // 检查棋块周围的所有空点
    for (auto& pos : group) {
        int directions[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
        for (auto& dir : directions) {
            int eyeRow = pos.first + dir[0];
            int eyeCol = pos.second + dir[1];
            
            if (eyeRow >= 0 && eyeRow < BOARD_SIZE && 
                eyeCol >= 0 && eyeCol < BOARD_SIZE &&
                !checked[eyeRow][eyeCol] &&
                m_board[eyeRow][eyeCol] == PieceColor::Empty) {
                
                checked[eyeRow][eyeCol] = true;
                if (isEye(eyeRow, eyeCol, color)) {
                    eyeCount++;
                    if (eyeCount >= 2) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

bool ChessLogic::isEye(int row, int col, PieceColor color)
{
    // 检查一个空点是否为某方的眼
    // 眼的条件：被同色棋子包围，对手无法在此落子
    
    int directions[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
    int friendlyCount = 0;
    
    for (auto& dir : directions) {
        int r = row + dir[0];
        int c = col + dir[1];
        
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) {
            return false; // 边界上的点不太可能是真眼
        }
        
        if (m_board[r][c] == color) {
            friendlyCount++;
        } else if (m_board[r][c] != PieceColor::Empty) {
            return false; // 有对手棋子包围
        }
    }
    
    // 简化判断：如果被三个或四个同色棋子包围，认为是眼
    return friendlyCount >= 3;
}

void ChessLogic::switchPlayer()
{
    m_currentPlayer = (m_currentPlayer == PieceColor::Black) ?
                      PieceColor::White : PieceColor::Black;
}