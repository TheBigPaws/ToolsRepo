#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;

	int rotate[2];

	int MouseXY[2];

	bool RMBdown;
	bool LMBdown;
};
