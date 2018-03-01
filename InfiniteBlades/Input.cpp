#include "Input.h"
Input* Input::instance = nullptr;

Input::Input()
{
	keyCount = 0;
}

Input::~Input()
{
}

Input * Input::GetInstance()
{
	if (instance == nullptr)
		instance = new Input();
	return instance;
}

void Input::ReleaseInstance()
{
	if (instance != nullptr)
		delete(instance);
}

void Input::Update()
{
	//std::cout << keyCount << std::endl;
	for (size_t i = 0; i < keyCount; ++i) {
		char key = keysToPollFor[i];
		keyStates[key] = GetAsyncKeyState(key);
	}
}

void Input::AddKeyToPollFor(char key)
{
	keysToPollFor.push_back(key);
	keyStates.insert(std::pair<char, short>(key, 0));
	keyCount += 1;
}

void Input::AddKeysToPollFor(char * keyArr, int count)
{
	for (size_t i = 0; i < count; ++i) {
		keysToPollFor.push_back(keyArr[i]);
		keyStates.insert(std::pair<char, short>(keyArr[i], 0));
		keyCount += 1;
	}
}

bool Input::GetKey(char key)
{
	return keyStates[key] & 0x8000;
}
