#ifndef FIVE_ENGINE_H
#define FIVE_ENGINE_H

#include "ThreadAI.h"
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include<functional>

using namespace std;

enum GAME_STATUS
{
    GAME_READY = 0,
    GAME_RUNING,
    GAME_THINKING,
    GAME_OVER
};

enum CHESS_TYPE
{
    CHESS_SPACE = 0,
    CHESS_BLACK,
    CHESS_WHITE
};

enum CHESS_MODE
{
    CHESS_FIVE = 0,
    LIVE_FOUR,
    HALF_FOUR_A,
    HALF_FOUR_B,
    HALF_FOUR_C,
    LIVE_THREE_A,
    LIVE_THREE_B,
    HALF_THREE_A,
    HALF_THREE_B,
    HALF_THREE_C,
    HALF_THREE_D,
    HALF_THREE_E,
    LIVE_TWO_A,
    LIVE_TWO_B,
    LIVE_TWO_C,
    HALF_TWO,
    LIVE_ONE,
    HALF_ONE,
    CHESS_MODE_NUM
};

struct CHESS_POINT
{
    int x;
    int y;
    int type;
};

struct VALUE_POINT
{
    int ref;
    CHESS_POINT* point;
};

struct CHESS_LINE
{
    int point[15];
    int mode[2][CHESS_MODE_NUM];
};

class Engine
{
public:
    Engine(CDialog* dialog);
    ~Engine(void);

    void Init();
    void StartGame();

    BOOL UserDown(int x, int y);

    BOOL AIDown(int x, int y);

    vector<CHESS_POINT*> getChessList() {return m_chessList;}

    BOOL isFive(int x, int y);

    int GetStatus() const {return m_gameState;}

    int GetWiner() const {return m_winer;};

    void Analyse( CHESS_LINE &chessLine, int type, BOOL bTurn);

    void Thinking();

    void TurnAI();

    BOOL Retract();

private:
    void InitChessLine();
    void AddChess(int x, int y, int type);
    void RemoveChess(int x, int y);
    void ChessChange(int x, int y, int type);

    void AddSpace(int x, int y);

    void RemoveSpace(int x, int y);

    int CalculateStore(int turnType);

    void GetSpaceList(list<int> &spaceList);

    void CheckPoint(int x, int y, map<int, CHESS_POINT*> &stateMap, list<CHESS_POINT*> &spaceList);

    int AlphaBeta(int alpha, int beta, int step); 

    int GetValue(int x, int y, int type);

    int GetSpaceValue(int x, int y, int type);

    bool IsPointType(int x, int y, int type);

    void FirstChess();
    
private:
    int m_gameState;
    int m_userType;
    int m_AIType;
    int m_winer;
    VALUE_POINT m_spaceTable[15][15]; //ÆåÅÌ×´Ì¬
    CHESS_POINT m_chessTable[15][15]; //Æå×Ó×´Ì¬
    CHESS_LINE m_chessLine[4][29]; //
    vector<CHESS_POINT*> m_chessList; //Æå×ÓÁ´±í
    map<int, VALUE_POINT*> m_spaceList; //¿ÕµãÁ´±í

    ThreadAI m_threadAI;
    CDialog* m_dialog;
};
#endif