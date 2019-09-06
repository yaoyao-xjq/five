#include "StdAfx.h"
#include "Engine.h"
#include "CalculateTask.h"

const int MODE_SCORE[] = {300000, 30000, 10000, 10000, 10000, 8000, 8000, 1000, 1000, 1000, 1000, 1000, 100, 100, 100, 30, 30, 10};

#define VALID_POINT(x,y)  ((x >= 0) && (x <= 14) && (y >= 0) && (y <= 14))

Engine::Engine(CDialog *dialog)
: m_dialog(dialog)
{
    Init();

    m_threadAI.StartThread(FALSE);
}

Engine::~Engine(void)
{
}

void Engine::Init()
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            m_chessTable[i][j].x = i;
            m_chessTable[i][j].y = j;
            m_chessTable[i][j].type = CHESS_SPACE;
        }
    }

    InitChessLine();

    m_chessList.clear();
    m_gameState = GAME_READY;
    m_userType = CHESS_BLACK;
    m_AIType = CHESS_WHITE;
    m_winer = 0;

    m_gameState = GAME_RUNING;
}

void Engine::StartGame()
{
   Init();
   m_gameState = GAME_RUNING;
}

BOOL Engine::UserDown( int x, int y )
{
    if (m_gameState != GAME_RUNING)
    {
        return FALSE;
    }

    if ((x > 14) || (x < 0) || (y > 14) || (y < 0))
    {
        return FALSE;
    }

    if (m_chessTable[x][y].type != CHESS_SPACE)
    {
        return FALSE;
    }

    //m_chessTable[x][y].type = m_userType;
    //m_chessList.push_back(&m_chessTable[x][y]);

    AddChess(x, y, m_userType);

    if (isFive(x, y))
    {
        m_gameState = GAME_OVER;
        m_winer = m_userType;
    }
    else
    {
        m_gameState = GAME_THINKING;
    }

    return TRUE;
}


BOOL Engine::AIDown( int x, int y )
{
    //m_chessTable[x][y].type = m_AIType;
    AddChess(x, y, m_AIType);  //这个才是正事的将棋子加入链表，并分析

    if (isFive(x, y))
    {
        m_gameState = GAME_OVER;
        m_winer = m_AIType;
    }
    else
    {
        m_gameState = GAME_RUNING;
    }
    
    m_dialog->RedrawWindow();

    return TRUE;
}

BOOL Engine::isFive( int x, int y )
{
    int value = 1;
    int i = 0;

    int type = m_chessTable[x][y].type;

    // 横向
    for (i = 1; ((x + i) <= 14) && (m_chessTable[x + i][y].type == type); i++, value++);
    for (i = 1; ((x - i) >= 0)  && (m_chessTable[x - i][y].type == type); i++, value++);

    if (value >= 5)
    {
        return TRUE;
    }

    // 纵向
    value = 1;
    for (i = 1; ((y + i) <= 14) && (m_chessTable[x][y + i].type == type); i++, value++);
    for (i = 1; ((y - i) >= 0)  && (m_chessTable[x][y - i].type == type); i++, value++);

    if (value >= 5)
    {
        return TRUE;
    }

    //西北-东南向
    value = 1;
    for (i = 1; ((x + i) <= 14) && ((y + i) <= 14) && (m_chessTable[x + i][y + i].type == type); i++, value++);
    for (i = 1; ((x - i) >= 0)  && ((y - i) >= 0)  && (m_chessTable[x - i][y - i].type == type); i++, value++);

    if (value >= 5)
    {
        return TRUE;
    }

    // 西南-东北
    value = 1;
    for (i = 1; ((x + i) <= 14) && ((y - i) >= 0)  && (m_chessTable[x + i][y - i].type == type); i++, value++);
    for (i = 1; ((x - i) >= 0)  && ((y + i) <= 14) && (m_chessTable[x - i][y + i].type == type); i++, value++);

    if (value >= 5)
    {
        return TRUE;
    }

    return FALSE;
}

void Engine::Analyse( CHESS_LINE &chessLine, int type, BOOL bTurn)
{
    int i = 0;
    int *line = chessLine.point;
    int *chessMode = chessLine.mode[type - 1]; //根据传过来的type 定义指向改颜色棋子的
    int enimyType = 3 - type; //不知道做什么用

    memset(chessMode, 0, CHESS_MODE_NUM * sizeof(int));

    // 分析是成五
    int value = 0;
    while (i <= 9)
    {
        if (line[i] == type)
        {
            if ((++value) == 5)
            {
                chessMode[CHESS_FIVE]++;
                return;
            }
        }
        else
        {
            value = 0;
        }

        i++;
    }

    // 分析活四
    i = 0;
    while (i <= 9)
    {
        if (line[i + 5] == -1) //去除除斜方向边界点
        {
            break;
        }

        // 判断是否有活四 -XXXX-
        if ( (line[i] == CHESS_SPACE)
          && (line[i + 1] == type)
          && (line[i + 2] == type)
          && (line[i + 3] == type)
          && (line[i + 4] == type)
          && (line[i + 5] == CHESS_SPACE))
        {
            chessMode[LIVE_FOUR]++;
            return;
        }
        // 判断另类活四 X-XXX-X
        else if ( (i < 9)
           && (line[i] == type)
           && (line[i + 1] == CHESS_SPACE)
           && (line[i + 2] == type)
           && (line[i + 3] == type)
           && (line[i + 4] == type)
           && (line[i + 5] == CHESS_SPACE)
           && (line[i + 6] == type) )
        {
            chessMode[LIVE_FOUR]++;
            return;
        }

        i++;
    }

    // 分析冲四
    i = 0;
    while (i < 11)
    {
        if (line[i + 4] == -1)
        {
            break;
        }

        // 冲四A XXXX-
        if ( (line[i] == type)
          && (line[i + 1] == type)
          && (line[i + 2] == type)
          && (line[i + 3] == type)
          && (line[i + 4] == CHESS_SPACE) )
        {
            chessMode[HALF_FOUR_A]++;
            i += 5; //i+1 无意义，情况已分析完毕

            // 先手直接返回
            if (bTurn)
            {
                return;
            }
        }
        // 冲四A -XXXX
        else if ( (line[i] == CHESS_SPACE)
           && (line[i + 1] == type)
           && (line[i + 2] == type)
           && (line[i + 3] == type)
           && (line[i + 4] == type) )
        {
            chessMode[HALF_FOUR_A]++;
            i += 5;

            if (bTurn)
            {
                return;
            }
        }
        // 冲四B X-XXX
        else if ( (line[i] == type)
           && (line[i + 1] == CHESS_SPACE)
           && (line[i + 2] == type)
           && (line[i + 3] == type)
           && (line[i + 4] == type) )
        {
            chessMode[HALF_FOUR_B]++;
            i += 5;

            if (bTurn)
            {
                return;
            }
        }
        // 冲四B XXX-X
        else if ( (line[i] == type)
           && (line[i + 1] == type)
           && (line[i + 2] == type)
           && (line[i + 3] == CHESS_SPACE)
           && (line[i + 4] == type) )
        {
            chessMode[HALF_FOUR_B]++;
            i += 5;

            if (bTurn)
            {
                return;
            }
        }
        // 冲四C XX-XX
        else if ( (line[i] == type)
           && (line[i + 1] == type)
           && (line[i + 2] == CHESS_SPACE)
           && (line[i + 3] == type)
           && (line[i + 4] == type) )
        {
            chessMode[HALF_FOUR_C]++;
            i += 5;

            if (bTurn)
            {
                return;
            }
        }
        else
        {
            i++;
        }
    }

    // 判断活三
    i = 0;
    while (i < 10)
    {
        if (line[i + 5] == -1)
        {
            break;
        }

        // 活三A -XXX--
        if (   (line[i] == CHESS_SPACE)
            && (line[i + 1] == type)
            && (line[i + 2] == type)
            && (line[i + 3] == type)
            && (line[i + 4] == CHESS_SPACE)
            && (line[i + 5] == CHESS_SPACE))
        {
            chessMode[LIVE_THREE_A]++;;
            i += 6;
        }
        // 活三A -XXX--
        else if ( (line[i] == CHESS_SPACE)
           && (line[i + 1] == CHESS_SPACE)
           && (line[i + 2] == type)
           && (line[i + 3] == type)
           && (line[i + 4] == type)
           && (line[i + 5] == CHESS_SPACE))
        {
            chessMode[LIVE_THREE_A]++;
            i += 6;
        }
        // 活三B -X-XX-
        else if ( (line[i] == CHESS_SPACE)
           && (line[i + 1] == type)
           && (line[i + 2] == CHESS_SPACE)
           && (line[i + 3] == type)
           && (line[i + 4] == type)
           && (line[i + 5] == CHESS_SPACE))
        {
            chessMode[LIVE_THREE_B]++;
            i += 6;
        }
        // 活三B -XX-X-
        else if ( (line[i] == CHESS_SPACE)
            && (line[i + 1] == type)
            && (line[i + 2] == type)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == type)
            && (line[i + 5] == CHESS_SPACE))
        {
            chessMode[LIVE_THREE_B]++;
            i += 6;
        }
        else
        {
            i++;
        }
    }

    // 分析冲三
    i = 0;
    while (i < 11)
    {
        if (line[i + 4] == -1)
        {
            break;
        }

        // 冲三A --XXX
        if (  (line[i] == CHESS_SPACE)
           && (line[i + 1] == CHESS_SPACE)
           && (line[i + 2] == type)
           && (line[i + 3] == type)
           && (line[i + 4] == type))
        {
            if ((i + 4 == 14) 
             || (line[i + 5] == enimyType)
             || (line[i + 5] == -1))
            {
                chessMode[HALF_THREE_A]++;
            }

            i += 5;
        }
        // 冲三A   XXX-- 
        else if ( (line[i] == type)
           && (line[i + 1] == type)
           && (line[i + 2] == type)
           && (line[i + 3] == CHESS_SPACE)
           && (line[i + 4] == CHESS_SPACE))
        {
            if ((i == 0) || (line[i - 1] == enimyType))
            {
                chessMode[HALF_THREE_A]++;
            }

            i += 5;
        }
        // 冲三B X--XX
        else if ( (line[i] == type)
           && (line[i + 1] == CHESS_SPACE)
           && (line[i + 2] == CHESS_SPACE)
           && (line[i + 3] == type)
           && (line[i + 4] == type))
        {
            chessMode[HALF_THREE_B]++;
            
            i += 5;
        }
        // 冲三B XX--X
        else if ( (line[i] == type)
           && (line[i + 1] == type)
           && (line[i + 2] == CHESS_SPACE)
           && (line[i + 3] == CHESS_SPACE)
           && (line[i + 4] == type))
        {
            chessMode[HALF_THREE_B]++;
            i += 5;
        }
        // 冲三C -X-XX
        else if ( (line[i] == CHESS_SPACE)
           && (line[i + 1] == type)
           && (line[i + 2] == CHESS_SPACE)
           && (line[i + 3] == type)
           && (line[i + 4] == type))
        {
            if ((i == 10) || (line[i + 5] == enimyType) || (line[i + 5] == -1))
            {
                chessMode[HALF_THREE_C]++;
            }
            
            i += 5;
        }
        // 冲三C XX-X-
        else if ((line[i] == type)
          && (line[i + 1] == type)
          && (line[i + 2] == CHESS_SPACE)
          && (line[i + 3] == type)
          && (line[i + 4] == CHESS_SPACE) )
        {
            if ((i == 0) || (line[i - 1] == enimyType))
            {
                chessMode[HALF_THREE_C]++;
            }

            i += 5;
        }
        // 冲三D -XX-X 
        else if ((line[i] == CHESS_SPACE)
            && (line[i + 1] == type)
            && (line[i + 2] == type)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == type) )
        {
            if ((i == 10) || (line[i + 5] == enimyType) || (line[i + 5] == -1))
            {
                chessMode[HALF_THREE_C]++;
            }
            
            i += 5;
        }
        // 冲三D  X-XX-
        else if ((line[i] == type)
          && (line[i + 1] == CHESS_SPACE)
          && (line[i + 2] == type)
          && (line[i + 3] == type)
          && (line[i + 4] == CHESS_SPACE) )
        {
            if ((i == 0) || (line[i] == enimyType))
            {
                chessMode[HALF_THREE_D]++;
            }

            i += 5;
        }
        // X-X-X 
        else if ((line[i] == type)
          && (line[i + 1] == CHESS_SPACE)
          && (line[i + 2] == type)
          && (line[i + 3] == CHESS_SPACE)
          && (line[i + 4] == type) )
        {
            chessMode[HALF_THREE_E]++;
            i += 5;
        }
        else
        {
            i++;
        }
    }

    // 分析活二  --XX-- --X-X- -X-X-- -X--X-
    i = 0;
    while (i < 10) 
    {
        if (line[i + 5] == -1)
        {
            break;
        }

        if ( (line[i] == CHESS_SPACE)
          && (line[i + 1] == CHESS_SPACE)
          && (line[i + 2] == type)
          && (line[i + 3] == type)
          && (line[i + 4] == CHESS_SPACE)
          && (line[i + 5] == CHESS_SPACE))
        {
            chessMode[LIVE_TWO_A]++;
            i += 6;
        }
        else if ((line[i] == CHESS_SPACE)
          && (line[i + 1] == type)
          && (line[i + 2] == CHESS_SPACE)
          && (line[i + 3] == type)
          && (line[i + 4] == CHESS_SPACE)
          && (line[i + 5] == CHESS_SPACE))
        {
            chessMode[LIVE_TWO_B]++;
            i += 6;
        }
        else if ((line[i] == CHESS_SPACE)
            && (line[i + 1] == CHESS_SPACE)
            && (line[i + 2] == type)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == type)
            && (line[i + 5] == CHESS_SPACE))
        {
            chessMode[LIVE_TWO_B]++;
            i += 6;
        }
        else if ((line[i] == CHESS_SPACE)
          && (line[i + 1] == type)
          && (line[i + 2] == CHESS_SPACE)
          && (line[i + 3] == CHESS_SPACE)
          && (line[i + 4] == type)
          && (line[i + 5] == CHESS_SPACE))
        {
            chessMode[LIVE_TWO_C]++;
            i += 6;
        }
        else
        {
            i++;
        }
    }

    // 分析冲二  OXX--- OX-X--  OX--X-
    i = 0;
    while (i < 10) 
    {
        if (line[i + 5] == -1)
        {
            break;
        }

        if ( (line[i] == type)
            && (line[i + 1] == type)
            && (line[i + 2] == CHESS_SPACE)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == CHESS_SPACE))
        {
            if ((i == 0) || (line[i - 1] != CHESS_SPACE))
            {
                chessMode[HALF_TWO]++;
            }

            i += 5;
        }
        else if ((line[i] == CHESS_SPACE)
            && (line[i + 1] == CHESS_SPACE)
            && (line[i + 2] == CHESS_SPACE)
            && (line[i + 3] == type)
            && (line[i + 4] == type))
        {
            if ((i == 10) || (line[i + 5] != CHESS_SPACE))
            {
                chessMode[HALF_TWO]++;
            }
            i += 5;
        }
        else if ((line[i] == type)
            && (line[i + 1] == CHESS_SPACE)
            && (line[i + 2] == type)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == CHESS_SPACE))
        {
            if ((i == 0) || (line[i - 1] != CHESS_SPACE))
            {
                chessMode[HALF_TWO]++;
            }

            i += 5;
        }
        else if ((line[i] == CHESS_SPACE)
            && (line[i + 1] == CHESS_SPACE)
            && (line[i + 2] == type)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == type))
        {
            if ((i == 10) || (line[i + 5] != CHESS_SPACE))
            {
                chessMode[HALF_TWO]++;
            }
            i += 6;
        }
        else if ((line[i] == type)
            && (line[i + 1] == CHESS_SPACE)
            && (line[i + 2] == CHESS_SPACE)
            && (line[i + 3] == type)
            && (line[i + 4] == CHESS_SPACE))
        {
            if ((i == 0) || (line[i - 1] != CHESS_SPACE))
            {
                chessMode[HALF_TWO]++;
            }

            i += 5;
        }
        else if ((line[i] == CHESS_SPACE)
            && (line[i + 1] == type)
            && (line[i + 2] == CHESS_SPACE)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == type))
        {
            if ((i == 10) || (line[i + 5] != CHESS_SPACE))
            {
                chessMode[HALF_TWO]++;
            }
            i += 6;
        }
        else
        {
            i++;
        }
    }  

    // 分析活一
    i = 0;
    while (i < 10) 
    {
        if (line[i + 5] == -1)
        {
            break;
        }

        if ( (line[i] == CHESS_SPACE)
            && (line[i + 1] == CHESS_SPACE)
            && (line[i + 2] == type)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == CHESS_SPACE))
        {
            chessMode[LIVE_ONE]++;

            i += 5;
        }
        else
        {
            i++;
        }
    }

    // 冲一
    i = 0;
    while (i < 10) 
    {
        if (line[i + 5] == -1)
        {
            break;
        }

        if ( (line[i] == type)
            && (line[i + 1] == CHESS_SPACE)
            && (line[i + 2] == CHESS_SPACE)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == CHESS_SPACE))
        {
            if ((i == 0) || (line[i - 1] != CHESS_SPACE))
            {
                chessMode[HALF_ONE]++;
            }
            
            i += 5;
        }
        else if ( (line[i] == CHESS_SPACE)
            && (line[i + 1] == type)
            && (line[i + 2] == CHESS_SPACE)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == CHESS_SPACE))
       {
            if ((i == 0) || (line[i - 1] != CHESS_SPACE))
            {
                chessMode[HALF_ONE]++;
            }

            i += 5;
        }
        else if ( (line[i] == CHESS_SPACE)
            && (line[i + 1] == CHESS_SPACE)
            && (line[i + 2] == CHESS_SPACE)
            && (line[i + 3] == type)
            && (line[i + 4] == CHESS_SPACE))
        {
            if ((i == 10) || (line[i + 5] != CHESS_SPACE))
            {
                chessMode[HALF_ONE]++;
            }

            i += 5;
        }
        else if ( (line[i] == CHESS_SPACE)
            && (line[i + 1] == CHESS_SPACE)
            && (line[i + 2] == CHESS_SPACE)
            && (line[i + 3] == CHESS_SPACE)
            && (line[i + 4] == type))
        {
            if ((i == 10) || (line[i + 5] != CHESS_SPACE))
            {
                chessMode[HALF_ONE]++;
            }

            i += 5;
        }
        else
        {
            i++;
        }
    }
}

void Engine::AddChess( int x, int y, int type )
{
    m_chessTable[x][y].type = type;
    m_chessList.push_back(&m_chessTable[x][y]);
    ChessChange(x, y, type);
    //AddSpace(x, y);
}

void Engine::RemoveChess( int x, int y)
{
    m_chessTable[x][y].type = 0;
    m_chessList.pop_back();
    ChessChange(x, y, 0); //必须要写的 因为棋子弹出，但是对应的m_chessline对应信息没清空
    //RemoveSpace(x, y);
}

BOOL Engine::Retract()
{
    BOOL result = FALSE;
    if (m_gameState != GAME_THINKING)
    {
        if (m_chessList.size() > 0)
        {
            CHESS_POINT *point = m_chessList.back();
            RemoveChess(point->x, point->y);
            result = TRUE;
        }

        if (m_chessList.size() > 0)
        {
            CHESS_POINT *point = m_chessList.back();
            RemoveChess(point->x, point->y);       
        }
    }

    return result;
}

void Engine::ChessChange(int x, int y, int type)
{
    // '-'
    m_chessLine[0][y].point[x] = type;
    Analyse(m_chessLine[0][y], CHESS_BLACK, FALSE); //计算这一行黑子是盘面那种模式  代表从上往下数第Y条线
    Analyse(m_chessLine[0][y], CHESS_WHITE, FALSE); //计算这一行白子是盘面那种模式

    // '|'
    m_chessLine[1][x].point[y] = type;
    Analyse(m_chessLine[1][x], CHESS_BLACK, FALSE); //计算这一列黑子是盘面那种模式  代表从左往右数第X条线
    Analyse(m_chessLine[1][x], CHESS_WHITE, FALSE);

    // '\'
    if (y >= x)
    {
        m_chessLine[2][14 - (y - x)].point[x] = type; //上半区，需要用14减去上半区空余线条数
        Analyse(m_chessLine[2][14 - (y - x)], CHESS_BLACK, FALSE); 
        Analyse(m_chessLine[2][14 - (y - x)], CHESS_WHITE, FALSE);
    }
    else
    {
        m_chessLine[2][14 + (x - y)].point[y] = type; //下半区，需要用14加上上半区空余线条数
        Analyse(m_chessLine[2][14 + (x - y)], CHESS_BLACK, FALSE);
        Analyse(m_chessLine[2][14 + (x - y)], CHESS_WHITE, FALSE);
    }

    // '/'
    if (x + y <= 14)
    {
        m_chessLine[3][x + y].point[x] = type;
    }
    else
    {
        m_chessLine[3][x + y].point[14 - y] = type;
    }

    Analyse(m_chessLine[3][x + y], CHESS_BLACK, FALSE);
    Analyse(m_chessLine[3][x + y], CHESS_WHITE, FALSE);
}

void Engine::InitChessLine()
{
    int i, j, k;

    for (i = 0; i < 4; i++)
    {
        if (i < 2)
        {
            for (j = 0; j < 15; j++)
            {
                memset(&m_chessLine[i][j], 0, sizeof(CHESS_LINE));
            }
        }
        else
        {
            for (j = 0; j < 29; j++)
            {
                memset(&m_chessLine[i][j], 0, sizeof(CHESS_LINE));
                if (j <= 14)
                {
                    for (k = j + 1; k < 15; k++)
                    {
                        m_chessLine[i][j].point[k] = -1;
                    }
                }
                else
                {
                    for (k = 29 - j; k < 15; k++)
                    {
                        m_chessLine[i][j].point[k] = -1;
                    }
                }
            }
        }
    }
}

int Engine::CalculateStore(int turnType)
{
    int chessMode[2][CHESS_MODE_NUM] = {0};
    int i, j;

    // '| -'
    for (i = 0; i < 29; i++)  //将所有线上的黑棋琪况、白棋琪况，汇总到一个二维数组中
    {
        for (j = 0; j < CHESS_MODE_NUM; j++)
        {
            if (i < 15)
            {
                chessMode[0][j] += m_chessLine[0][i].mode[0][j];  
                chessMode[1][j] += m_chessLine[0][i].mode[1][j];
                chessMode[0][j] += m_chessLine[1][i].mode[0][j];
                chessMode[1][j] += m_chessLine[1][i].mode[1][j];
            }
            
            chessMode[0][j] += m_chessLine[2][i].mode[0][j];
            chessMode[1][j] += m_chessLine[2][i].mode[1][j];
            chessMode[0][j] += m_chessLine[3][i].mode[0][j];
            chessMode[1][j] += m_chessLine[3][i].mode[1][j];
        }
     }

    int score[2] = {0};
     for (i = 0; i < CHESS_MODE_NUM; i++)
    {
        score[0] += chessMode[0][i] * MODE_SCORE[i];   //黑子的总共分数 有多少个此类型*此类型对应分数
        score[1] += chessMode[1][i] * MODE_SCORE[i];	//白字的总共分数
    }

    if (score[0] >= MODE_SCORE[CHESS_FIVE])
    {
        return -1000000;
    }
    else if (score[1] >= MODE_SCORE[CHESS_FIVE])
    {
        return 1000000;
    }

    if (turnType == 1)   //轮到谁，先算另外一个棋子分数
    {
        if ( score[0] >= MODE_SCORE[HALF_FOUR_C])
        {
            return -1000000;
        }
        else if (score[1] >= MODE_SCORE[LIVE_FOUR])
        {
            return 1000000;
        }
        else if (score[1] >= MODE_SCORE[HALF_FOUR_C] + MODE_SCORE[LIVE_THREE_B])  //充四+活三
        {
            return 1000000;
        }
        else if (score[0] >= MODE_SCORE[LIVE_THREE_B])  //黑子有活三
        {
            if (score[1] < MODE_SCORE[HALF_FOUR_C]) //如果白字没有冲四  
            {
                return -1000000;
            }
            else  //白子有冲四
            {
                return -500000;
            }
        }
        else if (score[1] >= 2 * MODE_SCORE[LIVE_THREE_B])  //白字有两个活三
        {
            if (score[0] < MODE_SCORE[HALF_THREE_E])  //黑子没有冲三
            {
                return 1000000;
            }
            else //黑子有活三
            {
                return 500000;
            }
        }
        else  //冲三以下的
        {
            return score[1] - (int)(1.5 * score[0]); //问题
        }
    }
    else
    {
        if ( score[1] >= MODE_SCORE[HALF_FOUR_C])
        {
            return 1000000;
        }
        else if (score[0] >= MODE_SCORE[LIVE_FOUR])
        {
            return -1000000;
        }
        else if (score[0] >= MODE_SCORE[HALF_FOUR_C] + MODE_SCORE[LIVE_THREE_B])
        {
            return -1000000;
        }
        else if (score[1] >= MODE_SCORE[LIVE_THREE_B])
        {
            if (score[0] < MODE_SCORE[HALF_FOUR_C])
            {
                return 1000000;
            }
            else
            {
                return 500000;
            }
        }
        else if (score[0] >= 2 * MODE_SCORE[LIVE_THREE_B])
        {
            if (score[1] < MODE_SCORE[HALF_THREE_E])
            {
                return -1000000;
            }
            else
            {
                return -500000;
            }
        }
        else if (score[1] >= 2 * MODE_SCORE[LIVE_TWO_C])
        {
            if (score[0] < MODE_SCORE[LIVE_THREE_B])
            {
                return 400000;
            }
            else
            {
                return 10000;
            }
        }
        else if (score[0] >= 2 * MODE_SCORE[LIVE_TWO_C])
        {
            if (score[1] < MODE_SCORE[LIVE_THREE_B])
            {
                return -400000;
            }
            else
            {
                return -10000;
            }
        }
        else
        {
            return  (int)(1.5 * score[1]) - score[0];
        }
    }
}

void Engine::Thinking()
{
    if (m_chessList.size() < 2)
    {
        FirstChess();//如果说现在棋子链表没有棋子  就根据玩家位置，在玩家四个方向空余最大的位置落子
        return;
    }

    list<int> spaceList;
    GetSpaceList(spaceList);//获取空点链表、其中有可能是five 和 four 的链表

    CHESS_POINT *bestPoint = NULL; //定义分值最高点
    int alpha = -100000000;
    int beta = 100000000;
    int tempValue = 0;
    int tempX, tempY;

    if (spaceList.size() == 1) //就剩一个空点了   有fivelist或者fourlist直接落子
    {
        tempX = (spaceList.back() % 10000) / 100;
        tempY = (spaceList.back()) % 100;
        AIDown(tempX, tempY);
        return;
    }
	//下面就是空点链表里面小于32分的
    list<int>::iterator itor = spaceList.begin();
    while(itor != spaceList.end())
    {
        tempX = ((*itor) % 10000) / 100;
        tempY = (*itor) % 100;
        AddChess(tempX, tempY, m_AIType); //分析如果这点落子，棋盘情况
        if (isFive(tempX, tempY))   //不重复，包含了**-**等中间为空两边有相同棋子的情况
        {
            RemoveChess(tempX, tempY); //在addchess时将棋子加入链表，现在讲其弹出
            bestPoint = &m_chessTable[tempX][tempY];
            break;
        }

        tempValue = AlphaBeta(alpha, beta, 3);
        RemoveChess(tempX, tempY);
        if (tempValue > alpha)
        {
            alpha = tempValue;
            bestPoint = &m_chessTable[tempX][tempY];
        }

        itor++;
    }

    AIDown(bestPoint->x, bestPoint->y);
}

// 获取有效空点
void Engine::GetSpaceList(list<int> &spaceList)
{
    CHESS_POINT* point = NULL;
    int blackValue = 0;
    int whiteValue = 0;

    list<int> fiveList;
    list<int> fourList;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
			//遍历棋盘每个位置，如果位置没有棋子
            if (m_chessTable[i][j].type == CHESS_SPACE)
            {
				//取这个位置CHESS_POINT信息
                point = &m_chessTable[i][j];

                blackValue = GetSpaceValue(point->x, point->y, CHESS_BLACK); //黑字的分数
                whiteValue = GetSpaceValue(point->x, point->y, CHESS_WHITE); //白字的分数

                if ((blackValue >= 128) || (whiteValue >= 128))  //先判断黑子，后判断白字，如果说黑子已经产生这种情况，那么就防守
                {
                    fiveList.push_back(100 * point->x + point->y);//如果第一个条件执行，说明AI必须防守，但是如果执行第二个条件，说明黑子没到128，AI落子获胜
                }
                else if ((blackValue >= 32) || (whiteValue >= 32))
                {
                    fourList.push_back(100 * point->x + point->y);
                }
                else 
                {
                    if (blackValue > 0 || whiteValue > 0)
                    {
                        if (blackValue >= whiteValue) //黑子比白字多，防守
                        {
                            spaceList.push_back(blackValue * 10000 + 100 * point->x + point->y);//通过万位、百位、个位，来分别点的信息
                        }
                        else
                        {
                            spaceList.push_back(whiteValue * 10000 + 100 * point->x + point->y);
                        }
                    }
                }
            }
        }
    }

    if (fiveList.size() > 0)
    {
        spaceList = fiveList; //算上当前空点，五子相同
    }
    else if (fourList.size() > 0)
    {
        spaceList = fourList; //算上当前空点，四子相同
    }
    else
    {
        spaceList.sort(greater<int>( ));//对现在的空点进行排序，之后取出的点才时最应该下的位置
		//spaceList.sort<int>;
    }
}

void Engine::CheckPoint( int x, int y, map<int, CHESS_POINT*> &stateMap, list<CHESS_POINT*> &spaceList )
{
    if (!VALID_POINT(x, y))
    {
        return;
    }

    if (m_chessTable[x][y].type != CHESS_SPACE)
    {
        return;
    }

    int key = x * 100 + y;
    if (stateMap[key] == NULL)
    {
        stateMap[key] = &m_chessTable[x][y];
        if ( (GetValue(x, y, m_AIType) > 0)
          || (GetValue(x, y, m_userType) > 0))
        {
            spaceList.push_front(&m_chessTable[x][y]);
        }
        else
        {
            spaceList.push_back(&m_chessTable[x][y]);
        }
    } 
}

void Engine::TurnAI()
{

    if (m_gameState == GAME_THINKING)
    {
        m_threadAI.PushTask(new CalculateTask(this));
    }
}

int Engine::AlphaBeta( int alpha, int beta, int depth )
{
    int type = (3 - depth) % 2 + 1;
     int score = CalculateStore(type);
    if (depth == 0 || abs(score) >= 1000000) //有活四等可以必赢的情况
    {
        if (score > 0)
        {
            return score + 10 * depth;  //白字有优势
        }
        else
        {
            return score - 10 * depth;
        }
    }

    list<int> spaceList;
    GetSpaceList(spaceList);

    CHESS_POINT *bestPoint = NULL;
    CHESS_POINT *tempPoint = NULL;
    int tempValue = 0;
    int bestValue = 0;
    int tempX, tempY;

    int i = 20;
    list<int>::iterator itor = spaceList.begin();
    if (type == m_userType)
    {
        bestValue = beta;
        
        while((itor != spaceList.end()) && (i-- > 0))
        {
            tempX = ((*itor) % 10000) / 100;
            tempY = (*itor) % 100;
            AddChess(tempX, tempY, m_userType);
            if (isFive(tempX, tempY))
            {
                RemoveChess(tempX, tempY);
                bestValue = -1000000;
                break;
            }

            tempValue = AlphaBeta(alpha, bestValue, depth - 1); //得到alpha
            RemoveChess(tempX, tempY);
            if (tempValue < bestValue) //alpha 给 beta赋值 
            {
                bestValue = tempValue;
            }
            
            if (bestValue <= alpha)  //剪枝
            {
                return bestValue;
            }

            itor++;
        }
    }
    else
    {
        bestValue = alpha;
        while((itor != spaceList.end()) && (i-- > 0))
        {
            tempX = ((*itor) % 10000) / 100;
            tempY = (*itor) % 100;
            AddChess(tempX, tempY, m_AIType);
            if (isFive(tempX, tempY))
            {
                RemoveChess(tempX, tempY);
                bestValue = 1000000;
                break;
            }
            tempValue = AlphaBeta(bestValue, beta, depth - 1);//得到beta
            RemoveChess(tempX, tempY);
            if (tempValue > bestValue) //beta 给alpha 赋值 
            {
                bestValue = tempValue;
            }

            if (bestValue >= beta) //剪枝
            {
                return bestValue;
            }

            itor++;
        }
    }

    return bestValue;
}

int Engine::GetValue(int x, int y, int type)
{
    int value = 0;

    int i, j;
    int temp = 1;
    int left = 0;
    int right = 0;

    int dx[4] = {1, 0, 1, 1};
    int dy[4] = {0, 1, 1, -1};

    for (int k = 0; k < 4; k++)
    {
        temp = 1;
        for (i = x - dx[k], j = y - dy[k]; (i >= 0) && (i <= 14) && (j >= 0) && (j <= 14) && (m_chessTable[i][j].type == type); i -= dx[k], j -= dy[k])
        {
            temp++;
        }
        if ((i < 0) || (i > 14) || (j < 0) || (j > 14) ||  (m_chessTable[i][j].type != CHESS_SPACE))
        {
            left = 1;
        }

        for (i = x + dx[k], j = y + dy[k]; (i >= 0) && (i <= 14) && (j >= 0) && (j <= 14) && (m_chessTable[i][j].type == type); i += dx[k], j += dy[k])
        {
            temp++;
        }
        if ((i > 14) || ((i <=14) && (m_chessTable[i][y].type != CHESS_SPACE)))
        {
            right = 1;
        }

        if (temp >= 5)
        {
            value += 10;
        }

        if (temp >= 4)
        {
            if ((left + right == 0))
            {
                value += 5;
            }
        }

        if (temp >= 3)
        {
            if ((left + right == 0))
            {
                value += 2;
            }
        }
    }

    return value;
}

void Engine::AddSpace( int x, int y )
{
    int tempX, tempY;
    int key = 100 * x + y;

    // 删除空白点
    map<int, VALUE_POINT*>::iterator itor = m_spaceList.find(key);
    if (itor != m_spaceList.end())
    {
        m_spaceList.erase(itor);
    }

    // 加入关联点
    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            tempX = x + i;
            tempY = y + j;

            if (!VALID_POINT(tempX, tempY))
            {
                continue;
            }

            m_spaceTable[tempX][tempY].ref++;
            if (m_chessTable[tempX][tempY].type != CHESS_SPACE)
            {
                continue;
            }

            key = tempX * 100 + tempY;
            m_spaceList[key] = &m_spaceTable[tempX][tempY];
        }
    }
}

void Engine::RemoveSpace( int x, int y )
{
    int tempX, tempY;
    int key = 100 * x + y;

    // 加入回退点
    m_spaceTable[x][y].ref--;
    if (m_spaceTable[x][y].ref > 0)
    {
        m_spaceList[key] = &m_spaceTable[x][y];
    }

    map<int, VALUE_POINT*>::iterator itor;

    // 删除关联点
    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            tempX = x + i;
            tempY = y + j;

            if (!VALID_POINT(tempX, tempY))
            {
                continue;
            }

            m_spaceTable[tempX][tempY].ref--;

            key = tempX * 100 + tempY;
            itor = m_spaceList.find(key);
            if (itor != m_spaceList.end())
            {
                if (itor->second->ref == 0)
                {
                    m_spaceList.erase(itor);
                }
            }
        }
    }
}

int Engine::GetSpaceValue( int x, int y, int type )
{
    int dx[4] = {1, 0, 1, -1,};//通过这两个矩阵，实现空点上下左右的移动
    int dy[4] = {0, 1, 1, 1, };

    int i, tempX, tempY;
    int curValue = 0;
    bool alone; //孤点，在当前方向的下一个点还是空点
    bool leftDeath, rightDeath;
    for (i = 0; i < 4; i++)//判断空点四个方向情况
    {
        int tempValue = 0;
        alone = true;
        leftDeath = false;
        rightDeath = false;

		//=================向左走=====================================
        tempX = x - dx[i];
        tempY = y - dy[i];
        while (IsPointType(tempX, tempY, type))//如果和传入的类型是同类型的话
        {
            tempValue++;//分数加一
            tempX -= dx[i];//继续向当前移动方向移动 判断下一个点的棋子状况
            tempY -= dy[i];
        }//当不是同一个类型的时候 跳出while循环

        if (!IsPointType(tempX, tempY, CHESS_SPACE))//如果下一个点不是空点，并且经过上面的while，此时的点也不是同一个颜色的点
        {
            leftDeath = true; //说明左面现在已经无路可走
        }
        else //下一个点是空点
        {
            if (IsPointType(tempX - dx[i], tempY - dy[i], type))//如果该点下下一个是同类型的话，这个点就不是孤点
            {
                alone = false;
            }
        }
		//=================================================================

		//======================向右走==========================================
        tempX = x + dx[i];
        tempY = y + dy[i];
        while (IsPointType(tempX, tempY, type))
        {
            tempValue++;
            tempX += dx[i];
            tempY += dy[i];
        }

        if (!IsPointType(tempX, tempY, CHESS_SPACE))
        {
            rightDeath = true;
        }
        else
        {
            if (IsPointType(tempX + dx[i], tempY + dy[i], type))
            {
                alone = false;
            }
        }
		//=================================================================


        if (tempValue >= 4)//有四子同色相连
        {
            curValue = 128;
            break;
        }
        else if (tempValue >= 3)
        {
            if (rightDeath || leftDeath) //有三字同色相连，但是其中一个方向已经被堵死
            {
                curValue += 16;
            }
            else //三字同色相连，两个方向都有空点
            {
                curValue += 64;
            }
            
        }

        else if (tempValue >= 2)//有两子同色相连
        {
            if (rightDeath || leftDeath)//其中一个方向被堵死
            {
                if (alone) //当前空点的下一个还是空点 下的价值小
                {
                    curValue += 4;
                }
                else
                {
                    curValue += 16;
                }
            }
            else
            {
                curValue += 16;
            }

            if (   VALID_POINT(x - 3 * dx[i], y - 3 * dy[i]) 
                && VALID_POINT(x + 3 * dx[i], y + 3 * dy[i])) //最起码走三步，还在棋盘范围内
            {
                if ( (m_chessTable[x - 3 * dx[i]][y - 3 * dy[i]].type == type)  //两字同色 目的是检测第三个位置有没有同色棋子
                    && (m_chessTable[x - 2 * dx[i]][y - 2 * dy[i]].type == CHESS_SPACE) //两字同色 所以旁边旁边是空
                    && (m_chessTable[x - 1 * dx[i]][y - 1 * dy[i]].type == type) //两字同色 所以旁边那个也是同类型
                    && (m_chessTable[x + 1 * dx[i]][y + 1 * dy[i]].type == type)
                    && (m_chessTable[x + 2 * dx[i]][y + 2 * dy[i]].type == CHESS_SPACE)
                    && (m_chessTable[x + 3 * dx[i]][y + 3 * dy[i]].type == type) )
                {
					//如果有同色棋子   *0** 价值很大
                    curValue += 64;
                }
            }
        }
        else if (tempValue == 0)
        {
			//价值为零  但是非孤点
            if (!alone)
            {
                curValue += 1;
            }
        }
        else //值为1的时候
        {
            if (alone)
            {
                curValue += 1;
            }
            else
            {
                curValue += 4;
            }
        }
    }

    return curValue;
}

bool Engine::IsPointType( int x, int y, int type )
{
    if (VALID_POINT(x,y) && m_chessTable[x][y].type == type)
    {
        return true;
    }

    return false;
}

void Engine::FirstChess()
{
    if (m_chessList.size() == 0)
    {
        AIDown(7, 7);
    }
    else
    {
        int shift_x = 0;
        int shift_y = 0;
        CHESS_POINT *point = m_chessList.back();
        if ((point->x < 7) && (point->y < 7))
        {
            shift_x = 1;
            shift_y = 1;
        }
        else if ((point->x < 7) && (point->y > 7))
        {
            shift_x = 1;
            shift_y = -1;
        }
        else if ((point->x > 7) && (point->y < 7))
        {
            shift_x = -1;
            shift_y = 1;
        }
        else if ((point->x > 7) && (point->y > 7))
        {
            shift_x = -1;
            shift_y = -1;
        }
        else if ((point->x == 7) && (point->y > 7))
        {
            shift_x = 0;
            shift_y = -1;

        }
        else if ((point->x == 7) && (point->y < 7))
        {
            shift_x = 0;
            shift_y = 1;
        }
        else if ((point->x > 7) && (point->y == 7))
        {
            shift_x = -1;
            shift_y = 0;
        }
        else if ((point->x < 7) && (point->y == 7))
        {
            shift_x = 1;
            shift_y = 0;
        }
        else
        {
            long rand = time(NULL);
            rand = rand % 9;
            if (rand == 4)
            {
                rand = 5;
            }

            shift_x = (int)(rand % 3 - 1);
            shift_y = (int)(rand / 3 - 1);
        }

        AIDown(point->x + shift_x, point->y + shift_y);
    }
}