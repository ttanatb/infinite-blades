#include "InputManager.h"
InputManager* InputManager::instance = nullptr;

InputManager::InputManager()
{
	keyCount = 0;
}

InputManager::~InputManager()
{
}

InputManager * InputManager::GetInstance()
{
	if (instance == nullptr)
		instance = new InputManager();
	return instance;
}

void InputManager::ReleaseInstance()
{
	if (instance != nullptr)
		delete(instance);
}

void InputManager::Update()
{
	for (size_t i = 0; i < keyCount; ++i) {
		char key = keysToPollFor[i];
		prevKeyStates[key] = keyStates[key];
		keyStates[key] = GetAsyncKeyState(key);
	}
}

void InputManager::AddKeyToPollFor(char key)
{
	keysToPollFor.push_back(key);
	keyStates.insert(std::pair<char, short>(key, 0));
	prevKeyStates.insert(std::pair<char, short>(key, 0));
	keyCount += 1;
}

void InputManager::AddKeysToPollFor(char * keyArr, size_t count)
{
	for (size_t i = 0; i < count; ++i) {
		AddKeyToPollFor(keyArr[i]);
	}
}

bool InputManager::GetKey(char key)
{
	return keyStates[key] & 0x8000;
}

bool InputManager::GetKeyDown(char key)
{
	return ((keyStates[key] & 0x8000) && !(prevKeyStates[key] & 0x8000));
}

bool InputManager::GetKeyUp(char key)
{
	return (!(keyStates[key] & 0x8000) && (prevKeyStates[key] & 0x8000));
}
