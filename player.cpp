//=============================================================================
//
// ���f������ [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "sound.h"
#include "field.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/bear/bear.obj"			// �ǂݍ��ރ��f����
#define MODEL_ARM_PLAYER    "data/MODEL/bear/bear_pistol.obj"

#define	VALUE_MOVE			(2.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define PLAYER_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(5.0f)							// �v���C���[�̑��������킹��

#define PLAYER_COLLIDER_WIDTH       (10.0f)
#define PLAYER_COLLIDER_HEIGHT      (10.0f)
#define PLAYER_COLLIDER_OFFSETX     (0.0f)
#define PLAYER_COLLIDER_OFFSETY     (0.0f)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER				g_Player;						// �v���C���[

long lastMouseX;
long lastMouseY;

static MAPOBJECT* g_CheckPoints;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	LoadModel(MODEL_ARM_PLAYER, &g_Player.armModel);

	g_Player.load = TRUE;

	XMFLOAT3 startPosition = XMFLOAT3(400.0f, 400.0f, 0.0f);

	g_CheckPoints = GetMapObjectsFromLayer(MAPOBJLAYER_LOCATIONS);

	if (g_CheckPoints != NULL)
	{
		startPosition = XMFLOAT3((g_CheckPoints[0].x), (g_CheckPoints[0].y), 0.0f);
	}

	g_Player.pos = { startPosition.x, PLAYER_OFFSET_Y, startPosition.y };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 0.3f, 0.3f, 0.3f };

	g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��

	g_Player.armPos = { 2.0f, PLAYER_OFFSET_Y, 0.0f };
	g_Player.armRot = { 0.0f, 0.0f, 0.0f };
	g_Player.armScl = { 0.3f, 0.3f, 0.3f };

	g_Player.collider = COLLIDER2DBOX(PLAYER_COLLIDER_OFFSETX, PLAYER_COLLIDER_OFFSETY, PLAYER_COLLIDER_WIDTH, PLAYER_COLLIDER_HEIGHT);

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�
	GetModelDiffuse(&g_Player.model, &g_Player.diffuse[0]);

	GetModelDiffuse(&g_Player.armModel, &g_Player.armModelDiffuse[0]);

	g_Player.use = TRUE;


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// ���f���̉������
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);

		UnloadModel(&g_Player.armModel);
		g_Player.load = FALSE;
	}


}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA* cam = GetCamera();
	g_Player.dir = 0;
	// �ړ��������Ⴄ
	

	if (GetKeyboardPress(DIK_A))
	{	// ���ֈړ�
		g_Player.spd = VALUE_MOVE;
		//g_Player.dir -= XM_PI / 2;
		g_Player.movY = -1;
	}
	if (GetKeyboardPress(DIK_D))
	{	// �E�ֈړ�
		g_Player.spd = VALUE_MOVE;
		//g_Player.dir += XM_PI / 4;
		g_Player.movY = 1;
	}
	if (GetKeyboardPress(DIK_W))
	{	// ��ֈړ�
		g_Player.spd = VALUE_MOVE;
		//g_Player.dir -= XM_PI / 2;
		g_Player.movX = 1;
	}
	if (GetKeyboardPress(DIK_S))
	{	// ���ֈړ�
		g_Player.spd = VALUE_MOVE;
		//g_Player.dir += (1 * XM_PI)/4;
		g_Player.movX = -1;
	}

	g_Player.dir = atan2f(g_Player.movY, g_Player.movX);


#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}
#endif

	POINT pt;

	GetCursorPos(&pt);

	long currentMouseX = pt.x;
	long currentMouseY = pt.y;

	long diffX = currentMouseX - lastMouseX;
	long diffY = currentMouseY - lastMouseY;

	//g_Camera.rot.y += (diffX * 0.03f);
	//g_Camera.rot.z += (diffY * 0.03f);

	//g_Player.viewPoint.x

	g_Player.viewDirX -= diffX * 0.005f;
	g_Player.viewDirY += diffY * 0.005f;

	if (g_Player.viewDirY >= XM_PI * 1.3)
		g_Player.viewDirY = XM_PI * 1.3;

	if (g_Player.viewDirY < XM_PI*0.6)
		g_Player.viewDirY = XM_PI*0.6;

	g_Player.viewPoint.x = g_Player.pos.x + (cos(g_Player.viewDirX) * cos(g_Player.viewDirY) * 50);
	g_Player.viewPoint.z = g_Player.pos.z + (sin(g_Player.viewDirX) * cos(g_Player.viewDirY) * 50);
	g_Player.viewPoint.y = g_Player.pos.y + (sin(g_Player.viewDirY) * 50);

	// Math.atan2( x * Math.cos(rotx), z )

	float rotx = atan2f(g_Player.viewPoint.y - g_Player.pos.y, g_Player.viewPoint.z - g_Player.pos.z);
	float roty = atan2f((g_Player.viewPoint.x - g_Player.pos.x) * cosf(g_Player.rot.x), g_Player.viewPoint.z - g_Player.pos.z);// g_Player.viewPoint.y;
	float rotz = atan2f(cosf(rotx), sinf(rotx) * sinf(roty));

	//g_Player.rot.x = rotx + XM_PI;
	g_Player.rot.y = roty + XM_PI; //+ g_Player.dir;
	//g_Player.rot.z = rotz + XM_PI;

	/*g_Player.armRot.x = rotx;
	g_Player.armRot.y = roty + XM_PI;*/

	SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	GetCursorPos(&pt);

	currentMouseX = pt.x;
	currentMouseY = pt.y;

	lastMouseX = currentMouseX;
	lastMouseY = currentMouseY;

	g_Player.armPos.x = g_Player.pos.x + (cos(g_Player.viewDirX) * cos(g_Player.viewDirY) * 10);
	g_Player.armPos.z = g_Player.pos.z + (sin(g_Player.viewDirX) * cos(g_Player.viewDirY) * 10);
	g_Player.armPos.y = g_Player.pos.y + (sin(g_Player.viewDirY) * 10);


	//g_Player.armRot.z = rotz;

	//	// Key���͂���������ړ���������
	if (g_Player.spd > 0.0f)
	{
		//g_Player.rot.y = g_Player.dir + cam->rot.y;

		//�@�t�B�[���h�̓����蔻��
		MAPOBJECT* walls = GetMapObjectsFromLayer(MAPOBJLAYER_WALL);

		XMFLOAT3 newXPos = XMFLOAT3(g_Player.pos);
		XMFLOAT3 newZPos = XMFLOAT3(g_Player.pos);
		newXPos.x -= sinf(g_Player.rot.y + g_Player.dir) * g_Player.spd;
		newXPos.y = newXPos.z;
		newZPos.z -= cosf(g_Player.rot.y + g_Player.dir) * g_Player.spd;
		newZPos.y = newZPos.z;

		for (int w = 0; w < MAP_OBJECTS_PER_LAYER_MAX; w++)
		{
			XMFLOAT3 wallPos = XMFLOAT3(walls[w].x, walls[w].y, 0.0f);
			COLLIDER2DBOX wallCollider = COLLIDER2DBOX(0.0f, 0.0f, walls[w].width, walls[w].height);

			// X���̓����蔻��
			BOOL ansX = CollisionBB(newXPos, g_Player.collider, wallPos, wallCollider);

			if (ansX)
			{
				//g_Player.move.x = 0;
				newXPos.x = g_Player.pos.x;
			}

			// Y���̓����蔻��
			BOOL ansY = CollisionBB(newZPos, g_Player.collider, wallPos, wallCollider);

			if (ansY)
			{
				//g_Player[i].move.y = 0;
				newZPos.z = g_Player.pos.z;
			}


		}

		// ���͂̂����������փv���C���[���������Ĉړ�������
		g_Player.pos.x = newXPos.x;//-= sinf(g_Player.rot.y + g_Player.dir) * g_Player.spd;
		g_Player.pos.z = newZPos.z;//-= cosf(g_Player.rot.y + g_Player.dir) * g_Player.spd;
	}

	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);


	g_Player.spd *= 0.8f;
	g_Player.movX *= 0.8f;
	g_Player.movY *= 0.8f;

	if (GetKeyboardTrigger(DIK_SPACE)) {

		XMFLOAT3 dir = XMFLOAT3(g_Player.viewPoint.x - g_Player.pos.x, g_Player.viewPoint.y - g_Player.pos.y, g_Player.viewPoint.z - g_Player.pos.z);
	

		XMFLOAT2 way = XMFLOAT2(g_Player.viewDirX, g_Player.viewDirY);

		SetBullet(g_Player.pos, dir, way);

		PlaySound(SOUND_LABEL_SE_shot000);
	}


	//g_Player.rot.y = cam->rot.y;

	{	// �|�C���g���C�g�̃e�X�g
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 30.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.5f, 1.5f, 1.5f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Player:�� �� �� ���@Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	/* { // �e

		// ���f���̐F��ύX�ł����I�������ɂ��ł����B
		for (int j = 0; j < g_Player.model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Player.model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.3f));
		}

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Player.scl.x, 0.1f, g_Player.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y - PLAYER_OFFSET_Y, g_Player.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Player.model);
	}*/

	{

		// ���f���̐F��ύX�ł����I�������ɂ��ł����B
		/*for (int j = 0; j < g_Player.model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Player.model, j, g_Player.diffuse[j]);
		}*/

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Player.model);
	}

	// Arm
	{

		// ���f���̐F��ύX�ł����I�������ɂ��ł����B
		/*for (int j = 0; j < g_Player.model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Player.model, j, g_Player.diffuse[j]);
		}*/
		CAMERA* cam = GetCamera();
		
		


		// �Ȃɂ�����Ƃ���
		// �����s��
		

		XMMATRIX mtxScl, mtxView, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		mtxView = XMLoadFloat4x4(&cam->mtxView);

		mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
		mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
		mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

		mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
		mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
		mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

		mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
		mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
		mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

		XMVECTOR vector = { g_Player.armRot.x, g_Player.armRot.y, g_Player.armRot.z};

		XMVector3Transform(vector, mtxWorld);
		XMStoreFloat3(&g_Player.armRot, vector);

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Player.armScl.x, g_Player.armScl.y, g_Player.armScl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		/*mtxRot = XMMatrixRotationRollPitchYaw(g_Player.armRot.x, g_Player.armRot.y + XM_PI, g_Player.armRot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);*/

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Player.armPos.x, g_Player.armPos.y, g_Player.armPos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Player.armModel);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

