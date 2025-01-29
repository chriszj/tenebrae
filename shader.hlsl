

//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************

// �}�g���N�X�o�b�t�@
cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}

// �}�e���A���o�b�t�@
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte���E�p
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ���C�g�p�o�b�t�@
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte���E�p
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte���E�p
};

// �t�H�O�p�o�b�t�@
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// �����p�o�b�t�@
cbuffer Fuchi : register(b6)
{
	int			fuchi;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

cbuffer LightViewBuffer : register(b8)
{
    matrix LightViewMatrix;
}

cbuffer LighProjectionBuffer : register(b9)
{
    matrix LightProjectionMatrix;
}


//=============================================================================
// ���_�V�F�[�_
//=============================================================================
void VertexShaderPolygon( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0,
						  out float2 outTexCoord	: TEXCOORD0,
						  out float4 outDiffuse		: COLOR0,
						  out float4 outWorldPos    : POSITION0,
						  out float4 outLightViewPos:POSITION1)
{
    inPosition.w = 1;
    
    // �J�����̎��_���璸�_�̈ʒu���v�Z���܂�
    outPosition = mul(inPosition, World);
    outPosition = mul(outPosition, View);
    outPosition = mul(outPosition, Projection);
	
	// ���C�g�̎��_���璸�_�̈ʒu���v�Z���܂�
    outLightViewPos = mul(inPosition, World);
    outLightViewPos = mul(outLightViewPos, LightViewMatrix);
    outLightViewPos = mul(outLightViewPos, LightProjectionMatrix);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

	outDiffuse = inDiffuse;
	
}

void VertexShaderForSM(in float4 inPosition : POSITION0,
						  in float4 inNormal : NORMAL0,
						  in float4 inDiffuse : COLOR0,
						  in float2 inTexCoord : TEXCOORD0,
						  in float4 inDepth : TEXTURE0,

						  out float4 outPosition : SV_POSITION,
						  out float4 outNormal : NORMAL0,
						  out float2 outTexCoord : TEXCOORD0,
						  out float4 outDiffuse : COLOR0,
						  out float4 outWorldPos : POSITION0,
						  out float4 outDepth : TEXTURE0)
{
    
	// �K�؂ȍs��v�Z���s���ɂ́A�ʒu�x�N�g���� 4 �P�ʂɕύX���܂��B
    inPosition.w = 1.0f;

	// ���C�g�̎��_���璸�_�̈ʒu���v�Z���܂��B
    outPosition = mul(inPosition, World);
    outPosition = mul(outPosition, LightViewMatrix);
    outPosition = mul(outPosition, LightProjectionMatrix);

	// �[���̒l���v�Z���邽�߂ɁA�ʒu�̒l�� 2 �Ԗڂ̓��͒l�ɕۑ����܂��B
    outDepth = outPosition;
	
}

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
Texture2D		g_DepthTexture : register(t1);
SamplerState	g_SamplerState : register( s0 );
SamplerState	g_SamplerStateClamp : register(s1);


//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
void PixelShaderPolygon( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,
						 in  float4 inWorldPos      : POSITION0,
						 in  float4 inLightViewPos  : POSITION1,

						 out float4 outDiffuse		: SV_Target )
{
	
 
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
	
    int renderPixel = 1;
	
    float4 color;

    if (Material.noTexSampling == 0)
    {
        color = g_Texture.Sample(g_SamplerState, inTexCoord);

        color *= inDiffuse;
    }
    else
    {
        color = inDiffuse;
    }

    if (Light.Enable == 0)
    {
        color = color * Material.Diffuse;
    }
    else
    {
        float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

        for (int i = 0; i < 5; i++)
        {
            float3 lightDir;
            float light;

            if (Light.Flags[i].y == 1)
            {
                if (Light.Flags[i].x == 1)
                {
                    lightDir = normalize(Light.Direction[i].xyz);
                    light = dot(lightDir, inNormal.xyz);

                    light = 0.5 - 0.5 * light;
                    tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
                }
                else if (Light.Flags[i].x == 2)
                {
                    lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
                    light = dot(lightDir, inNormal.xyz);

                    tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

                    float distance = length(inWorldPos - Light.Position[i]);

                    float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
                    tempColor *= att;
					
                    if (Light.Flags[i].z == 1)
                    {
						
                        renderPixel = 0;
						
						// ���e���ꂽ�e�N�X�`�����W���v�Z���܂��B
                        projectTexCoord.x = inLightViewPos.x / inLightViewPos.w / 2.0f + 0.5f;
                        projectTexCoord.y = -inLightViewPos.y / inLightViewPos.w / 2.0f + 0.5f;
						
                        if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
                        {
							// ���e���ꂽ�e�N�X�`�����W�ʒu�ŃT���v���[���g�p���āA�[�x�e�N�X�`������V���h�E �}�b�v�̐[�x�l���T���v�����O���܂��B
                            depthValue = g_DepthTexture.Sample(g_SamplerStateClamp, projectTexCoord).r;

							// ���C�g�̐[�����擾���܂��B
                            lightDepthValue = inLightViewPos.z / inLightViewPos.w;

							// ���C�g�̐[�x�l����o�C�A�X�����Z���܂��B
                            lightDepthValue = lightDepthValue - 0.0033f;

							// �V���h�E �}�b�v�l�̐[���ƃ��C�g�̐[�����r���āA���̃s�N�Z�����V���h�E���邩���C�g���邩�����肵�܂��B
                            if (lightDepthValue > depthValue)
                            {
                                tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
                            }
                        }
						
                    }
					
                   

                }
                else
                {
                    tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
                }

                outColor += tempColor;
            }
        }

        color = outColor;
        color.a = inDiffuse.a * Material.Diffuse.a;
    }

	//�t�H�O
    if (Fog.Enable == 1)
    {
        float z = inPosition.z * inPosition.w;
        float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
        f = saturate(f);
        outDiffuse = f * color + (1 - f) * Fog.FogColor;
        outDiffuse.a = color.a;
    }
    else
    {
        outDiffuse = color;
    }

	//�����
    if (fuchi == 1)
    {
        float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
		//if ((angle < 0.5f)&&(angle > -0.5f))
        if (angle > -0.3f)
        {
            outDiffuse.rb = 1.0f;
            outDiffuse.g = 0.0f;
        }
    }
    
}

void PixelShaderForSM(in float4 inPosition : SV_POSITION,
						 in float4 inNormal : NORMAL0,
						 in float2 inTexCoord : TEXCOORD0,
						 in float4 inDiffuse : COLOR0,
						 in float4 inWorldPos : POSITION0,
						 in float4 inDepth : TEXTURE0,

						 out float4 outDiffuse : SV_Target)
{
    
    float depthValue;
    float4 color;
	
	
	// Z �s�N�Z���[�x�𓯎� W ���W�ŏ��Z���āA�s�N�Z���̐[�x�l���擾���܂��B
    depthValue = inDepth.z / inDepth.w;

    color = float4(depthValue, depthValue, depthValue, 1.0f);

    outDiffuse = color;
	
    
}
