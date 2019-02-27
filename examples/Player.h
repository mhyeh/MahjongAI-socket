


#pragma once

#include"MJCards.h"
#include<iostream>
#include<string>
#include<tuple>



using namespace std;


enum CommandType
{
	NONE = 0b0000000,			//沒有
	COMMAND_PON = 0b0000001, //碰
	COMMAND_GON = 0b0000010, //直槓
	COMMAND_ANGON = 0b0000100, //暗槓
	COMMAND_PONGON = 0b0001000, //面下槓
	COMMAND_HU = 0b0010000, //胡
	COMMAND_ZIMO = 0b0100000, //自摸
	COMMAND_EAT = 0b1000000
};

enum Actions
{
	NOTREADY = 0,
	PASS,
	THROW,
	HU,
	ZIMO,
	GON,
	ONGON,
	PONGON,
	PON,
	EAT,
	BUHUA
};

class Player {
public:

	MJCards Hand;
	//MJCards Door;

	Player();
	
	bool isDron; //是否為莊
	long long eat; //記錄吃，每個吃用3bit儲存(可能吃了四次)，每一色有七種吃法，共63bit 0~20 萬 21~41 桶 42~62 條 
	long long pon; //記錄碰，0~8 萬 9~17 桶 18~26 條 27~29 字 30~33風
	long long gon; //記錄槓(包括兩種明槓)，0~8 萬 9~17 桶 18~26 條 27~29 字 30~33風
	long long anGon; //記錄暗槓，0~8 萬 9~17 桶 18~26 條 27~29 字 30~33風
	int flowers;
	int winds; //與博惟的格式相同
	MJCards discards;

	
	int id, uid;
	std::string Print();
	int Count();
	virtual bool Ready();
	virtual bool IsHu();
	virtual bool WannaHu(const MJCard & card); //要搶槓胡嗎?
	virtual std::pair<CommandType, MJCard> WannaHuGon(bool canHu, bool canGon, const MJCard & card, const MJCard & gonCard);
	virtual CommandType WannaHGPE(bool canHu, bool canGon, bool canPon, bool canEat, const MJCard & card, int idx); //idx為丟牌者的ID
	virtual int Pick2Eat(const MJCard & card);

	Actions Action;
	virtual MJCard Throw();


};