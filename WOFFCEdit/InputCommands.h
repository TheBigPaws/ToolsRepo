#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;

	bool raiseGround;
	bool lowerGround;
	bool levelGround;

	int rotate[2];
	
	int MouseXY[2];

	bool RMBdown;
	bool LMBdown;
};
