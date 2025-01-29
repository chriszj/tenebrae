//=============================================================================
//
// ���C�g���� [light.cpp]
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
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

static BOOL		g_FogEnable = FALSE;


//=============================================================================
// ����������
//=============================================================================
void InitLight(void)
{

	//���C�g������
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// ��������
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ���C�g�̃^�C�v
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// ���s�����̐ݒ�i���E���Ƃ炷���j
	g_Light[0].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );		// ���̌���
	g_Light[0].Diffuse   = XMFLOAT4( 0.3f, 0.3f, 0.3f, 1.0f );	// ���̐F
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// ���s����
	g_Light[0].Enable = TRUE;									// ���̃��C�g��ON
	SetLight(0, &g_Light[0]);									// ����Őݒ肵�Ă���



	// �t�H�O�̏������i���̌��ʁj
	g_Fog.FogStart = 100.0f;									// ���_���炱�̋��������ƃt�H�O��������n�߂�
	g_Fog.FogEnd   = 250.0f;									// �����܂ŗ����ƃt�H�O�̐F�Ō����Ȃ��Ȃ�
	g_Fog.FogColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );		// �t�H�O�̐F
	SetFog(&g_Fog);
	SetFogEnable(g_FogEnable);				// ���̏ꏊ���`�F�b�N���� shadow

}


//=============================================================================
// �X�V����
//=============================================================================
void UpdateLight(void)
{



}

//=============================================================================
// ���C�g�̐ݒ�
// Type�ɂ���ăZ�b�g���郁���o�[�ϐ����ς���Ă���
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

	// �r���[�}�g���b�N�X�ݒ�
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&light->Position), XMLoadFloat3(&light->View.at), XMLoadFloat3(&light->View.up));
	SetLightViewMatrix(&mtxView);
	XMStoreFloat4x4(&light->View.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&light->View.mtxInvView, mtxInvView);

	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
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
// �t�H�O�̐ݒ�
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}


BOOL	GetFogEnable(void)
{
	return(g_FogEnable);
}


