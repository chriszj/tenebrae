//=============================================================================
//
// 地面処理 [field.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct FIELD
{
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4		mtxWorld;   // ワールドマトリックス
	DX11_MODEL		fieldWall1Model;		// モデル情報
	DX11_MODEL      fieldWall2Model;
	DX11_MODEL      fieldPillarsModel;
	DX11_MODEL      fieldPillarCornersModel;
	DX11_MODEL      fieldWallDoorsModel;
	DX11_MODEL      fieldWallGroundModel;
	DX11_MODEL      fieldWallProps1Model;
	DX11_MODEL      fieldWallProps2Model;
	DX11_MODEL      fieldWallProps3Model;
	DX11_MODEL      fieldWallProps4Model;

	XMFLOAT4		diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	BOOL            load;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitField(void);
void UninitField(void);
void UpdateField(void);
void DrawField(void);

