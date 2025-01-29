

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マトリクスバッファ
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

// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte境界用
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ライト用バッファ
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte境界用
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
	float		Dummy[3];//16byte境界用
};

// フォグ用バッファ
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// 縁取り用バッファ
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
// 頂点シェーダ
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

	
	//matrix wvp;
	//wvp = mul(World, View);
	//wvp = mul(wvp, Projection);
	//outPosition = mul(inPosition, wvp);
    
    // Calculate the position of the vertex against the world, view, and projection matrices.
    outPosition = mul(inPosition, World);
    outPosition = mul(outPosition, View);
    outPosition = mul(outPosition, Projection);
	
	// Calculate the position of the vertice as viewed by the light source.
    outLightViewPos = mul(inPosition, World);
    outLightViewPos = mul(outLightViewPos, LightViewMatrix);
    outLightViewPos = mul(outLightViewPos, LightProjectionMatrix);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

	outDiffuse = inDiffuse;
	
    //outLightViewPos.rb = 0.5f;
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
    /*matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
	
    outPosition = mul(inPosition, wvp);

    outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

    outTexCoord = inTexCoord;

    outWorldPos = mul(inPosition, World);

    outDiffuse = inDiffuse;*/
	
    //PixelInputType output;
    
    
	// Change the position vector to be 4 units for proper matrix calculations.
    inPosition.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    outPosition = mul(inPosition, World);
    outPosition = mul(outPosition, LightViewMatrix);
    outPosition = mul(outPosition, LightProjectionMatrix);
	
	// testing
    //outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

    //outTexCoord = inTexCoord;

    //outWorldPos = mul(inPosition, World);

    //outDiffuse = inDiffuse;

	// Store the position value in a second input value for depth value calculations.
    outDepth = outPosition;
	
    
}

//-----------------------------------------------------------------------------
// Vertex Shader: VertShadow
// Desc: Process vertex for the shadow map
//-----------------------------------------------------------------------------
void VertexShadow(	in float4 inPosition : POSITION0,
					in float4 inNormal : NORMAL0,
					in float4 inDiffuse : COLOR0,
					in float2 inTexCoord : TEXCOORD0,

					out float4 outPosition : SV_POSITION,
					out float4 outNormal : NORMAL0,
					out float2 outTexCoord : TEXCOORD0,
					out float4 outDiffuse : COLOR0,
					out float4 outWorldPos : POSITION0)
{
    //
    // Compute the projected coordinates
    //
    //oPos = mul(Pos, g_mWorldView);
    //oPos = mul(oPos, g_mProj);
	
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    outPosition = mul(inPosition, wvp);

    //
    // Store z and w in our spare texcoord
    //
    //Depth.xy = oPos.zw;
    outTexCoord.xy = outPosition.zw;
}


//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
Texture2D		g_DepthTexture : register(t1);
SamplerState	g_SamplerState : register( s0 );
SamplerState	g_SamplerStateClamp : register(s1);


//=============================================================================
// ピクセルシェーダ
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
						
						// Calculate the projected texture coordinates.
                        projectTexCoord.x = inLightViewPos.x / inLightViewPos.w / 2.0f + 0.5f;
                        projectTexCoord.y = -inLightViewPos.y / inLightViewPos.w / 2.0f + 0.5f;
						
                        if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
                        {
							// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
                            depthValue = g_DepthTexture.Sample(g_SamplerStateClamp, projectTexCoord).r;

							// Calculate the depth of the light.
                            lightDepthValue = inLightViewPos.z / inLightViewPos.w;

							// Subtract the bias from the lightDepthValue.
                            lightDepthValue = lightDepthValue - 0.0022f;

							// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
							// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
                            if (lightDepthValue < depthValue)
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

	//フォグ
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

	//縁取り
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
 //   float4 color;

 //   if (Material.noTexSampling == 0)
 //   {
 //       color = g_Texture.Sample(g_SamplerState, inTexCoord);

 //       color *= inDiffuse;
 //   }
 //   else
 //   {
 //       color = inDiffuse;
 //   }

 //   if (Light.Enable == 0)
 //   {
 //       color = color * Material.Diffuse;
 //   }
 //   else
 //   {
 //       float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
 //       float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

 //       for (int i = 0; i < 5; i++)
 //       {
 //           float3 lightDir;
 //           float light;

 //           if (Light.Flags[i].y == 1)
 //           {
 //               if (Light.Flags[i].x == 1)
 //               {
 //                   lightDir = normalize(Light.Direction[i].xyz);
 //                   light = dot(lightDir, inNormal.xyz);

 //                   light = 0.5 - 0.5 * light;
 //                   tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
 //               }
 //               else if (Light.Flags[i].x == 2)
 //               {
 //                   lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
 //                   light = dot(lightDir, inNormal.xyz);

 //                   tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

 //                   float distance = length(inWorldPos - Light.Position[i]);

 //                   float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
 //                   tempColor *= att;
 //               }
 //               else
 //               {
 //                   tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
 //               }

 //               outColor += tempColor;
 //           }
 //       }

 //       color = outColor;
 //       color.a = inDiffuse.a * Material.Diffuse.a;
 //   }

	////フォグ
 //   if (Fog.Enable == 1)
 //   {
 //       float z = inPosition.z * inPosition.w;
 //       float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
 //       f = saturate(f);
 //       outDiffuse = f * color + (1 - f) * Fog.FogColor;
 //       outDiffuse.a = color.a;
 //   }
 //   else
 //   {
 //       outDiffuse = color;
 //   }

	////縁取り
 //   if (fuchi == 1)
 //   {
 //       float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	//	//if ((angle < 0.5f)&&(angle > -0.5f))
 //       if (angle > -0.3f)
 //       {
 //           outDiffuse.rb = 1.0f;
 //           outDiffuse.g = 0.0f;
 //       }
 //   }
	
    /*float depthValue;
    float4 color;
	
	
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    depthValue = input.depthPosition.z / input.depthPosition.w;

    color = float4(depthValue, depthValue, depthValue, 1.0f);

    return color;*/
	
    float depthValue;
    float4 color;
	
	
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    depthValue = inDepth.z / inDepth.w;

    color = float4(depthValue, depthValue, depthValue, 1.0f);

    outDiffuse = color;
	
    
}

//-----------------------------------------------------------------------------
// Pixel Shader: PixShadow
// Desc: Process pixel for the shadow map
//-----------------------------------------------------------------------------
void PixelShadow(float2 Depth : TEXCOORD0,
                out float4 Color : COLOR)
{
    //
    // Depth is z / w
    //
    Color = Depth.x / Depth.y;
}

void ShadowPixelShader2(in float4 inPosition : SV_POSITION,
						 in float4 inNormal : NORMAL0,
						 in float2 inTexCoord : TEXCOORD0,
						 in float4 inDiffuse : COLOR0,
						 in float4 inWorldPos : POSITION0,
						 in float4 inLightViewPos : POSITION1,

						 out float4 outDiffuse : SV_Target)
{
    float4 color;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
	

    // Set the default output color to the ambient light value for all pixels.
    color = float4(0.3f, 0.3f, 0.3f, 1.0f);

    // Calculate the projected texture coordinates.
    projectTexCoord.x = inLightViewPos.x / inLightViewPos.w / 2.0f + 0.5f;
    projectTexCoord.y = -inLightViewPos.y / inLightViewPos.w / 2.0f + 0.5f;

    // Determine if the projected coordinates are in the 0 to 1 range.  If it is then this pixel is inside the projected view port.
    //if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        // Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
        depthValue = g_DepthTexture.Sample(g_SamplerStateClamp, projectTexCoord).r;

        // Calculate the depth of the light.
        lightDepthValue = inLightViewPos.z / inLightViewPos.w;

        // Subtract the bias from the lightDepthValue.
        lightDepthValue = lightDepthValue - 0.0022f;

        // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
        // If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
        if (lightDepthValue > depthValue)
        {
            // Calculate the amount of light on this pixel.
            lightIntensity = saturate(dot(inNormal, Light.Position[1]));

            if (lightIntensity > 0.0f)
            {
                // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
                color += (inDiffuse * lightIntensity);
                //color.rgb = 1.0f;
            }
        }
    }

    

    // Saturate the final light color.
    color = saturate(color);

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = g_Texture.Sample(g_SamplerState, inTexCoord);

    // Combine the light and texture color.
    color = color * textureColor;

    outDiffuse = color;
}
