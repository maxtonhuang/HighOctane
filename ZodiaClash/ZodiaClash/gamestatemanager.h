#pragma once

typedef void(*FP)(void);

extern int current, previous, next;

extern FP fpLoad, fpInitialize, fpUpdate, fpDraw, fpFree, fpUnload;

void GSM_Initialize(int startingState);

void GSM_Update();

void GSM_Exit();

enum GS_STATES
{
	GS_MAINMENU = 0,
	GS_CREDITS,
	GS_EXPLORE,
	GS_BATTLE,

	GS_QUIT,
	GS_RESTART
};
