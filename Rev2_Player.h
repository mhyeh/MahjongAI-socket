#ifndef REV2_PLAYER_H
#define REV2_PLAYER_H

#include "Player.h"

#include "Rev2_MJCardManager.h"

class Rev2_Player : public Player {

	// 0~8:萬, 9~17:筒, 18~26:條, 27~33:東-南-西-北-中-發-白
	vector<int> hand;
	vector<int> deck;
	//用-4表示暗槓
	vector<int> door;
	vector<int> sea; //不應該包含別人的門前
	int totalDeck;
	int canPonPon;
	int nowHand;
	//無法讀取圈風資訊
	int roundWind;
	//目前尚無門風資訊
	int positionWind;
	
public:
	static int eatCommand;
	void dataSet();
	Rev2_Player();
	MJCard Throw();
	MJCard Throw(const MJCard & card);
	std::pair<CommandType, MJCard> WannaHuGon(bool canHu, bool canGon, const MJCard & card, const MJCard & gonCard);
	CommandType WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard & card, int idx);
	bool WannaHu(const MJCard & card) { return true; }

	int Pick2Eat(const MJCard & card);

};


#endif // !REV2_PLAYER_H


