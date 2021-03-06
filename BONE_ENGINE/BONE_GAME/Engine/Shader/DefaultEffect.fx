/*-----------------------------------------------------------------------------
	Name	: Soft Shadows.fx
	Desc	: Soft shadows effect file.
	Author	: Anirudh S Shastry. Copyright (c) 2004.
	Date	: 22nd June, 2004.
-----------------------------------------------------------------------------*/

//--------------------------------------------
// Global variables
//--------------------------------------------
#define MAX_POINT_LIGHTS 8

matrix	matWorldViewProj	: WorldViewProjection;
matrix	matLightViewProj	: LightViewProjection;
matrix	matWorld			: World;
matrix	matWorldIT			: WorldInverseTranspose;
float2  sampleOffsets[15];
float   sampleWeights[15];
bool	onLight[MAX_POINT_LIGHTS];

vector 	eyePos				: EyePosition;

float4 	globalAmbient;

struct PointLight
{
	float3 pos;
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float radius;
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 emissive;
	float4 specular;
	float shininess;
};

PointLight 	lights[MAX_POINT_LIGHTS];
Material 	material;

matrix	matTexture1		: Texture;
matrix  matTexture2		: Texture;
matrix  matTexture3		: Texture;
matrix  matTexture4		: Texture;
matrix  matTexture5		: Texture;
matrix  matTexture6		: Texture;
matrix  matTexture7		: Texture;
matrix  matTexture8		: Texture;

texture tShadowMap1;
texture tShadowMap2;
texture tShadowMap3;
texture tShadowMap4;
texture tShadowMap5;
texture tShadowMap6;
texture tShadowMap7;
texture tShadowMap8;
texture tScreenMap;
texture tBlurHMap;
texture tBlurVMap;
texture	tColorMap;
texture normalMapTexture;

sampler ShadowSampler1 = sampler_state
{
	texture = (tShadowMap1);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler2 = sampler_state
{
	texture = (tShadowMap2);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler3 = sampler_state
{
	texture = (tShadowMap3);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler4 = sampler_state
{
	texture = (tShadowMap4);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler5 = sampler_state
{
	texture = (tShadowMap5);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler6 = sampler_state
{
	texture = (tShadowMap6);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler7 = sampler_state
{
	texture = (tShadowMap7);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler8 = sampler_state
{
	texture = (tShadowMap8);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ScreenSampler = sampler_state
{
	texture = (tScreenMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler BlurHSampler = sampler_state
{
	texture = (tBlurHMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler BlurVSampler = sampler_state
{
	texture = (tBlurVMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ColorSampler = sampler_state
{
	texture = (tColorMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Wrap;
	AddressV = Wrap;
};

sampler2D normalMap = sampler_state
{
    Texture = <normalMapTexture>;
    MagFilter = Linear;
    MinFilter = Anisotropic;
    MipFilter = Linear;
    MaxAnisotropy = 16;
};

////////////////////////////////////////////////////////////////////////////////////
//// Shadow Tech
////////////////////////////////////////////////////////////////////////////////////

struct VS_OUTPUT_SHADOW
{
	float4 position	: POSITION;
	float  depth	: TEXCOORD0;
};

VS_OUTPUT_SHADOW VS_Shadow( float4 inPosition : POSITION )
{
	// Output struct
	VS_OUTPUT_SHADOW OUT = (VS_OUTPUT_SHADOW)0;

	// Output the transformed position
	inPosition = mul (inPosition, matWorld);
	OUT.position = mul(inPosition, matLightViewProj);

	// Output the scene depth
	OUT.depth = OUT.position.z;

	return OUT;
}

float4  PS_Shadow( VS_OUTPUT_SHADOW IN ) : COLOR0
{
	return float4( IN.depth, IN.depth, IN.depth, 1.0f );
}
	

////////////////////////////////////////////////////////////////////////////////////
//// Unlit Tech
////////////////////////////////////////////////////////////////////////////////////

struct VS_OUTPUT_UNLIT
{
	float4 position		: POSITION0;
	float4 worldPos		: POSITION1;
	float4 texCoord1	: TEXCOORD0;
	float4 texCoord2	: TEXCOORD1;
	float4 texCoord3	: TEXCOORD2;
	float4 texCoord4	: TEXCOORD3;
	float4 texCoord5	: TEXCOORD4;
	float4 texCoord6	: TEXCOORD5;
	float4 texCoord7	: TEXCOORD6;
    float4 texCoord8	: TEXCOORD7;
};

VS_OUTPUT_UNLIT VS_Unlit(float4 inPosition : POSITION)
{
	// Output struct
	VS_OUTPUT_UNLIT OUT = (VS_OUTPUT_UNLIT)0;

	// Output the transformed position
    inPosition = mul(inPosition, matWorld);
	OUT.worldPos = inPosition;
	OUT.position = mul(inPosition, matWorldViewProj);
		
	// Output the projective texture coordinates
	OUT.texCoord1 = mul(inPosition, matTexture1);
	OUT.texCoord2 = mul(inPosition, matTexture2);
	OUT.texCoord3 = mul(inPosition, matTexture3);
	OUT.texCoord4 = mul(inPosition, matTexture4);
	OUT.texCoord5 = mul(inPosition, matTexture5);
	OUT.texCoord6 = mul(inPosition, matTexture6);
	OUT.texCoord7 = mul(inPosition, matTexture7);
	return OUT;
}

// Shadow mapping pixel shader
float4  PS_Unlit( VS_OUTPUT_UNLIT IN ) : COLOR0
{
	// Generate the 9 texture co-ordinates for a 3x3 PCF kernel
	float4 TexCoords[9];

	// Texel size
	float TexelSize = 1.0f / 1024.0f;

	// Generate the tecture co-ordinates for the specified depth-map size
	// 4 3 5
	// 1 0 2
	// 7 6 8

	TexCoords[0] = 0;

	TexCoords[1] = float4( -TexelSize, 0.0f, 0.0f, 0.0f );
	TexCoords[2] = float4(  TexelSize, 0.0f, 0.0f, 0.0f );
	TexCoords[3] = float4( 0.0f, -TexelSize, 0.0f, 0.0f );
	TexCoords[6] = float4( 0.0f,  TexelSize, 0.0f, 0.0f );

	TexCoords[4] = float4( -TexelSize, -TexelSize, 0.0f, 0.0f );
	TexCoords[5] = float4(  TexelSize, -TexelSize, 0.0f, 0.0f );
	TexCoords[7] = float4( -TexelSize,  TexelSize, 0.0f, 0.0f );
	TexCoords[8] = float4(  TexelSize,  TexelSize, 0.0f, 0.0f );
	

	// Sample each of them checking whether the pixel under test is shadowed or not
	float ShadowTerm = 0.0f;
    float4 temp;
	
    for( int i = 0; i < 9; i++ )
	{
		float A1 = 1, A2 = 0;
		float B1 = 1, B2= 0;
		float C1 = 1, C2= 0;
		float D1 = 1, D2= 0;
		float E1 = 1, E2= 0;
		float F1 = 1, F2= 0;
		float G1 = 1, G2= 0;
		float H1 = 1, H2= 0;

		if (onLight[0])
		{
            temp = IN.texCoord1 + TexCoords[i];

			A1 = tex2Dproj( ShadowSampler1, temp ).r;
			A2 = (IN.texCoord1.z - 0.001f);
		}

		if (onLight[1])
		{
            temp = IN.texCoord2 + TexCoords[i];
	
			B1 = tex2Dproj( ShadowSampler2, temp ).r;
			B2 = (IN.texCoord2.z - 0.001f);
		}

		if (onLight[2])
		{
            temp = IN.texCoord3 + TexCoords[i];
	
			C1 = tex2Dproj( ShadowSampler3, temp ).r;
			C2 = (IN.texCoord3.z - 0.001f);
		}

		if (onLight[3])
		{
            temp = IN.texCoord4 + TexCoords[i];
	

			D1 = tex2Dproj( ShadowSampler4, temp ).r;
			D2 = (IN.texCoord4.z - 0.001f);
		}

		if (onLight[4])
		{
            temp = IN.texCoord5 + TexCoords[i];
	
			E1 = tex2Dproj( ShadowSampler5, temp ).r;
			E2 = (IN.texCoord5.z - 0.001f);
		}

		if (onLight[5])
		{
            temp = IN.texCoord6 + TexCoords[i];
	
			F1 = tex2Dproj( ShadowSampler6, temp ).r;
			F2 = (IN.texCoord6.z - 0.001f);
		}

		if (onLight[6])
		{
			temp = IN.texCoord7 + TexCoords[i];
	
			G1 = tex2Dproj( ShadowSampler7, temp ).r;
			G2 = (IN.texCoord7.z - 0.001f);
		}

		if (onLight[7])
		{
			temp = IN.texCoord8 + TexCoords[i];
	
			H1 = tex2Dproj( ShadowSampler8, temp ).r;
			H2 = (IN.texCoord8.z - 0.001f);
		}

		bool shaded = false;

		// Texel is shadowed
		if ( A1 < A2 && onLight[0] == true)
		{
			float dist = distance(lights[0].pos, IN.worldPos);

			if (dist < lights[0].radius)
			{
				ShadowTerm += 0.1f;
				shaded = true;
			}
		}

		if ( B1 < B2 && onLight[1] == true)
		{
			float dist = distance(lights[1].pos, IN.worldPos);

			if (dist < lights[1].radius && !shaded)
			{
				ShadowTerm += 0.1f;
				shaded = true;
			}
		}

		if ( C1 < C2 && onLight[2] == true)
		{
			float dist = distance(lights[2].pos, IN.worldPos);

			if (dist < lights[2].radius && !shaded)
			{
				ShadowTerm += 0.1f;
				shaded = true;
			}
		}

		if ( D1 < D2 && onLight[3] == true)
		{
			float dist = distance(lights[3].pos, IN.worldPos);

			if (dist < lights[3].radius && !shaded)
			{
				ShadowTerm += 0.1f;
				shaded = true;
			}
		}

		if ( E1 < E2 && onLight[4] == true)
		{
			float dist = distance(lights[4].pos, IN.worldPos);

			if (dist < lights[4].radius && !shaded)
			{
				ShadowTerm += 0.1f;
				shaded = true;
			}
		}

		if ( F1 < F2 && onLight[5] == true)
		{
			float dist = distance(lights[5].pos, IN.worldPos);

			if (dist < lights[5].radius && !shaded)
			{
				ShadowTerm += 0.1f;
				shaded = true;
			}
		}

		if ( G1 < G2 && onLight[6] == true)
		{
			float dist = distance(lights[6].pos, IN.worldPos);

			if (dist < lights[6].radius && !shaded)
			{
				ShadowTerm += 0.1f;
				shaded = true;
			}
		}
		
		if ( H1 < H2 && onLight[7] == true)
		{
			float dist = distance(lights[7].pos, IN.worldPos);

			if (dist < lights[7].radius && !shaded)
			{
				ShadowTerm += 0.1f;
				shaded = true;
			}
		}
		
		if (shaded == false)
			ShadowTerm += 1.0f;
	}
	
	// Get the average
	ShadowTerm /= 9.0f;

	int lights = 0;

	for (int i = 0; i < 8; i++)
		if (onLight[i])
			lights++;

	ShadowTerm /= lights;

	return ShadowTerm;
}



////////////////////////////////////////////////////////////////////////////////////
//// BLUR Tech
////////////////////////////////////////////////////////////////////////////////////

struct VS_OUTPUT_BLUR
{
	float4 position	: POSITION;
	float2 texCoord	: TEXCOORD0;
};

// Gaussian filter vertex shader
VS_OUTPUT_BLUR VS_Blur( float4 inPosition : POSITION, float2 inTexCoord : TEXCOORD0 )
{
	// Output struct
	VS_OUTPUT_BLUR OUT = (VS_OUTPUT_BLUR)0;

	// Output the position
	OUT.position = inPosition;

	// Output the texture coordinates
	OUT.texCoord = inTexCoord;

	return OUT;
}

// Horizontal blur pixel shader
float4 PS_BlurH( VS_OUTPUT_BLUR IN ) : COLOR0
{
	// Accumulated color
	float4 Accum = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Sample the taps (SampleOffsets holds the texel offsets and SampleWeights holds the texel weights)
	for( int i = 0; i < 15; i++ )
	{
		Accum += tex2D( ScreenSampler, IN.texCoord + sampleOffsets[i] ) * sampleWeights[i];
	}

	return Accum;
}

// Vertical blur pixel shader
float4 PS_BlurV( VS_OUTPUT_BLUR IN ) : COLOR0
{
	// Accumulated color
	float4 Accum = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Sample the taps (SampleOffsets holds the texel offsets and SampleWeights holds the texel weights)
	for( int i = 0; i < 15; i++ )
	{
		Accum += tex2D( BlurHSampler, IN.texCoord + sampleOffsets[i] ) * sampleWeights[i];
	}

	return Accum;
}
		
struct VS_INPUT_SCENE
{
	float4 position 	: POSITION;
	float3 normal 		: NORMAL;
	float2 texCoord 	: TEXCOORD0;
	float4 tangent 		: TANGENT;
};

struct VS_OUTPUT_SCENE
{
	float4 position		: POSITION;
	float2 texCoord		: TEXCOORD0;
	float4 projCoord	: TEXCOORD1;
	float4 screenCoord	: TEXCOORD2;
	float3 normal		: TEXCOORD3;
	float4 worldPos		: TEXCOORD4;
	float3 viewDir		: TEXCOORD5;
	float3 tangent 		: TEXCOORD6;
	float3 bitangent 	: TEXCOORD7;
};

// Scene pixel shader
VS_OUTPUT_SCENE VS_Scene(VS_INPUT_SCENE IN)
{
	VS_OUTPUT_SCENE OUT = (VS_OUTPUT_SCENE)0;

	IN.position.w = 1.0f;

	OUT.position = mul(IN.position, matWorld);
	OUT.position = mul(OUT.position, matWorldViewProj);

	OUT.texCoord = IN.texCoord;

	OUT.projCoord = mul(IN.position, matTexture1);

	OUT.screenCoord.x = ( OUT.position.x * 0.5 + OUT.position.w * 0.5 );
	OUT.screenCoord.y = ( OUT.position.w * 0.5 - OUT.position.y * 0.5 );
	OUT.screenCoord.z = OUT.position.w;
	OUT.screenCoord.w = OUT.position.w;

	OUT.worldPos = mul(IN.position, matWorld);
	OUT.normal = mul(IN.normal, matWorldIT);
	OUT.viewDir = eyePos - OUT.worldPos;

	OUT.tangent = mul(IN.tangent.xyz, (float3x3)matWorldIT);
	OUT.bitangent = cross(OUT.normal, OUT.tangent) * IN.tangent.w;
	
	return OUT;
}

float4 PS_Scene( VS_OUTPUT_SCENE IN ) : COLOR
{
	float3 t = normalize(IN.tangent);
    float3 b = normalize(IN.bitangent);
    float3 n = normalize(tex2D(normalMap, IN.texCoord).rgb * 2.0f - 1.0f);//normalize(IN.normal);

    float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
	                              t.y, b.y, n.y,
	                              t.z, b.z, n.z);
	                                 
    float3 v = normalize(mul(eyePos - IN.worldPos, tbnMatrix)) * 2.0f;
    float3 l = float3(0.0f, 0.0f, 0.0f);
    float3 h = float3(0.0f, 0.0f, 0.0f);
    
    float atten = 0.0f;
    float nDotL = 0.0f;
    float nDotH = 0.0f;
    float power = 0.0f;
    
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
    for (int i = 0; i < 8; i++)
    {
		if (onLight[i] == false)
			continue;

        l = (lights[i].pos - IN.worldPos) / lights[i].radius;
        atten = saturate(1.0f - dot(l, l));
        
        l = normalize(l);
        h = normalize(l + v);
        
        nDotL = saturate(dot(n, l));
        nDotH = saturate(dot(n, h));
        power = (nDotL == 0.0f) ? 0.0f : pow(nDotH, material.shininess);
        
        color += (material.ambient * (globalAmbient + (atten * lights[i].ambient))) +
                 (material.diffuse * lights[i].diffuse * nDotL * atten) +
                 (material.specular * lights[i].specular * power * atten);
    }

	float ShadowTerm = tex2Dproj( BlurVSampler, IN.screenCoord );
	float4 TextureColor = tex2D( ColorSampler, IN.texCoord );
	
	color *= TextureColor * ShadowTerm;
	
	return color;
}

//--------------------------------------------
// Techniques
//--------------------------------------------
technique techShadow
{
	pass p0
	{
		Lighting	= False;
		CullMode	= CCW;
		
		VertexShader = compile vs_3_0 VS_Shadow();
		PixelShader  = compile ps_3_0 PS_Shadow();
	}
}

technique techUnlit
{
	pass p0
	{
		Lighting	= False;
		CullMode	= CCW;
		
		VertexShader = compile vs_3_0 VS_Unlit();
		PixelShader  = compile ps_3_0 PS_Unlit();
	}
}

technique techBlurH
{
	pass p0
	{
		Lighting	= False;
		CullMode	= None;
		
		VertexShader = compile vs_3_0 VS_Blur();
		PixelShader  = compile ps_3_0 PS_BlurH();
	}
}

technique techBlurV
{
	pass p0
	{
		Lighting	= False;
		CullMode	= None;
		
		VertexShader = compile vs_3_0 VS_Blur();
		PixelShader  = compile ps_3_0 PS_BlurV();
	}
}

technique techScene
{
	pass p0
	{
		Lighting	= False;
		CullMode	= CCW;
		
		VertexShader = compile vs_3_0 VS_Scene();
		PixelShader  = compile ps_3_0 PS_Scene();
	}
}