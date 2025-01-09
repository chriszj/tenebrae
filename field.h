//=============================================================================
//
// �n�ʏ��� [field.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
struct FIELD
{
	XMFLOAT3		pos;		// �|���S���̈ʒu
	XMFLOAT3		rot;		// �|���S���̌���(��])
	XMFLOAT3		scl;		// �|���S���̑傫��(�X�P�[��)

	XMFLOAT4X4		mtxWorld;   // ���[���h�}�g���b�N�X
	DX11_MODEL		fieldWall1Model;		// ���f�����
	DX11_MODEL      fieldWall2Model;
	DX11_MODEL      fieldPillarsModel;
	DX11_MODEL      fieldPillarCornersModel;
	DX11_MODEL      fieldWallDoorsModel;
	DX11_MODEL      fieldWallGroundModel;
	DX11_MODEL      fieldWallProps1Model;
	DX11_MODEL      fieldWallProps2Model;
	DX11_MODEL      fieldWallProps3Model;
	DX11_MODEL      fieldWallProps4Model;

	XMFLOAT4		diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
	BOOL            load;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitField(void);
void UninitField(void);
void UpdateField(void);
void DrawField(void);

