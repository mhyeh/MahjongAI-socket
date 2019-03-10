#ifndef REV2_MJCARDMANAGER_H
#define REV2_MJCARDMANAGER_H

#include <iostream>
#include <vector>

using namespace std;

#define T1_WEIGHT 20 
#define T2_WEIGHT 5
#define VALUE_WEIGHT 1
#define VALUE_FIRST_X 3
//避免期望值計算為0
#define VALUE_BASE 0.5  
//忽略獨聽, 平胡
//對對胡, 混一色, 清一色, 三暗刻, 四暗刻, 五暗刻, 中發白, 大四喜, 小四喜, 大三元, 小三元, 圈風台, 門風台, 字一色
#define FAN_WEIGHT {4, 4, 8, 2, 5, 8, 1, 16, 8, 8, 4, 1, 1, 16}

typedef struct element
{
	int type; //0:刻子, 1:順子, 2:一對, 3:小順, 4:中洞, 5:單張
	vector<int> card;
}group;

typedef struct trans
{
	int leftCard;
	int type;
	//21 一對->一對
	//22 一對->刻子
	//31 小順->順子
	//32 小順->順子
	//41 中洞->順子
	//51 單張->順子
	//52 單張->順子
	//53 單張->順子
	//54 單張->刻子
	//55 單張->一對
	int len;
}changeNode;

vector< group > getElement(vector<int> hand);
vector<double> throwOneCard_v2(vector<int> hand, vector<int> deck, vector<int> door, int totalDeck, int handCount, vector<int> sea, int canPonPon, int position, int round);
vector<double> calcScore(vector<int>* record, vector<int> hand, vector<int> deck, vector<int> door, int handCount, int minStep, int countTi, int totalDeck);
double calcTi(vector<int> hand, vector<int> door);

void countTing(int remain, int nowPos, vector< group > combine, vector<int> usedHand, const vector<int>& hand, int *  min);

void estimute_v2(vector< group > combine, vector<int> deck, int totalDeck, vector<double> * totalValue, vector<int> door);
void returnAllCombineProp_v2(int nowPos, vector< vector<changeNode> > changePattern, int once, vector<int> nowHand, vector<double> * totalValue, vector<int> deck, int totalDeck, vector<int> door);

#endif // REV2_MJCARDMANAGER_H