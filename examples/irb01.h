#pragma once
#include"MJCards.h"
#include"Player.h"
#include <utility>
#include <functional>
#include<vector>

struct Tiles_
{
	int tiles[4];
	Tiles_ operator-=(const Tiles_ & t);
};

struct CandidateThrow {
	int stepToHu;
	int numOfImprove;
	//int t2;
	int color;
	int value;
};

typedef struct
{
	vector<int> code;
	vector<int> Hu_Tiles;
	int steps;
}Groups;


class Irb01 : public Player {
public:
	Irb01();
	MJCard Throw(const MJCard & card = MJCard(-100, -100));
	
	std::pair<CommandType, MJCard> WannaHuGon(bool canHu, bool canGon, const MJCard & card, const MJCard & gonCard);
	CommandType WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard & card, int idx);
	int Pick2Eat(const MJCard & card);
private:
	int StepToHu1617(const Tiles_ & Hand);
	vector<int> GetComponent(Tiles_ T);
	Tiles_ HandToTiles(const MJCards & cards);
	Tiles_ HandToTiles(const MJCards & cards, const MJCard & card);
	void T1(Tiles_ & Hand, int stepToHu, vector<CandidateThrow> & Candidates);
	void T2(Tiles_ & Hand, const Tiles_ & remaindTiles, int stepToHu, vector<CandidateThrow> & Candidates, vector<CandidateThrow> & Candidates2);
	MJCard TileToCard(int color, int value);
	int NumOfImprove(Tiles_ & Hand, const Tiles_ & remainTiles, int stepToHu);
	void FindGroups(Tiles_ T, const vector<int>& Componet, vector<Groups>& result, vector<int> tmp, int start);
	vector<Groups> CountSteps(vector<Groups>& G);
	bool Have(int t, int m);

	
};