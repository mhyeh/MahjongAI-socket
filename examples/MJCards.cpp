#include"MJCards.h"

#include<iostream>

using namespace std;

static const char* C2W = "ABCDEF";

int MJColor::Count() const{
	int result = 0;

	for (int i = 0; i < 9; i++) {
		result += ((value >> (i * 3)) & 7);
	}

	return result;
}

MJCards::MJCards(bool full) {
	if (full) {
		for (int i = 0; i < 3; i++)
			mjcolor[i] = 0b100100100100100100100100100;
		cfbCards = 0b100100100;
		windsCards = 0b100100100100;
		flowersCards = 0b11111111;
	}
	else {
		for (int i = 0; i < 3; i++)
			mjcolor[i] = 0;
		cfbCards = 0;
		windsCards = 0;
		flowersCards = 0;
	}
		
}

MJCards & MJCards::operator+=(std::vector<MJCard>& cards) {
	for (auto& card : cards) {
		if ((*this).mjcolor[card.color][card.value] < 4) {
			(*this) += card;
		}
	}
	return *this;
}

MJCards & MJCards::operator-=(std::vector<MJCard>& cards) {
	for (auto& card : cards) {
		if ((*this).mjcolor[card.color][card.value] > 0) {
			(*this) -= card;
		}
	}
	return *this;
}

MJCards & MJCards::operator+=(const MJCard& card) {
	if (card.color >= 0 && card.color <= 2) {
		if (mjcolor[card.color][card.value] < 4) {
			mjcolor[card.color] += card.value;
		}
	}
	else if (card.color == 3) {
		cfbCards += (1 << (card.value * 3));
	}
	else if (card.color == 4) {
		windsCards += (1 << (card.value * 3));
	}
	else if (card.color == 5) {
		flowersCards += (1 << card.value);		
	}
	return *this;
}

MJCards & MJCards::operator-=(const MJCard& card) {
	if (card.color >= 0 && card.color <= 2) {
		if (mjcolor[card.color][card.value] > 0) {
			mjcolor[card.color] -= card.value;
		}
	}
	else if (card.color == 3) {
		if ((cfbCards >> (card.value * 3)) & 7) {
			cfbCards -= (1 << (card.value * 3));
		}
		
	}
	else if (card.color == 4) {
		if ((windsCards >> (card.value * 3)) & 7) {
			windsCards -= (1 << (card.value * 3));
		}
		
	}
	else if (card.color == 5) {
		if ((flowersCards >> card.value) & 1) {
			flowersCards -= (1 << card.value);
		}
		
	}
	return *this;
}

MJCards & MJCards::operator+=(const MJCards & cards) {
	for (int i = 0; i < 3; ++i)
		mjcolor[i].value += cards.mjcolor[i].value;
	cfbCards += cards.cfbCards;
	windsCards += cards.windsCards;
	flowersCards += cards.flowersCards;
	return *this;
}

MJCards & MJCards::operator-=(const MJCards & cards) {
	for (int i = 0; i < 3; ++i)
		mjcolor[i].value -= cards.mjcolor[i].value;
	cfbCards -= cards.cfbCards;
	windsCards -= cards.windsCards;
	flowersCards -= cards.flowersCards;
	return *this;
}



bool MJCards::isEmpty() {
	return (mjcolor[0].value + mjcolor[1].value + mjcolor[2].value) == 0;
}

bool MJCards::ContainColor(int c) {
	return (mjcolor[c].Count() > 0);
}

bool MJCards::Have(MJCard card) {
	if (card.color >= 0 && card.color <= 2)
		return (mjcolor[card.color][card.value] > 0);
	else if (card.color == 3)
		return (((cfbCards >> (card.value * 3)) & 7) > 0);
	else if (card.color == 4)
		return(((windsCards >> (card.value * 3)) & 7) > 0);
	else if (card.color == 5)
		return(((flowersCards >> card.value) & 1) > 0);
	else {
		std::cout << "HAVE Error" << std::endl;
		return false;
	}
}

int MJCards::Count() const {
	int result = 0;

	for (int i = 0; i < 3; i++) {
		result += mjcolor[i].Count();
	}

	for (int i = 0; i < 3; i++) {
		result += ((cfbCards >> (i * 3)) & 7);
	}

	for (int i = 0; i < 4; i++) {
		result += ((windsCards >> (i * 3)) & 7);
	}

	
	for (int i = 0; i < 8; i++) {
		result += ((flowersCards >> i) & 1);
	}
	
	return result;
}

int MJCards::CountFlowers() {
	int result = 0;
	for (int i = 0; i < 8; i++) {
		result += ((flowersCards >> i) & 1);
	}
	return result;
}

int MJCards::Count(const MJCard & card) const {
	int ans;
	switch (card.color) {
	case 0:
		return mjcolor[0][card.value];
		break;
	case 1:
		return mjcolor[1][card.value];
		break;
	case 2:
		return mjcolor[2][card.value];
		break;
	case 3:
		ans = (cfbCards >> (card.value * 3)) & 7;
		return ans;
		break;
	case 4:
		ans = (windsCards >> (card.value * 3)) & 7;
		return ans;
		break;
	case 5:
		return (flowersCards >> card.value) & 1;
	default:
		std::cout << "Count Card error" << card.color << std::endl;
		return -1;
	}
}

string MJCards::Print() const {
	
	std::string str = "";
	for (int c = 0; c < 3; c++) {
		for (int v = 0; v < 9; v++) {
			for (int n = mjcolor[c][v]; n > 0; n--) {
				str += to_string(v) + C2W[c] + " ";
			}
		}
		str += "\n,,,,,,,";
	}

	

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < ((cfbCards >> (i * 3)) & 7); j++)
		{
			str += to_string(i) + "D ";
		}
	}
	str += "\n,,,,,,,";

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < ((windsCards >> (i * 3)) & 7); j++)
		{
			str += to_string(i) + "E ";
		}
	}
	str += "\n,,,,,,,";

	for (int i = 0; i < 8; i++)
	{
		if ((flowersCards >> i) & 1)

		{
			str += to_string(i) + "F ";
		}
	}
	return str;
}

MJCard MJCards::at(int idx) const {
	if (idx < 0)
		return MJCard(-21, -21);
	int count = 0;
	for (int c = 0; c < 3; c++) {
		for (int v = 0; v < 9; v++) {
			count += mjcolor[c][v];
			if (count > idx) {
				return MJCard(c, v);
			}
		}
	}

	for (int i = 0; i < 3; i++)
	{
		count += ((cfbCards >> (i * 3)) & 7);
		if (count > idx) {
			return MJCard(3, i);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		count += ((windsCards >> (i * 3)) & 7);
		if (count > idx) {
			return MJCard(4, i);
		}
	}

	for (int i = 0; i < 8; i++)
	{
		count += ((flowersCards >> i) & 1);
		if (count > idx) {
			return MJCard(5, i);
		}
	}
	std::cout << "at error " << count << " " << idx << std::endl;
	return MJCard(-1, -1);
}

MJCard MJCards::Draw() {
	int len = Count();
	if (len == 0)
		len = 1;
	MJCard result = at(rand() % len);
	*this -= result;
	return result;
}

MJCard MJCards::Rand() const {
	int len = Count();
	if (len == 0)
		len = 1;
	MJCard result = at(rand() % len);
	
	return result;
}

unsigned long long MJCards::Translate(int lack) {
	bool first = true;
	unsigned long long result = 0;
	for (int i = 0; i < 3; i++) {
		if (i != lack) {
			result |= mjcolor[i].value;
			if (first) {
				result <<= 27;
				first = false;
			}
		}
	}
	return result;
}

bool MJCard::operator!=(const MJCard& card) const {
	return (value != card.value || color != card.color);
}

bool MJCard::operator==(const MJCard& card) const {
	return (value == card.value && color == card.color);
}