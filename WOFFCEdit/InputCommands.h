#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;

	int rotate[2];
	
	bool moveObject;

	int MouseXY[2];

	bool RMBdown;
	bool LMBdown;
};
