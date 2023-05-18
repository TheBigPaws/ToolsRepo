#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;

	bool focus;
	bool zoomOut_AB;
	bool zoomIn_AB;


	int rotate[2];

	int MouseXY[2];

	bool RMBdown;
	bool LMBdown;
};
