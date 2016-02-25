#include "stdafx.h"

#ifndef _INPUTMANAGER_
#define _INPUTMANAGER_

class InputManager
{
public:
	InputManager(void);
	~InputManager(void);

	void Update();
	bool KeyDown( unsigned char key );
	bool KeyUp( unsigned char key );
	bool KeyPress( unsigned char key );

	void GetCursorPosClient(POINT &pos);
	void GetCursorPosScreen(POINT &pos);

private:
	HWND hWnd;
	unsigned char *m_curKeyBuffer;
	unsigned char *m_prevKeyBuffer;
};

#endif // _INPUTMANAGER_