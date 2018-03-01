#pragma once
#include <vector>
#include <Windows.h>
#include <map>
#include <iostream>
class Input {
private:
	Input();
	~Input();

	static Input* instance;// = nullptr;
	std::vector<char> keysToPollFor;
	std::map<char, short> keyStates;
	int keyCount;
public:
	static Input* GetInstance();
	static void ReleaseInstance();

	void Update();
	void AddKeyToPollFor(char key);
	void AddKeysToPollFor(char* keyArr, int count);

	bool GetKey(char key);
	bool GetKeyDown(char key);
	bool GetKeyUp(char key);
};