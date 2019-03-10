#include "Rev2_Player.h"

int Rev2_Player::eatCommand = 3;

Rev2_Player::Rev2_Player()
{

}

void Rev2_Player::dataSet()
{
	hand.clear();
	deck.clear();
	door.clear();
	sea.clear();
	totalDeck = 64;
	canPonPon = 1;
	nowHand = 0;
	roundWind = -1;
	positionWind = -1;

	//-----------Set------------
	for (int i = 0; i < 34; i = i + 1)
	{
		deck.push_back(4);
		door.push_back(0);
	}

	for (int i = 0; i < 3; i = i + 1)
	{
		for (int j = 0; j < 9; j = j + 1)
		{
			hand.push_back(Hand.mjcolor[i][j]);
			nowHand = nowHand + Hand.mjcolor[i][j];
			sea.push_back(discards.mjcolor[i][j]);
			deck[i * 9 + j] = deck[i * 9 + j] - discards.mjcolor[i][j] - Hand.mjcolor[i][j];

			if ((pon >> (i * 9 + j)) & 1 == 0)
			{
				door[i * 9 + j] = door[i * 9 + j] + 3;
				sea[i * 9 + j] = sea[i * 9 + j] + 3;
			}

			if ((anGon >> (i * 9 + j)) & 1 == 0)
			{
				door[i * 9 + j] = -4;
			}

			if ((gon >> (i * 9 + j)) & 1 == 0)
			{
				door[i * 9 + j] = 4;
				sea[i * 9 + j] = sea[i * 9 + j] + 4;
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{
		int amount = (discards.windsCards >> (i * 3)) & 7;
		sea.push_back(amount);
		deck[27 + i] = deck[27 + i] - amount;

		amount = (Hand.windsCards >> (i * 3)) & 7;
		hand.push_back(amount);
		nowHand = nowHand + amount;
		deck[27 + i] = deck[27 + i] - amount;

		if ((pon >> (27 + i)) & 1 == 0)
		{
			door[27 + i] = door[27 + i] + 3;
			sea[27 + i] = sea[27 + i] + 3;
		}

		if ((anGon >> (27 + i)) & 1 == 0)
		{
			door[27 + i] = -4;
		}

		if ((gon >> (27 + i)) & 1 == 0)
		{
			door[27 + i] = 4;
			sea[27 + i] = sea[27 + i] + 4;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		int amount = (discards.cfbCards >> (i * 3)) & 7;
		sea.push_back(amount);
		deck[31 + i] = deck[31 + i] - amount;

		amount = (Hand.cfbCards >> (i * 3)) & 7;
		hand.push_back(amount);
		nowHand = nowHand + amount;
		deck[31 + i] = deck[31 + i] - amount;

		if ((pon >> (31 + i)) & 1 == 0)
		{
			door[31 + i] = door[31 + i] + 3;
			sea[31 + i] = sea[31 + i] + 3;
		}

		if ((anGon >> (31 + i)) & 1 == 0)
		{
			door[31 + i] = -4;
		}

		if ((gon >> (31 + i)) & 1 == 0)
		{
			door[31 + i] = 4;
			sea[31 + i] = sea[31 + i] + 4;
		}
	}

	for (int i = 0; i < 3; i = i + 1)
	{
		for (int j = 0; j < 7; j = j + 1)
		{
			int amount = (eat >> (i * 9 + j) * 3) & 7;
			for (int k = 0; k < amount; k = k + 1)
			{
				canPonPon = 0;
				door[i * 9 + j] = door[i * 9 + j] + 1;
				door[i * 9 + j + 1] = door[i * 9 + j + 1] + 1;
				door[i * 9 + j + 2] = door[i * 9 + j + 2] + 1;

				sea[i * 9 + j] = sea[i * 9 + j] + 1;
				sea[i * 9 + j + 1] = sea[i * 9 + j + 1] + 1;
				sea[i * 9 + j + 2] = sea[i * 9 + j + 2] + 1;
			}
		}
	}

	//totalDeck = GameManager::gameManager->Sea.Count();
	totalDeck = 64;

	//-----------Set------------
}

MJCard Rev2_Player::Throw() 
{
	return Throw(MJCard(-100, -100));
}

MJCard Rev2_Player::Throw(const MJCard & card = MJCard(-100, -100))
{
	dataSet();
	switch (card.color)
	{
	case -100: //吃或碰之後
		break;
	case 3://中發白
		hand[31 + card.value] = hand[31 + card.value] + 1;
		deck[31 + card.value] = deck[31 + card.value] - 1;
		break;
	case 4://東南西北
		hand[27 + card.value] = hand[27 + card.value] + 1;
		deck[27 + card.value] = deck[27 + card.value] - 1;
		break;
	default:
		hand[card.color * 9 + card.value] = hand[card.color * 9 + card.value] + 1;
		deck[card.color * 9 + card.value] = deck[card.color * 9 + card.value] - 1;
		break;
	}
	//-----------Test------------
	
	cout << "Throw: " << endl;

	for (int i = 0; i < 3; i = i + 1)
	{
		for (int j = 0; j < 9; j = j + 1)
		{
			cout << hand[i * 9 + j] << " ";
		}
		cout << endl;
	}
	for (int i = 0; i < 7; i = i + 1)
	{
		cout << hand[27 + i] << " ";
	}
	cout << endl;
	
	
	for (int i = 0; i < 3; i = i + 1)
	{
		for (int j = 0; j < 9; j = j + 1)
		{
			cout << door[i * 9 + j] << " ";
		}
		cout << endl;
	}
	for (int i = 0; i < 7; i = i + 1)
	{
		cout << door[27 + i] << " ";
	}
	cout << endl;

	//-----------Test------------

	vector<double> resultValue = throwOneCard_v2(hand, deck, door, totalDeck, nowHand, sea, canPonPon, positionWind, roundWind);
	
	int color = 0; 
	int value = 0;
	switch ((int)resultValue[0] / 9)
	{	
	case 3: 
		if ((int)resultValue[0] % 9 < 4) //東南西北
		{
			color = 4;
			value = (int)resultValue[0] % 9;
		}
		else //中發白
		{
			color = 3;
			value = (int)resultValue[0] % 9 - 4;
		}
		break;
	default:
		color = resultValue[0] / 9;
		value = (int)resultValue[0] % 9;
		break;
	}

	return MJCard(color, value);
}

std::pair<CommandType, MJCard> Rev2_Player::WannaHuGon(bool canHu, bool canGon, const MJCard & card, const MJCard & gonCard)
{
	cout << "WannaHuGon->" << endl;
	if (canHu)
		return std::make_pair(COMMAND_ZIMO, card);
	else if (canGon)
	{
		int cardPos;
		dataSet();
		switch (gonCard.color)
		{
		case 3://中發白
			cardPos = 31 + gonCard.value;
			break;
		case 4://東南西北
			cardPos = 27 + gonCard.value;
			break;
		default:
			cardPos = gonCard.color * 9 + gonCard.value;
			break;
		}

		hand[cardPos] = hand[cardPos] + 1;
		nowHand = nowHand + 1;

		double minStep = 17;
		vector<int> scoreTemp;
		for (int i = 0; i < hand.size(); i = i + 1)
		{
			scoreTemp.push_back(0);
		}
		vector<double> S1 = calcScore(&scoreTemp, hand, deck, door, nowHand, minStep, 0, totalDeck);

		int before = S1[0];

		//--------------------------------------------------------

		if (hand[cardPos] == 4) //暗槓
		{
			hand[cardPos] = 0;
			door[cardPos] = -4;
			nowHand = nowHand - 4;
		}
		else //碰槓
		{
			hand[cardPos] = 0;
			door[cardPos] = 4;
			nowHand = nowHand - 1;
		}

		minStep = 17;
		scoreTemp.clear();
		for (int i = 0; i < hand.size(); i = i + 1)
		{
			scoreTemp.push_back(0);
		}
		S1 = calcScore(&scoreTemp, hand, deck, door, nowHand, minStep, 0, totalDeck);

		//--------------------------------------------------------

		if (S1[0] < before) return std::make_pair(COMMAND_GON, gonCard);
		else return std::make_pair(NONE, Throw(card));
	}		
	else
		return std::make_pair(NONE, Throw(card));
}

CommandType Rev2_Player::WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard & card, int idx)
{
	cout << "WannaHGPE->" << endl;
	if (canHu) {
		return COMMAND_HU;
	}
	
	if (canGon) {
		int cardPos;
		dataSet();
		switch (card.color)
		{
		case 3://中發白
			cardPos = 31 + card.value;
			break;
		case 4://東南西北
			cardPos = 27 + card.value;
			break;
		default:
			cardPos = card.color * 9 + card.value;
			break;
		}

		double minStep = 17;
		vector<int> scoreTemp;
		for (int i = 0; i < hand.size(); i = i + 1)
		{
			scoreTemp.push_back(0);
		}
		vector<double> S1 = calcScore(&scoreTemp, hand, deck, door, nowHand, minStep, 0, totalDeck);

		int before = S1[0];

		//--------------------------------------------------------

		//直槓
		hand[cardPos] = 0;
		door[cardPos] = 4;
		nowHand = nowHand - 3;

		minStep = 17;
		scoreTemp.clear();
		for (int i = 0; i < hand.size(); i = i + 1)
		{
			scoreTemp.push_back(0);
		}
		S1 = calcScore(&scoreTemp, hand, deck, door, nowHand, minStep, 0, totalDeck);

		//--------------------------------------------------------

		if (S1[0] < before) return NONE;
		return COMMAND_GON;
	}
	
	if (canPon) {

		int cardPos;
		dataSet();
		switch (card.color)
		{
		case 3://中發白
			cardPos = 31 + card.value;
			break;
		case 4://東南西北
			cardPos = 27 + card.value;
			break;
		default:
			cardPos = card.color * 9 + card.value;
			break;
		}

		double minStep = 17;
		vector<int> scoreTemp;
		for (int i = 0; i < hand.size(); i = i + 1)
		{
			scoreTemp.push_back(0);
		}
		vector<double> S1 = calcScore(&scoreTemp, hand, deck, door, nowHand, minStep, 0, totalDeck);

		int before = S1[0];

		//--------------------------------------------------------

		//碰
		hand[cardPos] = hand[cardPos] - 2;
		door[cardPos] = 3;
		nowHand = nowHand - 2;

		minStep = 17;
		scoreTemp.clear();
		for (int i = 0; i < hand.size(); i = i + 1)
		{
			scoreTemp.push_back(0);
		}
		S1 = calcScore(&scoreTemp, hand, deck, door, nowHand, minStep, 0, totalDeck);

		//--------------------------------------------------------

		return COMMAND_PON;
	}
	
	if (canEat) {

		Rev2_Player::eatCommand = 3; //不吃

		int cardPos;
		dataSet();
		switch (card.color)
		{
		case 3://中發白
			cardPos = 31 + card.value;
			break;
		case 4://東南西北
			cardPos = 27 + card.value;
			break;
		default:
			cardPos = card.color * 9 + card.value;
			break;
		}

		double minStep = 17;
		vector<int> scoreTemp;
		for (int i = 0; i < hand.size(); i = i + 1)
		{
			scoreTemp.push_back(0);
		}
		vector<double> S1 = calcScore(&scoreTemp, hand, deck, door, nowHand, minStep, 0, totalDeck);

		int before = S1[0];
		int beforePonPon = canPonPon;

		//--------------------------------------------------------
		if (cardPos < 27 && cardPos % 9 >= 2 && hand[cardPos - 2] != 0 && hand[cardPos - 1] != 0) //1 2 x
		{
			hand[cardPos - 2] = hand[cardPos - 2] - 1;
			hand[cardPos - 1] = hand[cardPos - 1] - 1;
			
			door[cardPos - 2] = door[cardPos - 2] + 1;
			door[cardPos - 1] = door[cardPos - 1] + 1;
			door[cardPos] = door[cardPos] + 1;

			nowHand = nowHand - 2;
			canPonPon = 0;

			minStep = 17;
			scoreTemp.clear();
			for (int i = 0; i < hand.size(); i = i + 1)
			{
				scoreTemp.push_back(0);
			}
			S1 = calcScore(&scoreTemp, hand, deck, door, nowHand, minStep, 0, totalDeck);

			if (S1[0] < before)
			{
				Rev2_Player::eatCommand = 0;
				return COMMAND_EAT;
			}
			else
			{
				hand[cardPos - 2] = hand[cardPos - 2] + 1;
				hand[cardPos - 1] = hand[cardPos - 1] + 1;

				door[cardPos - 2] = door[cardPos - 2] - 1;
				door[cardPos - 1] = door[cardPos - 1] - 1;
				door[cardPos] = door[cardPos] - 1;

				nowHand = nowHand + 2;
				canPonPon = beforePonPon;
			}

		}
		//--------------------------------------------------------
		if (cardPos < 27 && cardPos % 9 >= 1 && cardPos % 9 <= 7 && hand[cardPos - 1] != 0 && hand[cardPos + 1] != 0) //2 x 4
		{
			hand[cardPos - 1] = hand[cardPos - 1] - 1;
			hand[cardPos + 1] = hand[cardPos + 1] - 1;

			door[cardPos - 1] = door[cardPos - 1] + 1;
			door[cardPos + 1] = door[cardPos + 1] + 1;
			door[cardPos] = door[cardPos] + 1;

			nowHand = nowHand - 2;
			canPonPon = 0;

			minStep = 17;
			scoreTemp.clear();
			for (int i = 0; i < hand.size(); i = i + 1)
			{
				scoreTemp.push_back(0);
			}
			S1 = calcScore(&scoreTemp, hand, deck, door, nowHand, minStep, 0, totalDeck);

			if (S1[0] < before)
			{
				Rev2_Player::eatCommand = 1;
				return COMMAND_EAT;
			}
			else
			{
				hand[cardPos - 1] = hand[cardPos - 1] + 1;
				hand[cardPos + 1] = hand[cardPos + 1] + 1;

				door[cardPos - 1] = door[cardPos - 1] - 1;
				door[cardPos + 1] = door[cardPos + 1] - 1;
				door[cardPos] = door[cardPos] - 1;

				nowHand = nowHand + 2;
				canPonPon = beforePonPon;
			}
		}
		//--------------------------------------------------------
		if (cardPos < 27 && cardPos % 9 <= 6 && hand[cardPos + 2] != 0 && hand[cardPos + 1] != 0) //x 3 4
		{
			hand[cardPos + 2] = hand[cardPos + 2] - 1;
			hand[cardPos + 1] = hand[cardPos + 1] - 1;

			door[cardPos + 2] = door[cardPos + 2] + 1;
			door[cardPos + 1] = door[cardPos + 1] + 1;
			door[cardPos] = door[cardPos] + 1;

			nowHand = nowHand - 2;
			canPonPon = 0;

			minStep = 17;
			scoreTemp.clear();
			for (int i = 0; i < hand.size(); i = i + 1)
			{
				scoreTemp.push_back(0);
			}
			S1 = calcScore(&scoreTemp, hand, deck, door, nowHand, minStep, 0, totalDeck);

			if (S1[0] < before)
			{
				Rev2_Player::eatCommand = 2;
				return COMMAND_EAT;
			}
			else
			{
				hand[cardPos + 2] = hand[cardPos + 2] + 1;
				hand[cardPos + 1] = hand[cardPos + 1] + 1;
							
				door[cardPos + 2] = door[cardPos + 2] - 1;
				door[cardPos + 1] = door[cardPos + 1] - 1;
				door[cardPos] = door[cardPos] - 1;

				nowHand = nowHand + 2;
				canPonPon = beforePonPon;
			}
		}
		//--------------------------------------------------------

		return NONE;
	}
	else {
		cout << "WannaHGPE error" << endl;
		return NONE;
	}
}

int Rev2_Player::Pick2Eat(const MJCard & card)
{
	cout << "Pick2Eat->" << endl;
	return Rev2_Player::eatCommand;
}