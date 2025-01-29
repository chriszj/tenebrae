//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "shadow.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/enemy/bear_body.obj"		// 読み込むモデル名

#define	MODEL_ENEMY_HEAD	"data/MODEL/enemy/bear_head.obj"	// 読み込むモデル名
#define	MODEL_ENEMY_LARM	"data/MODEL/enemy/bear_left_arm.obj"	// 読み込むモデル名
#define	MODEL_ENEMY_RARM	"data/MODEL/enemy/bear_right_arm.obj"	// 読み込むモデル名
#define	MODEL_ENEMY_LLEG	"data/MODEL/enemy/bear_left_foot.obj"	// 読み込むモデル名
#define	MODEL_ENEMY_RLEG	"data/MODEL/enemy/bear_right_foot.obj"	// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(6.0f)						// エネミーの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー

int g_Enemy_load = 0;

static INTERPOLATION_DATA g_MoveTbl0[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-25.0f, ENEMY_OFFSET_Y,  114.0f), XMFLOAT3(0.0f, XMConvertToRadians(90.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 },
	{ XMFLOAT3(30.0f,  ENEMY_OFFSET_Y,  114.0f), XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 },
	{ XMFLOAT3( 30.0f, ENEMY_OFFSET_Y,  114.0f), XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 },
	{ XMFLOAT3(-25.0f, ENEMY_OFFSET_Y,  114.0f), XMFLOAT3(0.0f, XMConvertToRadians(90.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 }
};

static INTERPOLATION_DATA g_MoveTbl1[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y,  -30.0f), XMFLOAT3(0.0f, XMConvertToRadians(45.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(20.0f, ENEMY_OFFSET_Y,  36.0f), XMFLOAT3(0.0f, XMConvertToRadians(-45.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y,  36.0f), XMFLOAT3(0.0f, XMConvertToRadians(90.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-20.0f, ENEMY_OFFSET_Y, -30.0f), XMFLOAT3(0.0f, XMConvertToRadians(90.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }
};

static INTERPOLATION_DATA g_MoveTbl2[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y,  266.0f), XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y,  323.0f), XMFLOAT3(0.0f, XMConvertToRadians(180.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-145.0f, ENEMY_OFFSET_Y,  323.0f), XMFLOAT3(0.0f, XMConvertToRadians(90.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-145.0f, ENEMY_OFFSET_Y, 266.0f), XMFLOAT3(0.0f, XMConvertToRadians(0.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }
};

static INTERPOLATION_DATA g_MoveTbl3[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(66.0f, ENEMY_OFFSET_Y,  440.0f), XMFLOAT3(0.0f, XMConvertToRadians(0.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 },
	{ XMFLOAT3(66.0f,  ENEMY_OFFSET_Y,  490.0f), XMFLOAT3(0.0f, XMConvertToRadians(180.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 },
	{ XMFLOAT3(66.0f, ENEMY_OFFSET_Y,  490.0f), XMFLOAT3(0.0f, XMConvertToRadians(180.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 },
	{ XMFLOAT3(66.0f, ENEMY_OFFSET_Y,  440.0f), XMFLOAT3(0.0f, XMConvertToRadians(0.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 }
};

static INTERPOLATION_DATA g_MoveTbl4[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(190.0f, ENEMY_OFFSET_Y,  565.0f), XMFLOAT3(0.0f, XMConvertToRadians(0.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 },
	{ XMFLOAT3(190.0f,  ENEMY_OFFSET_Y,  615.0f), XMFLOAT3(0.0f, XMConvertToRadians(180.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 },
	{ XMFLOAT3(190.0f, ENEMY_OFFSET_Y,  615.0f), XMFLOAT3(0.0f, XMConvertToRadians(180.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 },
	{ XMFLOAT3(190.0f, ENEMY_OFFSET_Y,  565.0f), XMFLOAT3(0.0f, XMConvertToRadians(0.0f), 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 1 }
};


static INTERPOLATION_DATA* g_MoveEnemyTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,
	g_MoveTbl2,
	g_MoveTbl3,
	g_MoveTbl4

};

// プレイヤーの階層アニメーションデータ
// プレイヤーの左右パーツを動かしているアニメデータ
static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 2.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, XM_PI / 30),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, 2.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -XM_PI / 30),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_larm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-3.0f, 14.0f, 0.0f), XMFLOAT3(0.0f, -0.5f, -1.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-3.0f, 14.0f, 0.0f), XMFLOAT3(0.0f, 0.5f, -1.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_rarm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(3.0f, 14.0f, 0.0f), XMFLOAT3(0.0f, -0.5f, 1.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(3.0f, 14.0f, 0.0f), XMFLOAT3(0.0f, 0.5f, 1.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_lleg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, -2.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, -2.0f, 0.0f), XMFLOAT3(-XM_PI / 4, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_rleg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, -2.0f, 0.0f), XMFLOAT3(-XM_PI / 4, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, -2.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_head,
	move_tbl_larm,
	move_tbl_rarm,
	move_tbl_lleg,
	move_tbl_rleg,
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, 7.0f, 20.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(0.3f, 0.3f, 0.3f);

		g_Enemy[i].spd  = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		//g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Enemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Enemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット
		
		g_Enemy[i].use = TRUE;		// TRUE:生きてる

		for (int p = 0; p < ENEMY_PARTS_MAX; p++)
		{

			g_Enemy[i].parts[p].use = FALSE;

			// 位置・回転・スケールの初期設定
			g_Enemy[i].parts[p].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy[i].parts[p].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy[i].parts[p].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// 親子関係
			g_Enemy[i].parts[p].parentMTXWorld = &g_Enemy[i].mtxWorld;		// ← ここに親のアドレスを入れる
			//	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
			//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

				// 階層アニメーション用のメンバー変数の初期化
			g_Enemy[i].parts[p].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_Enemy[i].parts[p].tblNo = 0;			// 再生する行動データテーブルNoをセット
			g_Enemy[i].parts[p].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

			// パーツの読み込みはまだしていない
			g_Enemy[i].parts[p].load = 0;
		}

		g_Enemy[i].parts[0].use = TRUE;
		g_Enemy[i].parts[0].parentMTXWorld = &g_Enemy[i].mtxWorld;	// 親をセット
		g_Enemy[i].parts[0].tblNo = 0;			// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Enemy[i].parts[0].load = 1;
		LoadModel(MODEL_ENEMY_HEAD, &g_Enemy[i].parts[0].model);

		g_Enemy[i].parts[1].use = TRUE;
		g_Enemy[i].parts[1].parentMTXWorld = &g_Enemy[i].mtxWorld;	// 親をセット
		g_Enemy[i].parts[1].tblNo = 1;			// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].parts[1].tblMax = sizeof(move_tbl_larm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Enemy[i].parts[1].load = 1;
		LoadModel(MODEL_ENEMY_LARM, &g_Enemy[i].parts[1].model);

		g_Enemy[i].parts[2].use = TRUE;
		g_Enemy[i].parts[2].parentMTXWorld = &g_Enemy[i].mtxWorld;	// 親をセット
		g_Enemy[i].parts[2].tblNo = 2;			// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].parts[2].tblMax = sizeof(move_tbl_rarm) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Enemy[i].parts[2].load = 1;
		LoadModel(MODEL_ENEMY_RARM, &g_Enemy[i].parts[2].model);

		g_Enemy[i].parts[3].use = TRUE;
		g_Enemy[i].parts[3].parentMTXWorld = &g_Enemy[i].mtxWorld;	// 親をセット
		g_Enemy[i].parts[3].tblNo = 3;			// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].parts[3].tblMax = sizeof(move_tbl_lleg) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Enemy[i].parts[3].load = 1;
		LoadModel(MODEL_ENEMY_LLEG, &g_Enemy[i].parts[3].model);

		g_Enemy[i].parts[4].use = TRUE;
		g_Enemy[i].parts[4].parentMTXWorld = &g_Enemy[i].mtxWorld;	// 親をセット
		g_Enemy[i].parts[4].tblNo = 4;			// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].parts[4].tblMax = sizeof(move_tbl_rleg) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Enemy[i].parts[4].load = 1;
		LoadModel(MODEL_ENEMY_RLEG, &g_Enemy[i].parts[4].model);

		for each (ENEMY_PART part in g_Enemy[i].parts)
		{
			if (!part.load) {
			
				g_Enemy[i].load = FALSE;
				break;
			}
		}

	}

	// 0番だけ線形補間で動かしてみる
	g_Enemy[0].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[0].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 0番だけ線形補間で動かしてみる
	g_Enemy[1].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[1].tblNo = 1;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 0番だけ線形補間で動かしてみる
	g_Enemy[2].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[2].tblNo = 2;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[2].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット


	// 0番だけ線形補間で動かしてみる
	g_Enemy[3].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[3].tblNo = 3;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[3].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 0番だけ線形補間で動かしてみる
	g_Enemy[4].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[4].tblNo = 4;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[4].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}

		// パーツの解放処理
		for (int p = 0; p < ENEMY_PARTS_MAX; p++)
		{
			if (g_Enemy[i].parts[p].load == TRUE)
			{
				// パーツの解放処理
				UnloadModel(&g_Enemy[i].parts[p].model);
				g_Enemy[i].parts[p].load = FALSE;
			}
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	// エネミーを動かす場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes

			// 移動処理
			if (g_Enemy[i].tblMax > 0)	// 線形補間を実行する？
			{	// 線形補間の処理
				int nowNo = (int)g_Enemy[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Enemy[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveEnemyTblAdr[g_Enemy[i].tblNo];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Enemy[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);

				// frameを使て時間経過処理をする
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Enemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Enemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				}

			}

			// 階層アニメーション
			for (int p = 0; p < ENEMY_PARTS_MAX; p++)
			{
				// 使われているなら処理する
				if ((g_Enemy[i].parts[p].use == TRUE) && (g_Enemy[i].parts[p].tblMax > 0))
				{	// 線形補間の処理
					int nowNo = (int)g_Enemy[i].parts[p].time;			// 整数分であるテーブル番号を取り出している
					int maxNo = g_Enemy[i].parts[p].tblMax;				// 登録テーブル数を数えている
					int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
					INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].parts[p].tblNo];	// 行動テーブルのアドレスを取得

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

					float nowTime = g_Enemy[i].parts[p].time - nowNo;	// 時間部分である少数を取り出している

					Pos *= nowTime;								// 現在の移動量を計算している
					Rot *= nowTime;								// 現在の回転量を計算している
					Scl *= nowTime;								// 現在の拡大率を計算している

					// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
					XMStoreFloat3(&g_Enemy[i].parts[p].pos, nowPos + Pos);

					// 計算して求めた回転量を現在の移動テーブルに足している
					XMStoreFloat3(&g_Enemy[i].parts[p].rot, nowRot + Rot);

					// 計算して求めた拡大率を現在の移動テーブルに足している
					XMStoreFloat3(&g_Enemy[i].parts[p].scl, nowScl + Scl);

					// frameを使て時間経過処理をする
					g_Enemy[i].parts[p].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
					if ((int)g_Enemy[i].parts[p].time >= maxNo)			// 登録テーブル最後まで移動したか？
					{
						g_Enemy[i].parts[p].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
					}

				}

			}


			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			//SetPositionShadow(g_Enemy[i].shadowIdx, pos);

		}
	}




#ifdef _DEBUG

	if (GetKeyboardTrigger(DIK_P))
	{
		// モデルの色を変更できるよ！半透明にもできるよ。
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));
		}
	}

	if (GetKeyboardTrigger(DIK_L))
	{
		// モデルの色を元に戻している
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, g_Enemy[0].diffuse[j]);
		}
	}
#endif


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Enemy[i].model);

		// パーツの階層アニメーション
		for (int p = 0; p < ENEMY_PARTS_MAX; p++)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Enemy[i].parts[p].scl.x, g_Enemy[i].parts[p].scl.y, g_Enemy[i].parts[p].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].parts[p].rot.x, g_Enemy[i].parts[p].rot.y, g_Enemy[i].parts[p].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Enemy[i].parts[p].pos.x, g_Enemy[i].parts[p].pos.y, g_Enemy[i].parts[p].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Enemy[i].parts[p].parentMTXWorld != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(g_Enemy[i].parts[p].parentMTXWorld));
				// ↑
				// g_Player.mtxWorldを指している
			}

			XMStoreFloat4x4(&g_Enemy[i].parts[p].mtxWorld, mtxWorld);

			// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
			if (g_Enemy[i].parts[p].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// モデル描画
			DrawModel(&g_Enemy[i].parts[p].model);

		}
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}
