//=============================================================================
//
// フィールド表示処理 [field.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "field.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_FIELD_WALL1		"data/MODEL/level/lvl-0-wall1.obj"// 読み込むモデル名
#define MODEL_FIELD_WALL2		"data/MODEL/level/lvl-0-wall2.obj"// 読み込むモデル名
#define MODEL_FIELD_PILLARS		"data/MODEL/level/lvl-0-pillars.obj"// 読み込むモデル名
#define MODEL_FIELD_PILLARCORNERS		"data/MODEL/level/lvl-0-pillarscorner.obj"// 読み込むモデル名
#define MODEL_FIELD_WALLDOORS		"data/MODEL/level/lvl-0-walldoors.obj"// 読み込むモデル名
#define MODEL_FIELD_GROUND		"data/MODEL/level/lvl-0-ground.obj"// 読み込むモデル名
#define MODEL_FIELD_PROPS1		"data/MODEL/level/lvl-0-props1.obj"// 読み込むモデル名
#define MODEL_FIELD_PROPS2		"data/MODEL/level/lvl-0-props2.obj"// 読み込むモデル名
#define MODEL_FIELD_PROPS3		"data/MODEL/level/lvl-0-props3.obj"// 読み込むモデル名
#define MODEL_FIELD_PROPS4		"data/MODEL/level/lvl-0-props4.obj"// 読み込むモデル名

#define TEXTURE_MAX		(1)						// テクスチャの数

#define MAX_POLYGON		(6)						// キューブ１個あたりの面数

#define	VALUE_MOVE		(5.0f)					// 移動量
#define	VALUE_ROTATE	(XM_PI * 0.02f)			// 回転量

#define	SIZE_WH			(100.0f)				// 地面のサイズ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexField(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点情報
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static FIELD						g_Field;				// ポリゴンデータ
static int							g_TexNo;				// テクスチャ番号

static char* g_TextureName[] = {
	"data/TEXTURE/field004.jpg",
};

// 頂点配列
static VERTEX_3D g_VertexArray[4 * MAX_POLYGON] = {
	{XMFLOAT3(-SIZE_WH, 0.0f,  SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
	{XMFLOAT3( SIZE_WH, 0.0f,  SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
	{XMFLOAT3(-SIZE_WH, 0.0f, -SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
	{XMFLOAT3( SIZE_WH, 0.0f, -SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitField(void)
{
	// 頂点バッファの作成
	//MakeVertexField();
	LoadModel(MODEL_FIELD_WALL1, &g_Field.fieldWall1Model);
	LoadModel(MODEL_FIELD_WALL2, &g_Field.fieldWall2Model);
	LoadModel(MODEL_FIELD_PILLARS, &g_Field.fieldPillarsModel);
	LoadModel(MODEL_FIELD_PILLARCORNERS, &g_Field.fieldPillarCornersModel);
	LoadModel(MODEL_FIELD_WALLDOORS, &g_Field.fieldWallDoorsModel);
	LoadModel(MODEL_FIELD_GROUND, &g_Field.fieldWallGroundModel);
	LoadModel(MODEL_FIELD_PROPS1, &g_Field.fieldWallProps1Model);
	LoadModel(MODEL_FIELD_PROPS2, &g_Field.fieldWallProps2Model);
	LoadModel(MODEL_FIELD_PROPS3, &g_Field.fieldWallProps3Model);
	LoadModel(MODEL_FIELD_PROPS4, &g_Field.fieldWallProps4Model);

	g_Field.load = TRUE;

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 位置・回転・スケールの初期設定
	g_Field.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.scl = XMFLOAT3(0.045f, 0.045f, 0.045f);

	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitField(void)
{
	// 頂点バッファの解放
	//if (g_VertexBuffer) {
	//	g_VertexBuffer->Release();
	//	g_VertexBuffer = NULL;
	//}

	//// テクスチャの解放
	//for (int i = 0; i < TEXTURE_MAX; i++)
	//{
	//	if (g_Texture[i])
	//	{
	//		g_Texture[i]->Release();
	//		g_Texture[i] = NULL;
	//	}
	//}
	UnloadModel(&g_Field.fieldWall1Model);
	UnloadModel(&g_Field.fieldWall2Model);
	UnloadModel(&g_Field.fieldPillarsModel);
	UnloadModel(&g_Field.fieldPillarCornersModel);
	UnloadModel(&g_Field.fieldWallDoorsModel);
	UnloadModel(&g_Field.fieldWallGroundModel);
	UnloadModel(&g_Field.fieldWallProps1Model);
	UnloadModel(&g_Field.fieldWallProps2Model);
	UnloadModel(&g_Field.fieldWallProps3Model);
	UnloadModel(&g_Field.fieldWallProps4Model);

	g_Field.load = FALSE;

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateField(void)
{
//	CAMERA *cam = GetCamera();
//
//	// 回転させちゃう
//	if (GetKeyboardPress(DIK_LEFT))
//	{
//		g_Field.rot.y -= VALUE_ROTATE;
//	}
//	if (GetKeyboardPress(DIK_RIGHT))
//	{
//		g_Field.rot.y += VALUE_ROTATE;
//	}
//	if (GetKeyboardPress(DIK_UP))
//	{
//		g_Field.rot.x -= VALUE_ROTATE;
//	}
//	if (GetKeyboardPress(DIK_DOWN))
//	{
//		g_Field.rot.x += VALUE_ROTATE;
//	}
//
//	if (GetKeyboardPress(DIK_SPACE))
//	{
//		g_Field.rot.y = g_Field.rot.x = 0.0f;
//	}
//
//#ifdef _DEBUG	// デバッグ情報を表示する
//
//	PrintDebugProc("Field:↑→↓←\n");
//
//#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawField(void)
{
	// 頂点バッファ設定
	//UINT stride = sizeof(VERTEX_3D);
	//UINT offset = 0;
	//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	//// プリミティブトポロジ設定
	//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//// マテリアル設定
	//MATERIAL material;
	//ZeroMemory(&material, sizeof(material));
	//material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//SetMaterial(material);

	//// テクスチャ設定
	//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);


	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Field.scl.x, g_Field.scl.y, g_Field.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Field.rot.x, g_Field.rot.y, g_Field.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Field.pos.x, g_Field.pos.y, g_Field.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Field.mtxWorld, mtxWorld);


	// ポリゴン描画
	//GetDeviceContext()->Draw(4, 0);		// 4頂点分を0番目の頂点番号から描画
	DrawModel(&g_Field.fieldWall1Model);
	DrawModel(&g_Field.fieldWall2Model);
	DrawModel(&g_Field.fieldPillarsModel);
	DrawModel(&g_Field.fieldPillarCornersModel);
	DrawModel(&g_Field.fieldWallDoorsModel);
	DrawModel(&g_Field.fieldWallGroundModel);
	DrawModel(&g_Field.fieldWallProps1Model);
	DrawModel(&g_Field.fieldWallProps2Model);
	DrawModel(&g_Field.fieldWallProps3Model);
	DrawModel(&g_Field.fieldWallProps4Model);

}


HRESULT MakeVertexField(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4 * MAX_POLYGON;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// グローバルの頂点配列から頂点バッファをつくる
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = g_VertexArray;

	GetDevice()->CreateBuffer(&bd, &sd, &g_VertexBuffer);

	return S_OK;
}