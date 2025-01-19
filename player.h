//=============================================================================
//
// モデル処理 [player.h]
// Author : 
//
//=============================================================================
#pragma once

#include "collision.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER		(1)					// プレイヤーの数

#define	PLAYER_SIZE		(5.0f)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT3        viewPoint;
	float           viewDirX, viewDirY;

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス

	BOOL			load;
	DX11_MODEL		model;		// モデル情報
	XMFLOAT4		diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	XMFLOAT3		armPos;		// ポリゴンの位置
	XMFLOAT3		armRot;		// ポリゴンの向き(回転)
	XMFLOAT3		armScl;		// ポリゴンの大きさ(スケール)

	DX11_MODEL      armModel;
	XMFLOAT4        armModelDiffuse[MODEL_MAX_MATERIAL];

	float			spd;		// 移動スピード
	float           movX, movY;
	float			dir;		// 向き
	float			size;		// 当たり判定の大きさ
	int				shadowIdx;	// 影のIndex
	BOOL			use;

	COLLIDER2DBOX collider;       // 当たり判定の改善のため


};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

BOOL HasReachedGoal(void);
