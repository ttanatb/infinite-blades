#pragma once
#include <string>
using namespace std;

enum GameState
{
	Menu,
	Play,
	GameOver
};

class GameManager
{
public:
	GameState state;

	string GetStateName();
	void NextState();

};