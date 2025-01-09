//=============================================================================
//
// �t�B�[���h�\������ [field.cpp]
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
// �}�N����`
//*****************************************************************************
#define	MODEL_FIELD_WALL1		"data/MODEL/level/lvl-0-wall1.obj"// �ǂݍ��ރ��f����
#define MODEL_FIELD_WALL2		"data/MODEL/level/lvl-0-wall2.obj"// �ǂݍ��ރ��f����
#define MODEL_FIELD_PILLARS		"data/MODEL/level/lvl-0-pillars.obj"// �ǂݍ��ރ��f����
#define MODEL_FIELD_PILLARCORNERS		"data/MODEL/level/lvl-0-pillarscorner.obj"// �ǂݍ��ރ��f����
#define MODEL_FIELD_WALLDOORS		"data/MODEL/level/lvl-0-walldoors.obj"// �ǂݍ��ރ��f����
#define MODEL_FIELD_GROUND		"data/MODEL/level/lvl-0-ground.obj"// �ǂݍ��ރ��f����
#define MODEL_FIELD_PROPS1		"data/MODEL/level/lvl-0-props1.obj"// �ǂݍ��ރ��f����
#define MODEL_FIELD_PROPS2		"data/MODEL/level/lvl-0-props2.obj"// �ǂݍ��ރ��f����
#define MODEL_FIELD_PROPS3		"data/MODEL/level/lvl-0-props3.obj"// �ǂݍ��ރ��f����
#define MODEL_FIELD_PROPS4		"data/MODEL/level/lvl-0-props4.obj"// �ǂݍ��ރ��f����

#define TEXTURE_MAX		(1)						// �e�N�X�`���̐�

#define MAX_POLYGON		(6)						// �L���[�u�P������̖ʐ�

#define	VALUE_MOVE		(5.0f)					// �ړ���
#define	VALUE_ROTATE	(XM_PI * 0.02f)			// ��]��

#define	SIZE_WH			(100.0f)				// �n�ʂ̃T�C�Y

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexField(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_���
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static FIELD						g_Field;				// �|���S���f�[�^
static int							g_TexNo;				// �e�N�X�`���ԍ�

static char* g_TextureName[] = {
	"data/TEXTURE/field004.jpg",
};

// ���_�z��
static VERTEX_3D g_VertexArray[4 * MAX_POLYGON] = {
	{XMFLOAT3(-SIZE_WH, 0.0f,  SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
	{XMFLOAT3( SIZE_WH, 0.0f,  SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
	{XMFLOAT3(-SIZE_WH, 0.0f, -SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
	{XMFLOAT3( SIZE_WH, 0.0f, -SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitField(void)
{
	// ���_�o�b�t�@�̍쐬
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

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	g_Field.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.scl = XMFLOAT3(0.045f, 0.045f, 0.045f);

	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitField(void)
{
	// ���_�o�b�t�@�̉��
	//if (g_VertexBuffer) {
	//	g_VertexBuffer->Release();
	//	g_VertexBuffer = NULL;
	//}

	//// �e�N�X�`���̉��
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
// �X�V����
//=============================================================================
void UpdateField(void)
{
//	CAMERA *cam = GetCamera();
//
//	// ��]�������Ⴄ
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
//#ifdef _DEBUG	// �f�o�b�O����\������
//
//	PrintDebugProc("Field:��������\n");
//
//#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawField(void)
{
	// ���_�o�b�t�@�ݒ�
	//UINT stride = sizeof(VERTEX_3D);
	//UINT offset = 0;
	//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	//// �v���~�e�B�u�g�|���W�ݒ�
	//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//// �}�e���A���ݒ�
	//MATERIAL material;
	//ZeroMemory(&material, sizeof(material));
	//material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//SetMaterial(material);

	//// �e�N�X�`���ݒ�
	//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);


	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Field.scl.x, g_Field.scl.y, g_Field.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Field.rot.x, g_Field.rot.y, g_Field.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Field.pos.x, g_Field.pos.y, g_Field.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Field.mtxWorld, mtxWorld);


	// �|���S���`��
	//GetDeviceContext()->Draw(4, 0);		// 4���_����0�Ԗڂ̒��_�ԍ�����`��
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
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4 * MAX_POLYGON;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// �O���[�o���̒��_�z�񂩂璸�_�o�b�t�@������
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = g_VertexArray;

	GetDevice()->CreateBuffer(&bd, &sd, &g_VertexBuffer);

	return S_OK;
}