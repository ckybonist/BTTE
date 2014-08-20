#ifndef __DICE_H__
#define ___DICE_H__

#include <cstdlib>

class Dice {
public:
	Dice() {};
	int roll(int, int);
	int excludeNum(int, int, int);
	int excludeSet(int*, int, int, int);
};

#endif
// dice.h
