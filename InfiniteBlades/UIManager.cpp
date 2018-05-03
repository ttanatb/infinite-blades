#include "UIManager.h"

UIManager::UIManager(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureSrv, XMFLOAT4 textureRect) 
{
	spriteBatch = new SpriteBatch(context);
	batchTextureSrv = textureSrv;
	rect = { (int)textureRect.x,(int)textureRect.y,(int)textureRect.z,(int)textureRect.w };
}

UIManager::UIManager(ID3D11Device* device, ID3D11DeviceContext* context, wchar_t * fontc, XMFLOAT4 fontRect) 
{
	spriteBatch = new SpriteBatch(context);
	spriteFont = new SpriteFont(device, L"Assets/Fonts/Arial.spritefont");
	fontContent = fontc;
	rect = { (int)fontRect.x,(int)fontRect.y,(int)fontRect.z,(int)fontRect.w };
}

UIManager::~UIManager()
{
	if (spriteBatch)
		delete spriteBatch;

	if (spriteFont)
		delete spriteFont;
}

SpriteBatch* UIManager::getSpriteBatch() 
{
	return spriteBatch;
}

SpriteFont* UIManager::getSpriteFont() 
{
	return spriteFont;
}

RECT UIManager::getRECT() 
{
	return rect;
}