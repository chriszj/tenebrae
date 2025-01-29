//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
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
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/enemy/bear_body.obj"		// �ǂݍ��ރ��f����

#define	MODEL_ENEMY_HEAD	"data/MODEL/enemy/bear_head.obj"	// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_LARM	"data/MODEL/enemy/bear_left_arm.obj"	// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_RARM	"data/MODEL/enemy/bear_right_arm.obj"	// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_LLEG	"data/MODEL/enemy/bear_left_foot.obj"	// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_RLEG	"data/MODEL/enemy/bear_right_foot.obj"	// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ENEMY_OFFSET_Y		(6.0f)						// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[

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

// �v���C���[�̊K�w�A�j���[�V�����f�[�^
// �v���C���[�̍��E�p�[�c�𓮂����Ă���A�j���f�[�^
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
// ����������
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

		g_Enemy[i].spd  = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		//g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Enemy[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
		
		g_Enemy[i].use = TRUE;		// TRUE:�����Ă�

		for (int p = 0; p < ENEMY_PARTS_MAX; p++)
		{

			g_Enemy[i].parts[p].use = FALSE;

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_Enemy[i].parts[p].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy[i].parts[p].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy[i].parts[p].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// �e�q�֌W
			g_Enemy[i].parts[p].parentMTXWorld = &g_Enemy[i].mtxWorld;		// �� �����ɐe�̃A�h���X������
			//	g_Parts[�r].parent= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
			//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

				// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			g_Enemy[i].parts[p].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
			g_Enemy[i].parts[p].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Enemy[i].parts[p].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

			// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
			g_Enemy[i].parts[p].load = 0;
		}

		g_Enemy[i].parts[0].use = TRUE;
		g_Enemy[i].parts[0].parentMTXWorld = &g_Enemy[i].mtxWorld;	// �e���Z�b�g
		g_Enemy[i].parts[0].tblNo = 0;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Enemy[i].parts[0].load = 1;
		LoadModel(MODEL_ENEMY_HEAD, &g_Enemy[i].parts[0].model);

		g_Enemy[i].parts[1].use = TRUE;
		g_Enemy[i].parts[1].parentMTXWorld = &g_Enemy[i].mtxWorld;	// �e���Z�b�g
		g_Enemy[i].parts[1].tblNo = 1;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].parts[1].tblMax = sizeof(move_tbl_larm) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Enemy[i].parts[1].load = 1;
		LoadModel(MODEL_ENEMY_LARM, &g_Enemy[i].parts[1].model);

		g_Enemy[i].parts[2].use = TRUE;
		g_Enemy[i].parts[2].parentMTXWorld = &g_Enemy[i].mtxWorld;	// �e���Z�b�g
		g_Enemy[i].parts[2].tblNo = 2;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].parts[2].tblMax = sizeof(move_tbl_rarm) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Enemy[i].parts[2].load = 1;
		LoadModel(MODEL_ENEMY_RARM, &g_Enemy[i].parts[2].model);

		g_Enemy[i].parts[3].use = TRUE;
		g_Enemy[i].parts[3].parentMTXWorld = &g_Enemy[i].mtxWorld;	// �e���Z�b�g
		g_Enemy[i].parts[3].tblNo = 3;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].parts[3].tblMax = sizeof(move_tbl_lleg) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Enemy[i].parts[3].load = 1;
		LoadModel(MODEL_ENEMY_LLEG, &g_Enemy[i].parts[3].model);

		g_Enemy[i].parts[4].use = TRUE;
		g_Enemy[i].parts[4].parentMTXWorld = &g_Enemy[i].mtxWorld;	// �e���Z�b�g
		g_Enemy[i].parts[4].tblNo = 4;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].parts[4].tblMax = sizeof(move_tbl_rleg) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
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

	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[0].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[0].tblNo = 0;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[1].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[1].tblNo = 1;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[2].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[2].tblNo = 2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[2].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g


	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[3].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[3].tblNo = 3;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[3].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[4].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[4].tblNo = 4;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[4].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g


	return S_OK;
}

//=============================================================================
// �I������
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

		// �p�[�c�̉������
		for (int p = 0; p < ENEMY_PARTS_MAX; p++)
		{
			if (g_Enemy[i].parts[p].load == TRUE)
			{
				// �p�[�c�̉������
				UnloadModel(&g_Enemy[i].parts[p].model);
				g_Enemy[i].parts[p].load = FALSE;
			}
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes

			// �ړ�����
			if (g_Enemy[i].tblMax > 0)	// ���`��Ԃ����s����H
			{	// ���`��Ԃ̏���
				int nowNo = (int)g_Enemy[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Enemy[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveEnemyTblAdr[g_Enemy[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Enemy[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);

				// frame���g�Ď��Ԍo�ߏ���������
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Enemy[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Enemy[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

			}

			// �K�w�A�j���[�V����
			for (int p = 0; p < ENEMY_PARTS_MAX; p++)
			{
				// �g���Ă���Ȃ珈������
				if ((g_Enemy[i].parts[p].use == TRUE) && (g_Enemy[i].parts[p].tblMax > 0))
				{	// ���`��Ԃ̏���
					int nowNo = (int)g_Enemy[i].parts[p].time;			// �������ł���e�[�u���ԍ������o���Ă���
					int maxNo = g_Enemy[i].parts[p].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
					int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
					INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].parts[p].tblNo];	// �s���e�[�u���̃A�h���X���擾

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

					float nowTime = g_Enemy[i].parts[p].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

					Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
					Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
					Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

					// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
					XMStoreFloat3(&g_Enemy[i].parts[p].pos, nowPos + Pos);

					// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_Enemy[i].parts[p].rot, nowRot + Rot);

					// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_Enemy[i].parts[p].scl, nowScl + Scl);

					// frame���g�Ď��Ԍo�ߏ���������
					g_Enemy[i].parts[p].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
					if ((int)g_Enemy[i].parts[p].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
					{
						g_Enemy[i].parts[p].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
					}

				}

			}


			// �e���v���C���[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			//SetPositionShadow(g_Enemy[i].shadowIdx, pos);

		}
	}




#ifdef _DEBUG

	if (GetKeyboardTrigger(DIK_P))
	{
		// ���f���̐F��ύX�ł����I�������ɂ��ł����B
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));
		}
	}

	if (GetKeyboardTrigger(DIK_L))
	{
		// ���f���̐F�����ɖ߂��Ă���
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, g_Enemy[0].diffuse[j]);
		}
	}
#endif


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Enemy[i].model);

		// �p�[�c�̊K�w�A�j���[�V����
		for (int p = 0; p < ENEMY_PARTS_MAX; p++)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Enemy[i].parts[p].scl.x, g_Enemy[i].parts[p].scl.y, g_Enemy[i].parts[p].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].parts[p].rot.x, g_Enemy[i].parts[p].rot.y, g_Enemy[i].parts[p].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Enemy[i].parts[p].pos.x, g_Enemy[i].parts[p].pos.y, g_Enemy[i].parts[p].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Enemy[i].parts[p].parentMTXWorld != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(g_Enemy[i].parts[p].parentMTXWorld));
				// ��
				// g_Player.mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_Enemy[i].parts[p].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
			if (g_Enemy[i].parts[p].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// ���f���`��
			DrawModel(&g_Enemy[i].parts[p].model);

		}
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}
