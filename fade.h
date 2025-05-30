//=============================================================================
//
// フェード処理 [fade.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************

// フェードの状態
enum
{
	FADE_NONE = 0,		// 何もない状態
	FADE_IN,			// フェードイン処理
	FADE_OUT,			// フェードアウト処理
	FADE_MAX
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(int fade, int modeNext);
int GetFade(void);



