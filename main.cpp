//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "depthrenderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "field.h"
#include "player.h"
#include "enemy.h"
#include "shadow.h"
#include "light.h"
#include "winuser.h"
#include "bullet.h"
#include "sound.h"
#include "fade.h"
#include "title.h"
#include "tutorial.h"
#include "result.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"メッシュ表示"		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
// IMGUI
// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;

static bool g_IMGUIActive = false;

// Our state
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


///IMGUI

long g_MouseX = 0;
long g_MouseY = 0;

int	g_Mode = MODE_GAME;					// 起動時の画面を設定

#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif


//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	ShowCursor(FALSE);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// レンダラーの初期化
	InitRenderer(hInstance, hWnd, bWindow);

	InitDepthRenderer(GetDevice(), hWnd);

	InitSound(hWnd);

	PlaySound(SOUND_LABEL_BGM_inGame);

	// ライトの初期化
	InitLight();

	// カメラの初期化
	InitCamera();

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	InitFade();
	// ライトを有効化
	//SetLightEnable(FALSE);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);

	// 最初のモードをセット
	SetMode(g_Mode);	// ここはSetModeのままで！

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	g_pd3dDevice = GetDevice();
	g_pd3dDeviceContext = GetDeviceContext();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	UninitBullet();

	// フィールドの終了処理
	UninitField();

	// 影の終了処理
	UninitShadow();

	// カメラの終了処理
	UninitCamera();

	//入力の終了処理
	UninitInput();

	StopSound();

	UninitSound();

	UninitFade();

	UninitDepthRenderer();

	// レンダラーの終了処理
	UninitRenderer();

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	// カメラ更新
	UpdateCamera();

	//// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// フィールドの更新処理
	/*UpdateField();

	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	// 影の更新処理
	UpdateShadow();

	UpdateBullet();*/

	if (GetKeyboardTrigger(DIK_I))
	{	
		g_IMGUIActive = !g_IMGUIActive;
		ShowCursor(g_IMGUIActive);
	}

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の更新
		UpdateTitle();
		break;
	case MODE_TUTORIAL:
		UpdateTutorial();
		break;
	case MODE_GAME:			// ゲーム画面の更新

		
		UpdateField();
		
		UpdatePlayer();
		UpdateEnemy();
		UpdateShadow();
		UpdateBullet();

		if (GetFade() == FADE_NONE)
		{	// 全滅チェック
			int ans = CheckGameClear();
			if (ans != 0)
			{
				

				/*float factor = 50 - GetTimeMachineElapsedTime();

				factor /= 50;

				int currentScore = GetScore() * factor;

				SetScore(currentScore);*/

				//SetMode(MODE_RESULT);
				SetFade(FADE_OUT, MODE_RESULT);
				
			}
		}

		break;

	case MODE_RESULT:		// リザルト画面の更新
		UpdateResult();
		break;
	}

	//UpdateGUI();
	UpdateFade();			// フェードの更新処理

	//// Start the Dear ImGui frame
	//ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();

	//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//if (show_demo_window)
	//	ImGui::ShowDemoWindow(&show_demo_window);

	//// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	//{
	//	static float f = 0.0f;
	//	static int counter = 0;

	//	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//	ImGui::Checkbox("Another Window", &show_another_window);

	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//		counter++;
	//	ImGui::SameLine();
	//	ImGui::Text("counter = %d", counter);

	//	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	//	ImGui::End();
	//}

	//// 3. Show another simple window.
	//if (show_another_window)
	//{
	//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//	ImGui::Text("Hello from another window!");
	//	if (ImGui::Button("Close Me"))
	//		show_another_window = false;
	//	ImGui::End();
	//}

	ImGui::EndFrame();

	// Rendering
	//ImGui::Render();
	//const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	//g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
	//g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	ImGui::Render();

	// バックバッファクリア
	Clear();

	int renderingPass = 2;

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の描画
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawTitle();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

		break;
	case MODE_TUTORIAL:
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawTutorial();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	case MODE_GAME:			
		// ゲーム画面の描画

		

		do {

			// ライティングを有効に
			SetLightEnable(TRUE);

			// Z比較あり
			SetDepthEnable(TRUE);


			if (renderingPass > 1) {
				
				float clearcolor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

				SetClearColor(clearcolor);

				SetDepthRenderTarget(GetDeviceContext());
				ClearDepthRenderTarget(GetDeviceContext(), 0.0f, 0.0f, 0.0f, 0.0f);
				
				LIGHT* light = GetLightData(1);

				SetLightMatrices(light);

				SetDepthRenderShaders(GetDeviceContext());

			}
			else {

				ID3D11ShaderResourceView* depthTexture = GetDepthRendererShaderResourceView();

				//D3DX11SaveTextureToFileA(GetDeviceContext(), , "", "");

				// レンディングした深さテクスチャを渡す
				GetDeviceContext()->PSSetShaderResources(1, 1, &depthTexture);

				SetRenderTarget();
				SetRenderShaders();

				

				// プレイヤー視点
				XMFLOAT3 viewpointPos = GetPlayer()->viewPoint;
				XMFLOAT3 pos = GetPlayer()->pos;
				pos.y += 5;
				//pos.z -= 5;

				//pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
				SetFPSCameraAt(viewpointPos, pos);
				SetCamera();
			}

			// フィールドの描画処理
			DrawField();

			// 影の描画処理
			DrawShadow();

			// プレイヤーの描画処理
			DrawPlayer();

			// エネミーの描画処理
			DrawEnemy();

			DrawBullet();

			renderingPass -=1;

		} while (renderingPass > 0);

		break;

	case MODE_RESULT:		// リザルト画面の描画
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawResult();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;
	}

	//DrawGUI();
	DrawFade();

#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// バックバッファ、フロントバッファ入れ替え
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif

//=============================================================================
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	// モードを変える前に全部メモリを解放しちゃう
	StopSound();			// まず曲を止める

	// モードを変える前に全部メモリを解放しちゃう

	// タイトル画面の終了処理
	UninitTitle();

	UninitTutorial();

	// マップチップの終了処理
	UninitField();

	// プレイヤーの終了処理
	UninitPlayer();

	// エネミーの終了処理
	UninitEnemy();


	// リザルトの終了処理
	UninitResult();


	g_Mode = mode;	// 次のモードをセットしている

	switch (g_Mode)
	{
	case MODE_TITLE:
		// タイトル画面の初期化
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_TUTORIAL:
		InitTutorial();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_GAME:
		// ゲーム画面の初期化

		// 影の初期化処理
		InitShadow();

		// フィールドの初期化
		InitField();

		// プレイヤーの初期化
		InitPlayer();

		// エネミーの初期化
		InitEnemy();

		InitBullet();

		PlaySound(SOUND_LABEL_BGM_inGame);
		break;

	case MODE_RESULT:
		InitResult(RESULTTYPE_WIN);
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_MAX:
		break;
	}
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// 全滅チェック
//=============================================================================
int CheckGameClear(void)
{

	// エネミーの数が０ならプレイヤーの勝ちとする
	BOOL cnt = HasReachedGoal();
	if (cnt)
	{
		return 1;	// プレイヤーの勝ち
	}

	// プレイヤーの数が０ならエネミーの勝ちとする
	//cnt = GetPlayerCount();

	//int remainingTime = TIMELIMIT_SECONDS - GetTimeMachineElapsedTime();

	//if (cnt <= 0 || remainingTime < 1)
	//{
	//	return 2;	// エネミーの勝ち
	//}

	return 0;		// ゲーム継続
}

bool IsIMGUIActive(void) {

	return g_IMGUIActive;

}
