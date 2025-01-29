//=============================================================================
//
// ライト処理 [light.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"

#define LIGHT_VIEW_ANGLE  (XMConvertToRadians(90.0f))	
#define LIGHT_VIEW_ASPECT ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)
#define LIGHT_VIEW_NEAR_Z  1.0f
#define LIGHT_VIEW_FAR_Z   100000.0f

//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

static BOOL		g_FogEnable = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
void InitLight(void)
{

	//ライト初期化
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// 減衰距離
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ライトのタイプ
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// 並行光源の設定（世界を照らす光）
	g_Light[0].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );		// 光の向き
	g_Light[0].Diffuse   = XMFLOAT4( 0.3f, 0.3f, 0.3f, 1.0f );	// 光の色
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// 並行光源
	g_Light[0].Enable = TRUE;									// このライトをON
	SetLight(0, &g_Light[0]);									// これで設定している



	// フォグの初期化（霧の効果）
	g_Fog.FogStart = 100.0f;									// 視点からこの距離離れるとフォグがかかり始める
	g_Fog.FogEnd   = 250.0f;									// ここまで離れるとフォグの色で見えなくなる
	g_Fog.FogColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );		// フォグの色
	SetFog(&g_Fog);
	SetFogEnable(g_FogEnable);				// 他の場所もチェックする shadow

}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateLight(void)
{



}

//=============================================================================
// ライトの設定
// Typeによってセットするメンバー変数が変わってくる
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}

void SetLightMatrices(LIGHT* light)
{
	

	XMMATRIX mtxWorld = XMMatrixIdentity();
	XMMATRIX mtxTranslate;

	mtxTranslate = XMMatrixTranslation(light->Position.x, light->Position.y, light->Position.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	
	SetWorldMatrix(&mtxWorld);

	// ビューマトリックス設定
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&light->Position), XMLoadFloat3(&light->View.at), XMLoadFloat3(&light->View.up));
	SetLightViewMatrix(&mtxView);
	XMStoreFloat4x4(&light->View.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&light->View.mtxInvView, mtxInvView);

	// プロジェクションマトリックス設定
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(LIGHT_VIEW_ANGLE, LIGHT_VIEW_ASPECT, LIGHT_VIEW_NEAR_Z, LIGHT_VIEW_FAR_Z);

	SetLightProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&light->View.mtxProjection, mtxProjection);
}

void SetLightMatrices(int index)
{

	LIGHT* light = GetLightData(index);

	SetLightMatrices(light);

}

LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// フォグの設定
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}


BOOL	GetFogEnable(void)
{
	return(g_FogEnable);
}


