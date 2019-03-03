#pragma once

#include<random>
#include<vector>
#include<fstream>
#include<string>
#include <sstream>
#include <map>

using namespace std;

class MJCard {
public:
	static MJCard StringToCard(string card) {
		map<char, int> m;
		m['c'] = 0;
		m['d'] = 1;
		m['b'] = 2;
		m['o'] = 3;
		m['f'] = 5;
		int color = m[card[0]];
		int value = card[1] - '1';
		if (color == 3 && value < 4) {
			color == 4;
		}
		if (color == 3 && value >= 4) {
			value -= 4;
		}
		return MJCard(color, value);
	}
	int color;
	int value;

	bool operator!=(const MJCard& card) const;
	bool operator==(const MJCard& card) const;
	MJCard(int _c, int _v) : color(_c), value(_v) {}
	MJCard() : color(-1), value(-1) {}
	string toString() {
		string arr[] = { "c", "d", "b", "o", "f" };
		stringstream ss;
		int _color = color;
		int _value = value;
		if (_color == 3) {
			_value += 4;
		} else if (_color == 4) {
			_color = 3;
		}
		else if (_color == 5) {
			_color = 4;
		}
		ss << arr[_color] << _value + 1;
		string str;
		ss >> str;
		return str;
	}
};



struct MJColor {
	int value;

	int operator[](int idx) const {
		return (value >> (idx * 3)) & 7;
	}
	void operator=(int _value) {
		value = _value;
	}

	void operator+=(int _value) {
		value += (1LL << (_value * 3));
	}

	void operator-=(int _value) {
		value -= (1LL << (_value * 3));
	}


	bool have(int _idx) {
		return ((value >> (_idx * 3)) & 7) > 0;
	}

	int Count() const;
	
};

struct MJCards {
	MJColor mjcolor[3];
	int cfbCards; //中發白
	int windsCards; //風牌
	int flowersCards; //花牌
	
	MJCards(bool full = false);
	bool isEmpty();
	bool ContainColor(int c);
	MJCards &operator+=(std::vector<MJCard>& cards);
	MJCards &operator-=(std::vector<MJCard>& cards);
	MJCards &operator+=(const MJCard& card);
	MJCards &operator-=(const MJCard& card);
	MJCards &operator+=(const MJCards& cards);
	MJCards &operator-=(const MJCards& cards);
	int CountFlowers();
	
	bool Have(MJCard card);
	std::string Print() const;
	MJCard at(int idx) const;
	MJCard Draw();
	MJCard Rand() const;
	int Count() const; 

	int Count(const MJCard & card) const;
	unsigned long long Translate(int lack);

private:
	class _color_ref {
	public:
		class _value_ref {
		public:
			_value_ref(_color_ref &colorRef, int value) : colorRef(colorRef), value(value) {}
			_value_ref &operator=(int number) {
				if (colorRef.color == 5) colorRef = colorRef & ~(0b1 << value) | (number << value);
				else colorRef = colorRef & ~(0b111 << 3 * value) | (number << 3 * value);
				return *this;
			}
			_value_ref &operator+=(int number) { colorRef += number << (colorRef.color == 5 ? 1 : 3) * value; return *this; }
			_value_ref &operator-=(int number) { colorRef -= number << (colorRef.color == 5 ? 1 : 3) * value; return *this; }
			_value_ref &operator++() { return *this += 1; }
			_value_ref &operator--() { return *this -= 1; }
			_value_ref operator++(int) { _value_ref tmp(*this); ++*this; return tmp; }
			_value_ref operator--(int) { _value_ref tmp(*this); --*this; return tmp; }
			operator int() { return colorRef.color == 5 ? colorRef >> value & 0b1 : colorRef >> value * 3 & 0b111; }
		private:
			_color_ref &colorRef;
			int value;
		};
		_color_ref(int &ref, int color) : ref(ref), color(color) {}
		_color_ref &operator=(int v) { ref = v; return *this; }
		_color_ref &operator+=(int v) { ref += v; return *this; }
		_color_ref &operator-=(int v) { ref -= v; return *this; }
		operator int() { return ref; }
		_value_ref operator[](int value) { return _value_ref(*this, value); }
	private:
		int &ref;
		int color;
	};
public:
	_color_ref operator[](int color) { return _color_ref(color == 5 ? flowersCards : color == 4 ? windsCards : color == 3 ? cfbCards : mjcolor[color].value, color); }
	_color_ref::_value_ref operator[](const MJCard &card) { return (*this)[card.color][card.value]; }

	// func的型態為void(*)(int color, int value, int number)
	// all預設為false，忽略數量0的牌
	template <typename Func>
	void ForEach(Func func, bool all = false) const;

	struct FindResult { int color; int value; int number; };
	// condition的型態為bool(*)(int color, int value, int number)，true時搜尋結束
	template <typename Func>
	FindResult Find(Func condition, bool all = false) const;
};

template <typename Func>
void MJCards::ForEach(Func func, bool all) const {
	for (int c = 0; c < 3; ++c) {
		for (int v = 0; v < 9; ++v) {
			int n = mjcolor[c][v];
			if (all || n != 0)
				func(c, v, n);
		}
	}
	for (int v = 0; v < 3; ++v) {
		int n = cfbCards >> (v * 3) & 7;
		if (all || n != 0)
			func(3, v, n);
	}
	for (int v = 0; v < 4; ++v) {
		int n = windsCards >> (v * 3) & 7;
		if (all || n != 0)
			func(4, v, n);
	}
	for (int v = 0; v < 8; ++v) {
		int n = flowersCards >> v & 1;
		if (all || n != 0)
			func(5, v, n);
	}
}
template <typename Func>
MJCards::FindResult MJCards::Find(Func condition, bool all) const {
	for (int c = 0; c < 3; ++c) {
		for (int v = 0; v < 9; ++v) {
			int n = mjcolor[c][v];
			if ((all || n != 0) && condition(c, v, n))
				return { c, v, n };
		}
	}
	for (int v = 0; v < 3; ++v) {
		int n = cfbCards >> (v * 3) & 7;
		if ((all || n != 0) && condition(3, v, n))
			return { 3, v, n };
	}
	for (int v = 0; v < 4; ++v) {
		int n = windsCards >> (v * 3) & 7;
		if ((all || n != 0) && condition(4, v, n))
			return { 4, v, n };
	}
	for (int v = 0; v < 8; ++v) {
		int n = flowersCards >> v & 1;
		if ((all || n != 0) && condition(5, v, n))
			return { 5, v, n };
	}
	return { -1, -1, -1 };
}