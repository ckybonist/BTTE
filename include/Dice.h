#ifndef __DICE_H__
#define ___DICE_H__

#include <cstdlib>

class Dice
{
public:
	Dice() {};
	static int roll(int, int);
	static int excludeNum(int, int, int);
	static int excludeSet(int*, int, int, int);
};

#endif
// dice.h
