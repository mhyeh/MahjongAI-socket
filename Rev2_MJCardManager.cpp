#include "Rev2_MJCardManager.h"
#include <time.h>

//之後將elements改成用long long處理

int typeLen[] = { 3, 3, 2, 2, 2, 1 };
int ponPonFlag = 1; //1表示門前沒有吃
int positionFlag = 0; //座位(門風台) 東:0 南:1 西:2 北:3
int roundFlag = 0; //圈(圈風台)  東:0 南:1 西:2 北:3

vector< group > elements;
vector< vector< group > > result;

double countTingTime = 0;
double count2 = 0;

vector< group > getElement(vector<int> hand)
{
	elements.clear();

	//刻子
	for (int i = 0; i < hand.size(); i = i + 1)
	{
		if (hand[i] >= 3)
		{
			group temp;
			temp.type = 0;
			temp.card.push_back(i);
			temp.card.push_back(i);
			temp.card.push_back(i);

			elements.push_back(temp);
		}
	}

	//順子
	for (int i = 0; i < hand.size(); i = i + 1)
	{
		if (i > 26) continue; //大字沒有順子

		if (i % 9 < 7 && hand[i] != 0 && hand[i + 1] != 0 && hand[i + 2] != 0)
		{
			group temp;
			temp.type = 1;
			temp.card.push_back(i);
			temp.card.push_back(i + 1);
			temp.card.push_back(i + 2);

			elements.push_back(temp);
		}
	}

	//一對
	for (int i = 0; i < hand.size(); i = i + 1)
	{
		if (hand[i] >= 2)
		{
			group temp;
			temp.type = 2;
			temp.card.push_back(i);
			temp.card.push_back(i);

			elements.push_back(temp);
		}
	}

	//小順
	for (int i = 0; i < hand.size(); i = i + 1)
	{
		if (i > 26) continue; //大字沒有順子

		if (i % 9 < 8 && hand[i] != 0 && hand[i + 1] != 0)
		{
			group temp;
			temp.type = 3;
			temp.card.push_back(i);
			temp.card.push_back(i + 1);

			elements.push_back(temp);
		}

	}

	//中洞
	for (int i = 0; i < hand.size(); i = i + 1)
	{
		if (i > 26) continue; //大字沒有順子

		if (i % 9 < 7 && hand[i] != 0 && hand[i + 2] != 0)
		{
			group temp;
			temp.type = 4;
			temp.card.push_back(i);
			temp.card.push_back(i + 2);

			elements.push_back(temp);
		}
	}

	//單張
	for (int i = 0; i < hand.size(); i = i + 1)
	{
		if (hand[i] != 0)
		{
			group temp;
			temp.type = 5;
			temp.card.push_back(i);

			elements.push_back(temp);
		}
	}

	return elements;
}

double T2Time = 0;
double T2TimeCount = 0;

vector<double> throwOneCard_v2(vector<int> hand, vector<int> deck, vector<int> door, int totalDeck, int handCount, vector<int> sea, int canPonPon, int position, int round)
{
	//特殊資訊儲存
	ponPonFlag = canPonPon;
	positionFlag = position;
	roundFlag = round;

	vector< vector<double> > finalResult;
	double minStep = 17;

	vector<int> scoreTemp;

	for (int i = 0; i < hand.size(); i = i + 1)
	{
		scoreTemp.push_back(0);
	}

	vector<double> S1 = calcScore(&scoreTemp, hand, deck, door, handCount, minStep, 0, totalDeck);
	if (S1[0] < minStep) minStep = S1[0]; //此手牌最小進聽數

	for (int i = 0; i < hand.size(); i = i + 1)
	{
		if (hand[i] != 0)
		{
			hand[i] = hand[i] - 1;
			//startClock = clock();
			//--------------------------------------------------------------------------------

			vector<int> score;

			for (int j = 0; j < hand.size(); j = j + 1)
			{
				score.push_back(0);
			}

			//---T1---

			vector<double> S1 = calcScore(&score, hand, deck, door, handCount, minStep, 1, totalDeck);
			//vector<double> S1 = calcScore(&score, hand, deck, door, handCount, minStep, 0, totalDeck);  //不計算台數

			//台麻講求快速
			if (S1[0] > minStep || S1[0] == -2)//進聽數比之前多
			{
				hand[i] = hand[i] + 1;
				continue;
			}

			//if (S1[0] < minStep) minStep = S1[0];
			double startTime = clock();
			int T2Count = 0;
			for (int j = 0; j < deck.size(); j = j + 1)
			{
				if (score[j] != 0 || deck[j] <= 0) continue;

				//----------------------------------------------------------
				hand[j] = hand[j] + 1;
				deck[j] = deck[j] - 1;

				vector<int> temp;

				vector<double> S2 = calcScore(&temp, hand, deck, door, handCount, minStep, 0, totalDeck);
				if (S2[0] != -2 && S2[0] == S1[0] && S2[1] > S1[1])
				{
					score[j] = 2;
					T2Count = T2Count + deck[j];
				}

				deck[j] = deck[j] + 1;
				hand[j] = hand[j] - 1;
			}
			double endTime = clock();
			T2Time = T2Time + endTime - startTime;
			T2TimeCount = T2TimeCount + 1;


			finalResult.push_back(vector<double>());
			finalResult[finalResult.size() - 1].push_back(i);
			finalResult[finalResult.size() - 1].push_back(S1[0]); //進聽數
			finalResult[finalResult.size() - 1].push_back(S1[1]); //T1張數
			finalResult[finalResult.size() - 1].push_back(T2Count); //T2張數
			finalResult[finalResult.size() - 1].push_back(S1[2]); //期望值
			finalResult[finalResult.size() - 1].push_back((S1[1] * T1_WEIGHT + T2Count * T2_WEIGHT) * (S1[2] * VALUE_WEIGHT)); //總分
			//finalResult[finalResult.size() - 1].push_back((S1[1] * T1_WEIGHT + T2Count * T2_WEIGHT)); //總分(速度分)

			//--------------------------------------------------------------------------------
			//endClock = clock();
			hand[i] = hand[i] + 1;
			//cout << i << " - " << (double)endClock - startClock << endl;
		}
	}

	//cout << "T2Time: " << T2Time << "  T2count: " << T2TimeCount << endl;

	if (finalResult.size() == 0)
	{
		for (int i = 0; i < 3; i = i + 1)
		{
			for (int j = 0; j < 9; j = j + 1)
			{
				cout << hand[i * 9 + j] << " ";
			}
			cout << endl;
		}
		for (int i = 27; i < 34; i = i + 1)
		{
			cout << hand[i] << " ";
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
		for (int i = 27; i < 34; i = i + 1)
		{
			cout << door[i] << " ";
		}
		cout << handCount << endl;
		cout << totalDeck << endl;
	}

	//算分公式: (T1 * 20 + T2 * 5) * 期望值
	for (int i = 0; i < finalResult.size(); i = i + 1)
	{
		for (int j = i + 1; j < finalResult.size(); j = j + 1)
		{
			if (finalResult[i][5] < finalResult[j][5]) swap(finalResult[i], finalResult[j]);
			else if (finalResult[i][5] == finalResult[j][5])
			{
				if (sea[finalResult[i][0]] < sea[finalResult[j][0]])
				{
					swap(finalResult[i], finalResult[j]);
				}
				else if (sea[finalResult[i][0]] == sea[finalResult[j][0]] && hand[finalResult[i][0]] > hand[finalResult[j][0]]) //手上有比較少的先丟
				{
					swap(finalResult[i], finalResult[j]);
				}
				else if (finalResult[i][0] / 9 < 3 && finalResult[i][0] >= 3) //大字先丟
				{
					swap(finalResult[i], finalResult[j]);
				}
				else if (sea[finalResult[i][0]] == sea[finalResult[j][0]] && hand[finalResult[i][0]] == hand[finalResult[j][0]] && abs(int(finalResult[i][0]) % 9 - 4) < abs(int(finalResult[j][0]) % 9 - 4)) //靠邊的先丟
				{
					swap(finalResult[i], finalResult[j]);
				}
			}
		}
	}

	vector<double> returnValue;
	returnValue.push_back(finalResult[0][0]);
	returnValue.push_back(finalResult[0][1]);
	returnValue.push_back(finalResult[0][2]);
	returnValue.push_back(finalResult[0][3]);
	returnValue.push_back(finalResult[0][4]);
	returnValue.push_back(finalResult[0][5]);

	//cout << "countTing: " << countTingTime << endl;
	//cout << "count: " << count2 << endl;

	return returnValue;
}

vector<double> calcScore(vector<int>* record, vector<int> hand, vector<int> deck, vector<int> door, int handCount, int minStep, int countTi, int totalDeck)
{
	vector<double> finalResult;

	vector<int> score; //1表示T1, 2表示T2, 3表示T3, 0表示沒用

	for (int i = 0; i < hand.size(); i = i + 1)
	{
		score.push_back(0);
	}
	
	elements = getElement(hand);
	
	vector<group> combine;
	vector<int> usedHand;
	int min = 17;
	int flag = 0;

	for (int i = 0; i < hand.size(); i = i + 1)
	{
		usedHand.push_back(0);
	}

	result.clear();

	double startTime = clock();
	countTing(handCount / 3 + 1, 0, combine, usedHand, hand, &min);
	double endTime = clock();

	countTingTime = countTingTime + endTime - startTime;
	count2 = count2 + 1;

	double totalTi = 0;
	double totalTiCount = 0;

	if (min <= minStep)
	{
		flag = 1;
		for (int i = 0; i < result.size(); i = i + 1)
		{
			int temp = 0;
			int adStep = 1;
			int hasPair = 0;
			for (int j = 0; j < result[i].size(); j = j + 1)
			{
				if (result[i][j].type == 2) hasPair = 1;
				if (result[i][j].type == 2 || result[i][j].type == 5) adStep = 0;
				temp = temp + result[i][j].card.size();
			}
			if ((handCount / 3 + 1) * 3 - 2 - temp + adStep > min) continue;

			vector< vector<int> > changeTile;

			//是最小的組合
			for (int j = 0; j < result[i].size(); j = j + 1)
			{
				changeTile.push_back(vector<int>());

				if (result[i][j].type == 2) //一對
				{
					score[result[i][j].card[0]] = 1;
				}
				else if (result[i][j].type == 3) //小順
				{
					if (result[i][j].card[0] % 9 != 0) //左邊可以接
					{
						score[result[i][j].card[0] - 1] = 1;
					}

					if (result[i][j].card[1] % 9 != 8) //右邊可以接
					{
						score[result[i][j].card[1] + 1] = 1;
					}
				}
				else if (result[i][j].type == 4) //中洞
				{
					score[result[i][j].card[0] + 1] = 1;
				}
				else if (result[i][j].type == 5) //單張
				{
					if (hasPair == 1 && result[i][j].card[0] / 9 < 3) //且不是大字
					{
						if (result[i][j].card[0] % 9 > 0) //左邊可以接
						{
							score[result[i][j].card[0] - 1] = 1;
						}

						if (result[i][j].card[0] % 9 > 1) //左邊可以接
						{
							score[result[i][j].card[0] - 2] = 1;
						}

						if (result[i][j].card[0] % 9 < 8) //右邊可以接
						{
							score[result[i][j].card[0] + 1] = 1;
						}

						if (result[i][j].card[0] % 9 < 7) //右邊可以接
						{
							score[result[i][j].card[0] + 2] = 1;
						}
					}

					score[result[i][j].card[0]] = 1;
				}
			}

			if (countTi == 1) //估算台數
			{
				vector<double> totalValue;
				double temp = 0;

				estimute_v2(result[i], deck, totalDeck, &totalValue, door);

				if (totalValue.size() < VALUE_FIRST_X)
				{
					for (int j = 0; j < totalValue.size(); j = j + 1)
					{
						temp = temp + totalValue[j];
					}
				}
				else
				{
					for (int j = 0; j < VALUE_FIRST_X; j = j + 1)
					{
						for (int k = j + 1; k < totalValue.size(); k = k + 1)
						{
							if (totalValue[j] < totalValue[k]) swap(totalValue[j], totalValue[k]);
						}
						temp = temp + totalValue[j];
					}
				}

				if (totalTi < temp) totalTi = temp;

			}
		}
	}

	if (flag == 0)
	{
		finalResult.push_back(-2);

		return finalResult;
	}

	int total = 0;

	for (int i = 0; i < score.size(); i = i + 1)
	{
		if (score[i] == 1)
		{
			total = total + deck[i];
		}
	}

	record->assign(score.begin(), score.end());

	finalResult.push_back(min);
	finalResult.push_back(total);
	finalResult.push_back(totalTi);
	finalResult.push_back(totalTiCount);

	return finalResult;
}

double calcTi(vector<int> hand, vector<int> door)
{
	int flag[3] = {0}; //暗刻數量, 東南西北組數, 中發白組數
	double score = VALUE_BASE;
	double fanWeight[14] = FAN_WEIGHT;

	int noThreeTemp = 0;
	int colorCheck[4] = { 0 };
	int pairCount = 0;
	int pairTile = 0;

	for (int i = 0; i < 27; i = i + 1)
	{
		if (hand[i] != 0)
		{
			colorCheck[i / 9] = 1;

			if (hand[i] == 2) pairCount = pairCount + 1;
			else if (hand[i] == 4) pairCount = pairCount + 2;

			if (hand[i] != 3) noThreeTemp = noThreeTemp + 1;

			if (hand[i] >= 3) flag[0] = flag[0] + 1;
			
		}

		if (door[i] != 0)
		{
			colorCheck[i / 9] = 1;

			if (door[i] == -4) flag[0] = flag[0] + 1; //暗刻
		}
	}
	for (int i = 27; i < 31; i = i + 1) //東南西北
	{
		if (hand[i] != 0 || door[i] != 0)
		{
			colorCheck[3] = 1;
			flag[1] = flag[1] + 1;

			if (hand[i] == 2) pairTile = 1;
			
			if (hand[i] >= 3 || door[i] != 0) //有刻或槓
			{
				if (i - 27 == roundFlag) score = score + fanWeight[11]; //圈風台
				if (i - 27 == positionFlag) score = score + fanWeight[12]; //門風台
			}
		}
	}
	for (int i = 31; i < 34; i = i + 1)
	{
		if (hand[i] != 0 || door[i] != 0)
		{
			colorCheck[3] = 1;
			flag[2] = flag[2] + 1;

			if (hand[i] == 2) pairTile = 2;

			if (hand[i] >= 3 || door[i] != 0) //有刻或槓
			{
				score = score + fanWeight[6]; //中發白
			}
		}
	}

	if (ponPonFlag == 1 && noThreeTemp == 1 && pairCount == 1) score = score + fanWeight[0]; //對對胡
	if (colorCheck[0] + colorCheck[1] + colorCheck[2] == 1)
	{
		if (colorCheck[3] == 1) score = score + fanWeight[1]; //混一色
		else if (colorCheck[3] == 0)score = score + fanWeight[2]; //清一色
	}
	if (flag[0] == 3) score = score + fanWeight[3]; //三暗刻
	if (flag[0] == 4) score = score + fanWeight[4]; //四暗刻
	if (flag[0] == 5) score = score + fanWeight[5]; //五暗刻
	if (flag[1] == 4 && pairTile != 1) score = score + fanWeight[7] - fanWeight[12]; //大四喜
	if (flag[1] == 4 && pairTile == 1) score = score + fanWeight[8] - fanWeight[12]; //小四喜
	if (flag[2] == 3 && pairTile != 2) score = score + fanWeight[9] - 3 * fanWeight[6]; //大三元
	if (flag[2] == 3 && pairTile == 2) score = score + fanWeight[10] - 3 * fanWeight[6]; //大三元
	if (flag[1] + flag[2] == 6) score = score + fanWeight[13] - fanWeight[0]; //字一色
	
	//cout << "score: " << score << "  fourTileTemp: " << fourTileTemp << endl;
	
	return score;
}

void countTing(int remain, int nowPos, vector< group > combine, vector<int> usedHand, const vector<int>& hand, int *  min)
{
	int len = 0;
	for (int i = 0; i < combine.size(); i = i + 1)
	{
		len = len + typeLen[combine[i].type];
	}

	if (remain == 0)
	{
		result.push_back(combine);

		int additionalStep = 1;
		for (int i = 0; i < combine.size(); i = i + 1)
		{
			if (combine[i].type == 2 || combine[i].type == 5) additionalStep = 0;
		}

		int nowLen = len - additionalStep;
		int maxLen = (remain + combine.size()) * 3 - 2;

		if (maxLen - nowLen < *min) *min = maxLen - nowLen;
		return;
	}

	for (int i = nowPos; i < elements.size(); i = i + 1)
	{
		int nowLen = len + (remain * typeLen[elements[i].type]);
		int maxLen = (remain + combine.size()) * 3 - 2;

		if (maxLen - nowLen > *min) break;

		int can = 1;
		int temp[34] = { 0 };

		for (int j = 0; j < elements[i].card.size(); j = j + 1)
		{
			temp[elements[i].card[j]] = temp[elements[i].card[j]] + 1;

			if (hand[elements[i].card[j]] - usedHand[elements[i].card[j]] - temp[elements[i].card[j]] < 0)
			{
				can = 0;
				break;
			}
		}

		if (can == 1)
		{
			combine.push_back(elements[i]);
			for (int j = 0; j < usedHand.size(); j = j + 1)
			{
				usedHand[j] = usedHand[j] + temp[j];
			}

			countTing(remain - 1, i, combine, usedHand, hand, min);

			for (int j = 0; j < elements[i].card.size(); j = j + 1)
			{
				usedHand[elements[i].card[j]] = usedHand[elements[i].card[j]] - 1;
			}
			combine.pop_back();
		}
	}
}

void estimute_v2(vector< group > combine, vector<int> deck, int totalDeck, vector<double> * totalValue, vector<int> door)
{
	vector< vector<changeNode> > changePattern;

	vector<int> nowHand;

	for (int i = 0; i < 34; i = i + 1)
	{
		nowHand.push_back(0);
	}

	changeNode temp;

	for (int i = 0; i < combine.size(); i = i + 1)
	{
		changePattern.push_back(vector<changeNode>());

		if (combine[i].type == 0)
		{
			nowHand[combine[i].card[0]] = nowHand[combine[i].card[0]] + 3;
		}
		else if (combine[i].type == 1)
		{
			nowHand[combine[i].card[0]] = nowHand[combine[i].card[0]] + 1;
			nowHand[combine[i].card[1]] = nowHand[combine[i].card[1]] + 1;
			nowHand[combine[i].card[2]] = nowHand[combine[i].card[2]] + 1;
		}
		else if (combine[i].type == 2) //一對
		{
			nowHand[combine[i].card[0]] = nowHand[combine[i].card[0]] + 2;

			temp.leftCard = combine[i].card[0];
			temp.type = 21;
			temp.len = 2;
			changePattern[i].push_back(temp);

			if (deck[combine[i].card[0]] >= 1)
			{
				temp.leftCard = combine[i].card[0];
				temp.type = 22;
				temp.len = 3;
				changePattern[i].push_back(temp);
			}
		}
		else if (combine[i].type == 3) //小順
		{
			nowHand[combine[i].card[0]] = nowHand[combine[i].card[0]] + 1;
			nowHand[combine[i].card[1]] = nowHand[combine[i].card[1]] + 1;

			if (combine[i].card[0] % 9 != 0 && deck[combine[i].card[0] - 1] >= 1)
			{
				temp.leftCard = combine[i].card[0] - 1;
				temp.type = 31;
				temp.len = 3;
				changePattern[i].push_back(temp);
			}
			if (combine[i].card[1] % 9 != 8 && deck[combine[i].card[1] + 1] >= 1)
			{
				temp.leftCard = combine[i].card[0];
				temp.type = 32;
				temp.len = 3;
				changePattern[i].push_back(temp);
			}
		}
		else if (combine[i].type == 4) //中洞
		{
			nowHand[combine[i].card[0]] = nowHand[combine[i].card[0]] + 1;
			nowHand[combine[i].card[1]] = nowHand[combine[i].card[1]] + 1;

			if (deck[combine[i].card[0] + 1] >= 1)
			{
				temp.leftCard = combine[i].card[0];
				temp.type = 41;
				temp.len = 3;
				changePattern[i].push_back(temp);
			}
		}
		else if (combine[i].type == 5) //單張
		{
			nowHand[combine[i].card[0]] = nowHand[combine[i].card[0]] + 1;

			if (combine[i].card[0] <= 26) //不是大字才有順子
			{
				if (combine[i].card[0] % 9 >= 2 && deck[combine[i].card[0] - 2] >= 1 && deck[combine[i].card[0] - 1] >= 1) // 1 2 x
				{
					temp.leftCard = combine[i].card[0] - 2;
					temp.type = 51;
					temp.len = 3;
					changePattern[i].push_back(temp);
				}
				if (combine[i].card[0] % 9 >= 1 && combine[i].card[0] % 9 <= 7 && deck[combine[i].card[0] - 1] >= 1 && deck[combine[i].card[0] + 1] >= 1) // 2 x 4
				{
					temp.leftCard = combine[i].card[0] - 1;
					temp.type = 52;
					temp.len = 3;
					changePattern[i].push_back(temp);
				}
				if (combine[i].card[0] % 9 <= 6 && deck[combine[i].card[0] + 1] >= 1 && deck[combine[i].card[0] + 2] >= 1) // x 3 4
				{
					temp.leftCard = combine[i].card[0];
					temp.type = 53;
					temp.len = 3;
					changePattern[i].push_back(temp);
				}
			}

			// x x x
			if (deck[combine[i].card[0]] >= 2)
			{
				temp.leftCard = combine[i].card[0];
				temp.type = 54;
				temp.len = 3;
				changePattern[i].push_back(temp);
			}

			// x x
			if (deck[combine[i].card[0]] >= 1)
			{
				temp.leftCard = combine[i].card[0];
				temp.type = 55;
				temp.len = 2;
				changePattern[i].push_back(temp);
			}

		}

	}

	returnAllCombineProp_v2(0, changePattern, 0, nowHand, totalValue, deck, totalDeck, door);

}

void returnAllCombineProp_v2(int nowPos, vector< vector<changeNode> > changePattern, int once, vector<int> nowHand, vector<double> * totalValue, vector<int> deck, int totalDeck, vector<int> door)
{
	if (nowPos >= changePattern.size()) //end
	{
		if (once == 1)
		{
			totalValue->push_back(calcTi(nowHand, door));
		}
		return;
	}

	if (changePattern[nowPos].size() == 0)
	{
		returnAllCombineProp_v2(nowPos + 1, changePattern, once, nowHand, totalValue, deck, totalDeck, door);
	}

	vector<int> oriHand;
	oriHand.assign(nowHand.begin(), nowHand.end());
	int oriTotalDeck = totalDeck;
	vector<int> oriDeck;
	oriDeck.assign(deck.begin(), deck.end());

	for (int i = 0; i < changePattern[nowPos].size(); i = i + 1)
	{
		if (changePattern[nowPos][i].len == 2 && once == 1) continue;

		nowHand.assign(oriHand.begin(), oriHand.end());
		deck.assign(oriDeck.begin(), oriDeck.end());
		totalDeck = oriTotalDeck;

		int flag = 0;
		
		switch (changePattern[nowPos][i].type)
		{
		case 21:
			//不用換牌
			break;
		case 22:
			if (deck[changePattern[nowPos][i].leftCard] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard] = nowHand[changePattern[nowPos][i].leftCard] + 1;
				deck[changePattern[nowPos][i].leftCard] = deck[changePattern[nowPos][i].leftCard] - 1;
				totalDeck = totalDeck - 1;
			}

			break;
		case 31:
			if (deck[changePattern[nowPos][i].leftCard] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard] = nowHand[changePattern[nowPos][i].leftCard] + 1;
				deck[changePattern[nowPos][i].leftCard] = deck[changePattern[nowPos][i].leftCard] - 1;
				totalDeck = totalDeck - 1;
			}
			break;
		case 32:
			if (deck[changePattern[nowPos][i].leftCard + 2] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard + 2] = nowHand[changePattern[nowPos][i].leftCard + 2] + 1;
				deck[changePattern[nowPos][i].leftCard + 2] = deck[changePattern[nowPos][i].leftCard + 2] - 1;
				totalDeck = totalDeck - 1;
			}
			break;
		case 41:
			if (deck[changePattern[nowPos][i].leftCard + 1] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard + 1] = nowHand[changePattern[nowPos][i].leftCard + 1] + 1;
				deck[changePattern[nowPos][i].leftCard + 1] = deck[changePattern[nowPos][i].leftCard + 1] - 1;
				totalDeck = totalDeck - 1;
			}
			break;
		case 51:
			if (deck[changePattern[nowPos][i].leftCard] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard] = nowHand[changePattern[nowPos][i].leftCard] + 1;
				deck[changePattern[nowPos][i].leftCard] = deck[changePattern[nowPos][i].leftCard] - 1;
				totalDeck = totalDeck - 1;
			}

			if (deck[changePattern[nowPos][i].leftCard + 1] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard + 1] = nowHand[changePattern[nowPos][i].leftCard + 1] + 1;
				deck[changePattern[nowPos][i].leftCard + 1] = deck[changePattern[nowPos][i].leftCard + 1] - 1;
				totalDeck = totalDeck - 1;
			}
			break;
		case 52:
			if (deck[changePattern[nowPos][i].leftCard] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard] = nowHand[changePattern[nowPos][i].leftCard] + 1;
				deck[changePattern[nowPos][i].leftCard] = deck[changePattern[nowPos][i].leftCard] - 1;
				totalDeck = totalDeck - 1;
			}

			if (deck[changePattern[nowPos][i].leftCard + 2] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard + 2] = nowHand[changePattern[nowPos][i].leftCard + 2] + 1;
				deck[changePattern[nowPos][i].leftCard + 2] = deck[changePattern[nowPos][i].leftCard + 2] - 1;
				totalDeck = totalDeck - 1;
			}
			break;
		case 53:
			if (deck[changePattern[nowPos][i].leftCard + 1] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard + 1] = nowHand[changePattern[nowPos][i].leftCard + 1] + 1;
				deck[changePattern[nowPos][i].leftCard + 1] = deck[changePattern[nowPos][i].leftCard + 1] - 1;
				totalDeck = totalDeck - 1;
			}

			if (deck[changePattern[nowPos][i].leftCard + 2] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard + 2] = nowHand[changePattern[nowPos][i].leftCard + 2] + 1;
				deck[changePattern[nowPos][i].leftCard + 2] = deck[changePattern[nowPos][i].leftCard + 2] - 1;
				totalDeck = totalDeck - 1;
			}
			break;
		case 54:
			if (deck[changePattern[nowPos][i].leftCard] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard] = nowHand[changePattern[nowPos][i].leftCard] + 1;
				deck[changePattern[nowPos][i].leftCard] = deck[changePattern[nowPos][i].leftCard] - 1;
				totalDeck = totalDeck - 1;
			}

			if (deck[changePattern[nowPos][i].leftCard] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard] = nowHand[changePattern[nowPos][i].leftCard] + 1;
				deck[changePattern[nowPos][i].leftCard] = deck[changePattern[nowPos][i].leftCard] - 1;
				totalDeck = totalDeck - 1;
			}
			break;
		case 55:
			if (deck[changePattern[nowPos][i].leftCard] <= 0 || totalDeck <= 0)
			{
				flag = 1;
			}

			if (flag == 0)
			{
				nowHand[changePattern[nowPos][i].leftCard] = nowHand[changePattern[nowPos][i].leftCard] + 1;
				deck[changePattern[nowPos][i].leftCard] = deck[changePattern[nowPos][i].leftCard] - 1;
				totalDeck = totalDeck - 1;
			}
			break;
		default:
			cout << "unknown typeID!!!" << endl;
			break;
		}

		if (flag == 1) continue;

		returnAllCombineProp_v2(nowPos + 1, changePattern, ((changePattern[nowPos][i].len == 2 || once == 1) ? 1 : 0), nowHand, totalValue, deck, totalDeck, door);

	}
}