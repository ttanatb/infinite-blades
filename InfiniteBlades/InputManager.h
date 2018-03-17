#pragma once
#include <vector>
#include <Windows.h>
#include <map>
#include <iostream>
class InputManager {
private:
	InputManager();
	~InputManager();

	static InputManager* instance;// = nullptr;
	std::vector<char> keysToPollFor;
	std::map<char, short> keyStates;
	std::map<char, short> prevKeyStates;
	int keyCount;
public:
	static InputManager* GetInstance();
	static void ReleaseInstance();

	void Update();
	void AddKeyToPollFor(char key);
	void AddKeysToPollFor(char* keyArr, int count);

	bool GetKey(char key);
	bool GetKeyDown(char key);
	bool GetKeyUp(char key);
};