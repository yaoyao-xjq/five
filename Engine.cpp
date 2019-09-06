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
    AddChess(x, y, m_AIType);  //����������µĽ����Ӽ�������������

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

    // ����
    for (i = 1; ((x + i) <= 14) && (m_chessTable[x + i][y].type == type); i++, value++);
    for (i = 1; ((x - i) >= 0)  && (m_chessTable[x - i][y].type == type); i++, value++);

    if (value >= 5)
    {
        return TRUE;
    }

    // ����
    value = 1;
    for (i = 1; ((y + i) <= 14) && (m_chessTable[x][y + i].type == type); i++, value++);
    for (i = 1; ((y - i) >= 0)  && (m_chessTable[x][y - i].type == type); i++, value++);

    if (value >= 5)
    {
        return TRUE;
    }

    //����-������
    value = 1;
    for (i = 1; ((x + i) <= 14) && ((y + i) <= 14) && (m_chessTable[x + i][y + i].type == type); i++, value++);
    for (i = 1; ((x - i) >= 0)  && ((y - i) >= 0)  && (m_chessTable[x - i][y - i].type == type); i++, value++);

    if (value >= 5)
    {
        return TRUE;
    }

    // ����-����
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
    int *chessMode = chessLine.mode[type - 1]; //���ݴ�������type ����ָ�����ɫ���ӵ�
    int enimyType = 3 - type; //��֪����ʲô��

    memset(chessMode, 0, CHESS_MODE_NUM * sizeof(int));

    // �����ǳ���
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

    // ��������
    i = 0;
    while (i <= 9)
    {
        if (line[i + 5] == -1) //ȥ����б����߽��
        {
            break;
        }

        // �ж��Ƿ��л��� -XXXX-
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
        // �ж�������� X-XXX-X
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

    // ��������
    i = 0;
    while (i < 11)
    {
        if (line[i + 4] == -1)
        {
            break;
        }

        // ����A XXXX-
        if ( (line[i] == type)
          && (line[i + 1] == type)
          && (line[i + 2] == type)
          && (line[i + 3] == type)
          && (line[i + 4] == CHESS_SPACE) )
        {
            chessMode[HALF_FOUR_A]++;
            i += 5; //i+1 �����壬����ѷ������

            // ����ֱ�ӷ���
            if (bTurn)
            {
                return;
            }
        }
        // ����A -XXXX
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
        // ����B X-XXX
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
        // ����B XXX-X
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
        // ����C XX-XX
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

    // �жϻ���
    i = 0;
    while (i < 10)
    {
        if (line[i + 5] == -1)
        {
            break;
        }

        // ����A -XXX--
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
        // ����A -XXX--
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
        // ����B -X-XX-
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
        // ����B -XX-X-
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

    // ��������
    i = 0;
    while (i < 11)
    {
        if (line[i + 4] == -1)
        {
            break;
        }

        // ����A --XXX
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
        // ����A   XXX-- 
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
        // ����B X--XX
        else if ( (line[i] == type)
           && (line[i + 1] == CHESS_SPACE)
           && (line[i + 2] == CHESS_SPACE)
           && (line[i + 3] == type)
           && (line[i + 4] == type))
        {
            chessMode[HALF_THREE_B]++;
            
            i += 5;
        }
        // ����B XX--X
        else if ( (line[i] == type)
           && (line[i + 1] == type)
           && (line[i + 2] == CHESS_SPACE)
           && (line[i + 3] == CHESS_SPACE)
           && (line[i + 4] == type))
        {
            chessMode[HALF_THREE_B]++;
            i += 5;
        }
        // ����C -X-XX
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
        // ����C XX-X-
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
        // ����D -XX-X 
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
        // ����D  X-XX-
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

    // �������  --XX-- --X-X- -X-X-- -X--X-
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

    // �������  OXX--- OX-X--  OX--X-
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

    // ������һ
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

    // ��һ
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
    ChessChange(x, y, 0); //����Ҫд�� ��Ϊ���ӵ��������Ƕ�Ӧ��m_chessline��Ӧ��Ϣû���
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
    Analyse(m_chessLine[0][y], CHESS_BLACK, FALSE); //������һ�к�������������ģʽ  ���������������Y����
    Analyse(m_chessLine[0][y], CHESS_WHITE, FALSE); //������һ�а�������������ģʽ

    // '|'
    m_chessLine[1][x].point[y] = type;
    Analyse(m_chessLine[1][x], CHESS_BLACK, FALSE); //������һ�к�������������ģʽ  ���������������X����
    Analyse(m_chessLine[1][x], CHESS_WHITE, FALSE);

    // '\'
    if (y >= x)
    {
        m_chessLine[2][14 - (y - x)].point[x] = type; //�ϰ�������Ҫ��14��ȥ�ϰ�������������
        Analyse(m_chessLine[2][14 - (y - x)], CHESS_BLACK, FALSE); 
        Analyse(m_chessLine[2][14 - (y - x)], CHESS_WHITE, FALSE);
    }
    else
    {
        m_chessLine[2][14 + (x - y)].point[y] = type; //�°�������Ҫ��14�����ϰ�������������
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
    for (i = 0; i < 29; i++)  //���������ϵĺ����������������������ܵ�һ����ά������
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
        score[0] += chessMode[0][i] * MODE_SCORE[i];   //���ӵ��ܹ����� �ж��ٸ�������*�����Ͷ�Ӧ����
        score[1] += chessMode[1][i] * MODE_SCORE[i];	//���ֵ��ܹ�����
    }

    if (score[0] >= MODE_SCORE[CHESS_FIVE])
    {
        return -1000000;
    }
    else if (score[1] >= MODE_SCORE[CHESS_FIVE])
    {
        return 1000000;
    }

    if (turnType == 1)   //�ֵ�˭����������һ�����ӷ���
    {
        if ( score[0] >= MODE_SCORE[HALF_FOUR_C])
        {
            return -1000000;
        }
        else if (score[1] >= MODE_SCORE[LIVE_FOUR])
        {
            return 1000000;
        }
        else if (score[1] >= MODE_SCORE[HALF_FOUR_C] + MODE_SCORE[LIVE_THREE_B])  //����+����
        {
            return 1000000;
        }
        else if (score[0] >= MODE_SCORE[LIVE_THREE_B])  //�����л���
        {
            if (score[1] < MODE_SCORE[HALF_FOUR_C]) //�������û�г���  
            {
                return -1000000;
            }
            else  //�����г���
            {
                return -500000;
            }
        }
        else if (score[1] >= 2 * MODE_SCORE[LIVE_THREE_B])  //��������������
        {
            if (score[0] < MODE_SCORE[HALF_THREE_E])  //����û�г���
            {
                return 1000000;
            }
            else //�����л���
            {
                return 500000;
            }
        }
        else  //�������µ�
        {
            return score[1] - (int)(1.5 * score[0]); //����
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
        FirstChess();//���˵������������û������  �͸������λ�ã�������ĸ������������λ������
        return;
    }

    list<int> spaceList;
    GetSpaceList(spaceList);//��ȡ�յ����������п�����five �� four ������

    CHESS_POINT *bestPoint = NULL; //�����ֵ��ߵ�
    int alpha = -100000000;
    int beta = 100000000;
    int tempValue = 0;
    int tempX, tempY;

    if (spaceList.size() == 1) //��ʣһ���յ���   ��fivelist����fourlistֱ������
    {
        tempX = (spaceList.back() % 10000) / 100;
        tempY = (spaceList.back()) % 100;
        AIDown(tempX, tempY);
        return;
    }
	//������ǿյ���������С��32�ֵ�
    list<int>::iterator itor = spaceList.begin();
    while(itor != spaceList.end())
    {
        tempX = ((*itor) % 10000) / 100;
        tempY = (*itor) % 100;
        AddChess(tempX, tempY, m_AIType); //�������������ӣ��������
        if (isFive(tempX, tempY))   //���ظ���������**-**���м�Ϊ����������ͬ���ӵ����
        {
            RemoveChess(tempX, tempY); //��addchessʱ�����Ӽ����������ڽ��䵯��
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

// ��ȡ��Ч�յ�
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
			//��������ÿ��λ�ã����λ��û������
            if (m_chessTable[i][j].type == CHESS_SPACE)
            {
				//ȡ���λ��CHESS_POINT��Ϣ
                point = &m_chessTable[i][j];

                blackValue = GetSpaceValue(point->x, point->y, CHESS_BLACK); //���ֵķ���
                whiteValue = GetSpaceValue(point->x, point->y, CHESS_WHITE); //���ֵķ���

                if ((blackValue >= 128) || (whiteValue >= 128))  //���жϺ��ӣ����жϰ��֣����˵�����Ѿ����������������ô�ͷ���
                {
                    fiveList.push_back(100 * point->x + point->y);//�����һ������ִ�У�˵��AI������أ��������ִ�еڶ���������˵������û��128��AI���ӻ�ʤ
                }
                else if ((blackValue >= 32) || (whiteValue >= 32))
                {
                    fourList.push_back(100 * point->x + point->y);
                }
                else 
                {
                    if (blackValue > 0 || whiteValue > 0)
                    {
                        if (blackValue >= whiteValue) //���ӱȰ��ֶ࣬����
                        {
                            spaceList.push_back(blackValue * 10000 + 100 * point->x + point->y);//ͨ����λ����λ����λ�����ֱ�����Ϣ
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
        spaceList = fiveList; //���ϵ�ǰ�յ㣬������ͬ
    }
    else if (fourList.size() > 0)
    {
        spaceList = fourList; //���ϵ�ǰ�յ㣬������ͬ
    }
    else
    {
        spaceList.sort(greater<int>( ));//�����ڵĿյ��������֮��ȡ���ĵ��ʱ��Ӧ���µ�λ��
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
    if (depth == 0 || abs(score) >= 1000000) //�л��ĵȿ��Ա�Ӯ�����
    {
        if (score > 0)
        {
            return score + 10 * depth;  //����������
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

            tempValue = AlphaBeta(alpha, bestValue, depth - 1); //�õ�alpha
            RemoveChess(tempX, tempY);
            if (tempValue < bestValue) //alpha �� beta��ֵ 
            {
                bestValue = tempValue;
            }
            
            if (bestValue <= alpha)  //��֦
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
            tempValue = AlphaBeta(bestValue, beta, depth - 1);//�õ�beta
            RemoveChess(tempX, tempY);
            if (tempValue > bestValue) //beta ��alpha ��ֵ 
            {
                bestValue = tempValue;
            }

            if (bestValue >= beta) //��֦
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

    // ɾ���հ׵�
    map<int, VALUE_POINT*>::iterator itor = m_spaceList.find(key);
    if (itor != m_spaceList.end())
    {
        m_spaceList.erase(itor);
    }

    // ���������
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

    // ������˵�
    m_spaceTable[x][y].ref--;
    if (m_spaceTable[x][y].ref > 0)
    {
        m_spaceList[key] = &m_spaceTable[x][y];
    }

    map<int, VALUE_POINT*>::iterator itor;

    // ɾ��������
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
    int dx[4] = {1, 0, 1, -1,};//ͨ������������ʵ�ֿյ��������ҵ��ƶ�
    int dy[4] = {0, 1, 1, 1, };

    int i, tempX, tempY;
    int curValue = 0;
    bool alone; //�µ㣬�ڵ�ǰ�������һ���㻹�ǿյ�
    bool leftDeath, rightDeath;
    for (i = 0; i < 4; i++)//�жϿյ��ĸ��������
    {
        int tempValue = 0;
        alone = true;
        leftDeath = false;
        rightDeath = false;

		//=================������=====================================
        tempX = x - dx[i];
        tempY = y - dy[i];
        while (IsPointType(tempX, tempY, type))//����ʹ����������ͬ���͵Ļ�
        {
            tempValue++;//������һ
            tempX -= dx[i];//������ǰ�ƶ������ƶ� �ж���һ���������״��
            tempY -= dy[i];
        }//������ͬһ�����͵�ʱ�� ����whileѭ��

        if (!IsPointType(tempX, tempY, CHESS_SPACE))//�����һ���㲻�ǿյ㣬���Ҿ��������while����ʱ�ĵ�Ҳ����ͬһ����ɫ�ĵ�
        {
            leftDeath = true; //˵�����������Ѿ���·����
        }
        else //��һ�����ǿյ�
        {
            if (IsPointType(tempX - dx[i], tempY - dy[i], type))//����õ�����һ����ͬ���͵Ļ��������Ͳ��ǹµ�
            {
                alone = false;
            }
        }
		//=================================================================

		//======================������==========================================
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


        if (tempValue >= 4)//������ͬɫ����
        {
            curValue = 128;
            break;
        }
        else if (tempValue >= 3)
        {
            if (rightDeath || leftDeath) //������ͬɫ��������������һ�������Ѿ�������
            {
                curValue += 16;
            }
            else //����ͬɫ���������������пյ�
            {
                curValue += 64;
            }
            
        }

        else if (tempValue >= 2)//������ͬɫ����
        {
            if (rightDeath || leftDeath)//����һ�����򱻶���
            {
                if (alone) //��ǰ�յ����һ�����ǿյ� �µļ�ֵС
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
                && VALID_POINT(x + 3 * dx[i], y + 3 * dy[i])) //���������������������̷�Χ��
            {
                if ( (m_chessTable[x - 3 * dx[i]][y - 3 * dy[i]].type == type)  //����ͬɫ Ŀ���Ǽ�������λ����û��ͬɫ����
                    && (m_chessTable[x - 2 * dx[i]][y - 2 * dy[i]].type == CHESS_SPACE) //����ͬɫ �����Ա��Ա��ǿ�
                    && (m_chessTable[x - 1 * dx[i]][y - 1 * dy[i]].type == type) //����ͬɫ �����Ա��Ǹ�Ҳ��ͬ����
                    && (m_chessTable[x + 1 * dx[i]][y + 1 * dy[i]].type == type)
                    && (m_chessTable[x + 2 * dx[i]][y + 2 * dy[i]].type == CHESS_SPACE)
                    && (m_chessTable[x + 3 * dx[i]][y + 3 * dy[i]].type == type) )
                {
					//�����ͬɫ����   *0** ��ֵ�ܴ�
                    curValue += 64;
                }
            }
        }
        else if (tempValue == 0)
        {
			//��ֵΪ��  ���Ƿǹµ�
            if (!alone)
            {
                curValue += 1;
            }
        }
        else //ֵΪ1��ʱ��
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