//=============================================================================
//
// ���C������ [main.cpp]
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
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"���b�V���\��"		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// �O���[�o���ϐ�:
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

int	g_Mode = MODE_GAME;					// �N�����̉�ʂ�ݒ�

#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif


//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
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
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// �E�B���h�E�̍����W
		CW_USEDEFAULT,																		// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// ����������(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	ShowCursor(FALSE);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//=============================================================================
// �v���V�[�W��
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
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �����_���[�̏�����
	InitRenderer(hInstance, hWnd, bWindow);

	InitDepthRenderer(GetDevice(), hWnd);

	InitSound(hWnd);

	PlaySound(SOUND_LABEL_BGM_inGame);

	// ���C�g�̏�����
	InitLight();

	// �J�����̏�����
	InitCamera();

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	InitFade();
	// ���C�g��L����
	//SetLightEnable(FALSE);

	// �w�ʃ|���S�����J�����O
	SetCullingMode(CULL_MODE_BACK);

	// �ŏ��̃��[�h���Z�b�g
	SetMode(g_Mode);	// ������SetMode�̂܂܂ŁI

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
// �I������
//=============================================================================
void Uninit(void)
{
	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	UninitBullet();

	// �t�B�[���h�̏I������
	UninitField();

	// �e�̏I������
	UninitShadow();

	// �J�����̏I������
	UninitCamera();

	//���͂̏I������
	UninitInput();

	StopSound();

	UninitSound();

	UninitFade();

	UninitDepthRenderer();

	// �����_���[�̏I������
	UninitRenderer();

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	// �J�����X�V
	UpdateCamera();

	//// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// �t�B�[���h�̍X�V����
	/*UpdateField();

	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �G�l�~�[�̍X�V����
	UpdateEnemy();

	// �e�̍X�V����
	UpdateShadow();

	UpdateBullet();*/

	if (GetKeyboardTrigger(DIK_I))
	{	
		g_IMGUIActive = !g_IMGUIActive;
		ShowCursor(g_IMGUIActive);
	}

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̍X�V
		UpdateTitle();
		break;
	case MODE_TUTORIAL:
		UpdateTutorial();
		break;
	case MODE_GAME:			// �Q�[����ʂ̍X�V

		
		UpdateField();
		
		UpdatePlayer();
		UpdateEnemy();
		UpdateShadow();
		UpdateBullet();

		if (GetFade() == FADE_NONE)
		{	// �S�Ń`�F�b�N
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

	case MODE_RESULT:		// ���U���g��ʂ̍X�V
		UpdateResult();
		break;
	}

	//UpdateGUI();
	UpdateFade();			// �t�F�[�h�̍X�V����

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
// �`�揈��
//=============================================================================
void Draw(void)
{
	ImGui::Render();

	// �o�b�N�o�b�t�@�N���A
	Clear();

	int renderingPass = 2;

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̕`��
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawTitle();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;

		break;
	case MODE_TUTORIAL:
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawTutorial();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;

	case MODE_GAME:			
		// �Q�[����ʂ̕`��

		

		do {

			// ���C�e�B���O��L����
			SetLightEnable(TRUE);

			// Z��r����
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

				// �����f�B���O�����[���e�N�X�`����n��
				GetDeviceContext()->PSSetShaderResources(1, 1, &depthTexture);

				SetRenderTarget();
				SetRenderShaders();

				

				// �v���C���[���_
				XMFLOAT3 viewpointPos = GetPlayer()->viewPoint;
				XMFLOAT3 pos = GetPlayer()->pos;
				pos.y += 5;
				//pos.z -= 5;

				//pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
				SetFPSCameraAt(viewpointPos, pos);
				SetCamera();
			}

			// �t�B�[���h�̕`�揈��
			DrawField();

			// �e�̕`�揈��
			DrawShadow();

			// �v���C���[�̕`�揈��
			DrawPlayer();

			// �G�l�~�[�̕`�揈��
			DrawEnemy();

			DrawBullet();

			renderingPass -=1;

		} while (renderingPass > 0);

		break;

	case MODE_RESULT:		// ���U���g��ʂ̕`��
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		DrawResult();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
		break;
	}

	//DrawGUI();
	DrawFade();

#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
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
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// ���[�h��ς���O�ɑS������������������Ⴄ
	StopSound();			// �܂��Ȃ��~�߂�

	// ���[�h��ς���O�ɑS������������������Ⴄ

	// �^�C�g����ʂ̏I������
	UninitTitle();

	UninitTutorial();

	// �}�b�v�`�b�v�̏I������
	UninitField();

	// �v���C���[�̏I������
	UninitPlayer();

	// �G�l�~�[�̏I������
	UninitEnemy();


	// ���U���g�̏I������
	UninitResult();


	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	switch (g_Mode)
	{
	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_TUTORIAL:
		InitTutorial();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_GAME:
		// �Q�[����ʂ̏�����

		// �e�̏���������
		InitShadow();

		// �t�B�[���h�̏�����
		InitField();

		// �v���C���[�̏�����
		InitPlayer();

		// �G�l�~�[�̏�����
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
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// �S�Ń`�F�b�N
//=============================================================================
int CheckGameClear(void)
{

	// �G�l�~�[�̐����O�Ȃ�v���C���[�̏����Ƃ���
	BOOL cnt = HasReachedGoal();
	if (cnt)
	{
		return 1;	// �v���C���[�̏���
	}

	// �v���C���[�̐����O�Ȃ�G�l�~�[�̏����Ƃ���
	//cnt = GetPlayerCount();

	//int remainingTime = TIMELIMIT_SECONDS - GetTimeMachineElapsedTime();

	//if (cnt <= 0 || remainingTime < 1)
	//{
	//	return 2;	// �G�l�~�[�̏���
	//}

	return 0;		// �Q�[���p��
}

bool IsIMGUIActive(void) {

	return g_IMGUIActive;

}
