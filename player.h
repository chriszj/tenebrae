//=============================================================================
//
// ���f������ [player.h]
// Author : 
//
//=============================================================================
#pragma once

#include "collision.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER		(1)					// �v���C���[�̐�

#define	PLAYER_SIZE		(5.0f)				// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3		pos;		// �|���S���̈ʒu
	XMFLOAT3		rot;		// �|���S���̌���(��])
	XMFLOAT3		scl;		// �|���S���̑傫��(�X�P�[��)

	XMFLOAT3        viewPoint;
	float           viewDirX, viewDirY;

	XMFLOAT4X4		mtxWorld;	// ���[���h�}�g���b�N�X

	BOOL			load;
	DX11_MODEL		model;		// ���f�����
	XMFLOAT4		diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	XMFLOAT3		armPos;		// �|���S���̈ʒu
	XMFLOAT3		armRot;		// �|���S���̌���(��])
	XMFLOAT3		armScl;		// �|���S���̑傫��(�X�P�[��)

	DX11_MODEL      armModel;
	XMFLOAT4        armModelDiffuse[MODEL_MAX_MATERIAL];

	float			spd;		// �ړ��X�s�[�h
	float           movX, movY;
	float			dir;		// ����
	float			size;		// �����蔻��̑傫��
	int				shadowIdx;	// �e��Index
	BOOL			use;

	COLLIDER2DBOX collider;       // �����蔻��̉��P�̂���


};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

BOOL HasReachedGoal(void);
