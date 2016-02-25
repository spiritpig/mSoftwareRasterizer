#include "stdafx.h"

extern HWND g_hWnd;

InputManager::InputManager()
{
	m_curKeyBuffer = new unsigned char[256];
	ZeroMemory( m_curKeyBuffer, sizeof(unsigned char)*256 );
	m_prevKeyBuffer = new unsigned char[256];
	ZeroMemory( m_prevKeyBuffer, sizeof(unsigned char)*256 );
}


InputManager::~InputManager(void)
{
	SAFE_DELARR(m_prevKeyBuffer);
	SAFE_DELARR(m_curKeyBuffer);
}

void InputManager::Update()
{
	Swap<unsigned char *>(m_prevKeyBuffer, m_curKeyBuffer );
	GetKeyboardState( m_curKeyBuffer );
}

bool InputManager::KeyDown( unsigned char key )
{
	if ( !(m_prevKeyBuffer[key] & 0x80) &&
		 (m_curKeyBuffer[key] & 0x80) )
	{
		return true;
	}
	return false;
}

bool InputManager::KeyUp( unsigned char key )
{
	if ( (m_prevKeyBuffer[key] & 0x80) &&
		!(m_curKeyBuffer[key] & 0x80) )
	{
		return true;
	}
	return false;
}

bool InputManager::KeyPress( unsigned char key )
{
	return (m_curKeyBuffer[key] & 0x80);
}

void InputManager::GetCursorPosClient(POINT &pos)
{
	GetCursorPos(&pos);
	ScreenToClient(g_hWnd, &pos);
}

void InputManager::GetCursorPosScreen(POINT &pos)
{
	GetCursorPos(&pos);
}