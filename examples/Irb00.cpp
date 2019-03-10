#include"Irb00.h"

static const int GroupMask[40] = { 02,020,0200,02000,020000,0200000,02000000,020000000,0200000000,\
03,030,0300,03000,030000,0300000,03000000,030000000,0300000000,\
0111,01110,011100,0111000,01110000,011100000,0111000000,\
011,0110,01100,011000,0110000,01100000,011000000,0110000000,\
0101,01010,010100,0101000,01010000,010100000,0101000000 };





Irb00::Tiles_ Irb00::Tiles_::operator-=(const Tiles_ & t) {
	this->tiles[0] -= t.tiles[0];
	this->tiles[1] -= t.tiles[1];
	this->tiles[2] -= t.tiles[2];
	this->tiles[3] -= t.tiles[3];

	return (*this);
}

Irb00::Tiles_ Irb00::Tiles_::operator+=(const MJCard & card) {
	
	if (card.color >= 0 && card.color <= 2) {
		this->tiles[card.color] += 1 << (card.value * 3);
	}
	else if (card.color == 3) {
		this->tiles[3] += 1 << (card.value * 3 + 12);

	}
	else if (card.color == 4) {
		this->tiles[3] += 1 << (card.value * 3);

	}
	else {
		cout << "operator+= error" << endl;
	}

	return (*this);
}

Irb00::Tiles_ Irb00::Tiles_::operator-=(const MJCard & card) {
	if (card.color >= 0 && card.color <= 2) {
		this->tiles[card.color] -= 1 << (card.value * 3);
	}
	else if (card.color == 3) {
		this->tiles[3] -= 1 << (card.value * 3 + 12);

	}
	else if (card.color == 4) {
		this->tiles[3] -= 1 << (card.value * 3);

	}
	else {
		cout << "operator-= error" << endl;
	}

	return (*this);
}




Irb00::Irb00() {

}

MJCard Irb00::Throw(const MJCard & card) {

	Tiles_ Hand_;

	if (card.color == -100) {
		Hand_ = HandToTiles(Hand);
	}
	else {
		Hand_ = HandToTiles(Hand, card);
	}


	int stepToHu = StepToHu1617(Hand_);

	vector<CandidateThrow> Candidates;

	T1(Hand_, stepToHu, Candidates);

	if (Candidates.size() == 1) {
		cout << Print() << " throw " << Candidates[0].color << " " << Candidates[0].value << " draw " << card.color << " " << card.value << endl;
		return TileToCard(Candidates[0].color, Candidates[0].value);
	}

	vector<CandidateThrow> Candidates2;

	Tiles_ remainTiles;

	remainTiles.tiles[0] = 0444444444;
	remainTiles.tiles[1] = 0444444444;
	remainTiles.tiles[2] = 0444444444;
	remainTiles.tiles[3] = 04444444;

	remainTiles -= Hand_;
	remainTiles -= HandToTiles(discards);

	T2(Hand_, remainTiles, stepToHu, Candidates, Candidates2);

	cout << Print() << " throw " << Candidates2[0].color << " " << Candidates2[0].value << " draw " << card.color << " " << card.value << endl;

	if (Candidates2.size() < 20) {
		return TileToCard(Candidates2[0].color, Candidates2[0].value);
	}

	return MJCard(0, 0);
}

std::pair<CommandType, MJCard> Irb00::WannaHuGon(bool canHu, bool canGon, const MJCard & card, const MJCard & gonCard) {
	if (canHu)
		return std::make_pair(COMMAND_ZIMO, card);
	else if (canGon) {
		
		std::pair<bool, MJCard> temp = GonOrNot(card, true); 

		//槓會讓進廳數增加
		if (!temp.first) {
			return std::make_pair(NONE, Throw(card));
		}
		else {
			Hand += card;
			if (Hand.Count(temp.second) == 1) {
				Hand -= card;
				return std::make_pair(COMMAND_PONGON, temp.second);
			}
			else if (Hand.Count(temp.second) == 4) {
				Hand -= card;
				return std::make_pair(COMMAND_ANGON, temp.second);
			}
			else {
				cout << "WannaHuGon error" << endl;
			}
			
		}
		
	}		
	else
		return std::make_pair(NONE, Throw(card));
}
CommandType Irb00::WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard & card, int idx) {
	if (canHu) {
		return COMMAND_HU;
	}
	else if (canPon) {
		bool pon = PonOrNot(card);
		if (pon) {
			return COMMAND_PON;
		}
		
	}
	else if (canEat) {
		std::pair<bool, int> eat = EatOrNot(card);
		if (eat.first) {
			pickToEat = eat.second;
			return COMMAND_EAT;
		}
	}
	else if (canGon) {
		return NONE;
	}
	else {
		cout << "WannaHGPE error" << endl;
		return NONE;
	}

	return NONE;
}

int Irb00::Pick2Eat(const MJCard & card) {
	return pickToEat;
}

int Irb00::StepToHu1617(const Tiles_ & Hand) {
	vector<int> component = GetComponent(Hand);

	vector<Groups> result;
	vector<int> tmp;
	FindGroups(Hand, component, result, tmp, 0);

	vector<Groups> G = CountSteps(result);

	return G[0].steps;
}

vector<int> Irb00::GetComponent(Tiles_ T)
{
	vector<int> ret;
	for (int i = 0; i < 138; i++)
	{
		if (Have(T.tiles[i / 40], GroupMask[i % 40]))
			ret.push_back(i);
	}
	return ret;
}

Irb00::Tiles_ Irb00::HandToTiles(const MJCards & cards) {

	Tiles_ ret;
	ret.tiles[0] = cards.mjcolor[0].value;
	ret.tiles[1] = cards.mjcolor[1].value;
	ret.tiles[2] = cards.mjcolor[2].value;
	ret.tiles[3] = cards.cfbCards << 12;
	ret.tiles[3] += cards.windsCards;

	return ret;

}


Irb00::Tiles_ Irb00::HandToTiles(const MJCards & cards, const MJCard & card) {

	Tiles_ ret;
	ret.tiles[0] = cards.mjcolor[0].value;
	ret.tiles[1] = cards.mjcolor[1].value;
	ret.tiles[2] = cards.mjcolor[2].value;
	ret.tiles[3] = cards.cfbCards << 12;
	ret.tiles[3] += cards.windsCards;

	if (card.color >= 0 && card.color <= 2) {
		ret.tiles[card.color] += 1 << (card.value * 3);
	}
	else if (card.color == 3) {
		ret.tiles[3] += 1 << (card.value * 3 + 12);

	}
	else if (card.color == 4) {
		ret.tiles[3] += 1 << (card.value * 3);

	}
	else {
		cout << "HandToTiles Error" << endl;
	}

	return ret;

}

bool Irb00::Have(int t, int m)
{
	for (int i = 0; i < 9; i++)
		if (((t >> i * 3) & 7) < ((m >> i * 3) & 7))
			return false;
	return true;
}

void Irb00::T1(Tiles_ & Hand, int stepToHu, vector<CandidateThrow> & Candidates) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 9; j++) {
			if (i == 3 && j >= 7) {
				break;
			}
			if ((Hand.tiles[i] >> (j * 3)) & 7) {

				Hand.tiles[i] -= 1 << (j * 3);
				vector<int> component = GetComponent(Hand);

				vector<Groups> result;
				vector<int> tmp;
				FindGroups(Hand, component, result, tmp, 0);

				vector<Groups> G = CountSteps(result);
				if (G[0].steps == stepToHu) {
					CandidateThrow temp = { stepToHu, 0, i, j };
					Candidates.push_back(temp);
				}


				Hand.tiles[i] += 1 << (j * 3);
			}
		}
	}

}

MJCard Irb00::TileToCard(int color, int value) {
	int rColor, rValue;
	if (color == 3) {
		if (value <= 3) {
			rColor = 4;
			rValue = value;
		}
		else {
			rColor = 3;
			rValue = value - 4;
		}
	}
	else {
		rColor = color;
		rValue = value;
	}

	return MJCard(rColor, rValue);
}

void Irb00::T2(Tiles_ & Hand, const Tiles_ & remaindTiles, int stepToHu, vector<CandidateThrow> & Candidates, vector<CandidateThrow> & Candidates2) {
	int max = 0;
	for (int i = 0; i < Candidates.size(); i++) {
		Hand.tiles[Candidates[i].color] -= 1 << (Candidates[i].value * 3);
		int temp = NumOfImprove(Hand, remaindTiles, stepToHu);
		Candidates[i].numOfImprove = temp;
		if (temp > max) {
			max = temp;
		}

		Hand.tiles[Candidates[i].color] += 1 << (Candidates[i].value * 3);
	}
	for (int i = 0; i < Candidates.size(); i++) {
		if (Candidates[i].numOfImprove == max) {
			Candidates2.push_back(Candidates[i]);
		}
	}
}

int Irb00::NumOfImprove(Tiles_ & Hand, const Tiles_ & remainTiles, int stepToHu) {
	int ans = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 9; j++) {
			if (i == 3 && j >= 7) {
				break;
			}
			if ((remainTiles.tiles[i] >> (j * 3)) & 7) {

				Hand.tiles[i] += 1 << (j * 3);
				int temp = StepToHu1617(Hand);
				if (temp == stepToHu - 1) {
					ans += (remainTiles.tiles[i] >> (j * 3)) & 7;
				}
				Hand.tiles[i] -= 1 << (j * 3);
			}
		}
	}
	return ans;
}

void Irb00::FindGroups(Tiles_ T, const vector<int>& Componet, vector<Groups>& result, vector<int> tmp, int start) {
	bool f = true;
	for (int i = start; i < Componet.size(); i++)
	{
		if (Have(T.tiles[Componet[i] / 40], GroupMask[Componet[i] % 40]))
		{
			f = false;
			Tiles_ t = T;
			t.tiles[Componet[i] / 40] -= GroupMask[Componet[i] % 40];
			tmp.push_back(Componet[i]);
			FindGroups(t, Componet, result, tmp, i);
			tmp.pop_back();
		}
	}

	if (f)
	{
		for (int i = 0; i < 34; i++)
		{
			if ((T.tiles[i / 9] >> (i % 9 * 3)) & 7)
			{
				for (int j = 0; j < ((T.tiles[i / 9] >> (i % 9 * 3)) & 7); j++)
					tmp.push_back(180 + i);
			}
		}
		Groups G;
		G.code = tmp;
		result.push_back(G);
	}
}

vector<Irb00::Groups> Irb00::CountSteps(vector<Groups>& G) {
	vector<Groups> ret;

	int min = 100;
	int total, single, pair, group, pre_group, steps;
	for (int i = 0; i < G.size(); i++)
	{
		single = 0, pair = 0, group = 0, pre_group = 0;
		for (int j = 0; j < G[i].code.size(); j++)
		{
			if (G[i].code[j] >= 180)
				single++;
			else if (G[i].code[j] % 40 < 9)
				pair++;
			else if (G[i].code[j] % 40 < 25)
				group++;
			else
				pre_group++;
		}
		total = group * 3 + pre_group * 2 + pair * 2 + single;
		steps = total / 3 * 2 + 1;
		steps -= group * 2 + pre_group + pair;

		if (group + pre_group + pair > total / 3 + 1)
			steps += group + pre_group + pair - (total / 3 + 1);

		if (pre_group != 0 && pre_group >= (total / 3 + 1 - group) && !pair)
			steps += 1;

		if (steps < min)
			min = steps;

		G[i].steps = steps;
	}

	for (int i = 0; i < G.size(); i++)
	{
		if (G[i].steps <= min + 0)
			ret.push_back(G[i]);
	}

	return ret;
}

std::pair<bool, MJCard> Irb00::GonOrNot(const MJCard & card, bool zimo) {
	vector<MJCard> gonArr;
	Tiles_ Hand_ = HandToTiles(Hand, card);
	int tin = StepToHu1617(Hand_);

	if (!zimo) {
		gonArr.push_back(card);
		cout << "GonOrNot error" << endl;
	}
	else {
		gonArr = AllYouCanGon(card);
	}

	if (zimo) {
		Hand += card;

		for (int i = 0; i < gonArr.size(); i++) {
			if (Hand.Count(gonArr[i]) == 1) {
				Hand_ -= gonArr[i];
				if (StepToHu1617(Hand_) == tin) {
					Hand -= card;
					return std::make_pair(true, gonArr[i]);
				}
				Hand_ += gonArr[i];
			}
			else if (Hand.Count(gonArr[i]) == 4) {
				Hand_ -= gonArr[i];
				Hand_ -= gonArr[i];
				Hand_ -= gonArr[i];
				Hand_ -= gonArr[i];
				if (StepToHu1617(Hand_) == tin) {
					Hand -= card;
					return std::make_pair(true, gonArr[i]);
				}
				Hand_ += gonArr[i];
				Hand_ += gonArr[i];
				Hand_ += gonArr[i];
				Hand_ += gonArr[i];
			}
			else {
				cout << "GonOrNot error" << endl;
			}
		}
		Hand -= card;
		return std::make_pair(false, MJCard(-10, -10));
	}
	else {
		//我目前在別人丟牌時一律不槓
		cout << "GonOrNot error" << endl;
	}
}

vector<MJCard> Irb00::AllYouCanGon(const MJCard & card) {
	vector<MJCard> ret;


	//槓摸進來的那張
	if (Hand.Count(card) == 3) {
		ret.push_back(card);
	}
	else if (pon & CardTo34(card)) {
		ret.push_back(card);
	}

	//延遲槓

	//萬筒條
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (Hand.Count(MJCard(i, j)) == 4) {
				ret.push_back(MJCard(i, j));
			}
			else if (Hand.Count(MJCard(i, j)) == 1 && (pon & CardTo34(MJCard(i, j)))) {
				ret.push_back(MJCard(i, j));
			}
		}
	}

	//中發白
	for (int i = 0; i < 3; i++) {
		if (Hand.Count(MJCard(3, i)) == 4) {
			ret.push_back(MJCard(3, i));
		}
		else if (Hand.Count(MJCard(3, i)) == 1 && (pon & CardTo34(MJCard(3, i)))) {
			ret.push_back(MJCard(3, i));
		}
	}

	//風牌
	for (int i = 0; i < 4; i++) {
		if (Hand.Count(MJCard(4, i)) == 4) {
			ret.push_back(MJCard(4, i));
		}
		else if (Hand.Count(MJCard(4, i)) == 1 && (pon & CardTo34(MJCard(4, i)))) {
			ret.push_back(MJCard(4, i));
		}
	}

	return ret;
}

long long Irb00::CardTo34(const MJCard & card) {
	long long result = 1;
	switch (card.color) {
	case 0:
		result <<= card.value;
		return result;
	case 1:
		result <<= (9 + card.value);
		return result;
	case 2:
		result <<= (18 + card.value);
		return result;
	case 3:
		result <<= (27 + card.value);
		return result;
	case 4:
		result <<= (30 + card.value);
		return result;
	default:
		cout << "CardTo34 error" << endl;
		return -1;
	}
}

bool Irb00::PonOrNot(const MJCard & card) {
	Tiles_ Hand_ = HandToTiles(Hand);
	int tin = StepToHu1617(Hand_);

	Hand_ -= card;
	Hand_ -= card;

	if (StepToHu1617(Hand_) == tin - 1) {
		return true;
	}
	else {
		return false;
	}

}

std::pair<bool, int> Irb00::EatOrNot(const MJCard & card) {
	Tiles_ Hand_ = HandToTiles(Hand);
	int tin = StepToHu1617(Hand_);

	if (card.color > 2 || card.color == -1)
	{
		cout << "EatOrNot error" << endl;
		return std::make_pair(false, -1);
	}

	//看是否是 1 9
	if (card.value == 0) {
		Hand_ -= MJCard(card.color, card.value + 1);
		Hand_ -= MJCard(card.color, card.value + 2);

		if (StepToHu1617(Hand_) == tin - 1) {
			return std::make_pair(true, 2);
		}

		
	}
	else if (card.value == 8) {
		Hand_ -= MJCard(card.color, card.value - 1);
		Hand_ -= MJCard(card.color, card.value - 2);

		if (StepToHu1617(Hand_) == tin - 1) {
			return std::make_pair(true, 0);
		}

				
	}

	//2 8
	else if (card.value == 1) {
		if (Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && Hand.Count(MJCard(card.color, card.value + 1)) >= 1) {
			Hand_ -= MJCard(card.color, card.value - 1);
			Hand_ -= MJCard(card.color, card.value + 1);
			if (StepToHu1617(Hand_) == tin - 1) {
				return std::make_pair(true, 1);
			}
			Hand_ += MJCard(card.color, card.value - 1);
			Hand_ += MJCard(card.color, card.value + 1);
		}
		if (Hand.Count(MJCard(card.color, card.value + 2)) >= 1 && Hand.Count(MJCard(card.color, card.value + 1)) >= 1) {
			Hand_ -= MJCard(card.color, card.value + 2);
			Hand_ -= MJCard(card.color, card.value + 1);
			if (StepToHu1617(Hand_) == tin - 1) {
				return std::make_pair(true, 2);
			}
			Hand_ += MJCard(card.color, card.value + 2);
			Hand_ += MJCard(card.color, card.value + 1);
		}
		
			
	}
	else if (card.value == 7) {

		if (Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && Hand.Count(MJCard(card.color, card.value + 1)) >= 1) {
			Hand_ -= MJCard(card.color, card.value - 1);
			Hand_ -= MJCard(card.color, card.value + 1);
			if (StepToHu1617(Hand_) == tin - 1) {
				return std::make_pair(true, 1);
			}
			Hand_ += MJCard(card.color, card.value - 1);
			Hand_ += MJCard(card.color, card.value + 1);
		}
		if (Hand.Count(MJCard(card.color, card.value - 2)) >= 1 && Hand.Count(MJCard(card.color, card.value - 1)) >= 1) {
			Hand_ -= MJCard(card.color, card.value - 2);
			Hand_ -= MJCard(card.color, card.value - 1);
			if (StepToHu1617(Hand_) == tin - 1) {
				return std::make_pair(true, 0);
			}
			Hand_ += MJCard(card.color, card.value - 2);
			Hand_ += MJCard(card.color, card.value - 1);
		}

		
	}


	else {
		if (Hand.Count(MJCard(card.color, card.value - 1)) >= 1 && Hand.Count(MJCard(card.color, card.value + 1)) >= 1) {
			Hand_ -= MJCard(card.color, card.value - 1);
			Hand_ -= MJCard(card.color, card.value + 1);
			if (StepToHu1617(Hand_) == tin - 1) {
				return std::make_pair(true, 1);
			}
			Hand_ += MJCard(card.color, card.value - 1);
			Hand_ += MJCard(card.color, card.value + 1);
		}
		if (Hand.Count(MJCard(card.color, card.value + 2)) >= 1 && Hand.Count(MJCard(card.color, card.value + 1)) >= 1) {
			Hand_ -= MJCard(card.color, card.value + 2);
			Hand_ -= MJCard(card.color, card.value + 1);
			if (StepToHu1617(Hand_) == tin - 1) {
				return std::make_pair(true, 2);
			}
			Hand_ += MJCard(card.color, card.value + 2);
			Hand_ += MJCard(card.color, card.value + 1);
		}

		if (Hand.Count(MJCard(card.color, card.value - 2)) >= 1 && Hand.Count(MJCard(card.color, card.value - 1)) >= 1) {
			Hand_ -= MJCard(card.color, card.value - 2);
			Hand_ -= MJCard(card.color, card.value - 1);
			if (StepToHu1617(Hand_) == tin - 1) {
				return std::make_pair(true, 0);
			}
			Hand_ += MJCard(card.color, card.value - 2);
			Hand_ += MJCard(card.color, card.value - 1);
		}

		
	}

	return std::make_pair(false, -1);
}