//=============================================================================
//
// �O���t�B�b�N���[�U�[�C���^�[�t�F�[�X���� [gui.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"
#include "gui.h"
#include "pugixml.hpp"
//#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(16)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(32)	// 
#define TEXTURE_MAX                 (3)     

#define TEXTURE_HUD_DIVIDEX         (1)
#define TEXTURE_HUD_DIVIDEY         (1)
#define TEXTURE_HUD_WIDTH           (128)
#define TEXTURE_HUD_HEIGHT          (32)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void ParseFont(BMPFONT* font, char* file);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[FONTTEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static ID3D11ShaderResourceView* g_GUITexture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char g_FontFolder[] = "data/TEXTURE/FONT/";

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/common/icon-hud.png",
	"data/TEXTURE/common/time-effect-bg.png",
	"data/TEXTURE/common/time-effect.png",
};

static BMPFONT g_Font;
static BMPTEXT g_Texts[TEXTOBJECTS_MAX];

static BOOL						g_Use;						// true:�g���Ă���  false:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�



static int						g_Score;					// �X�R�A

static BOOL                     g_TimeMachineGUIActive;
static BOOL						g_PlayerGUIActive;

static GUIIMAGE	                g_GUIImages[GUI_ICON_MAX];

//static PLAYER*                  g_registeredPlayer;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGUI(void)
{
	ID3D11Device *pDevice = GetDevice();

	ParseFont(&g_Font, "data/TEXTURE/FONT/cjk-jp.fnt");

	//�e�N�X�`������
	for (int i = 0; i < FONTTEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;

		if (g_Font.pages[i].id < 0)
			continue;

		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_Font.pages[i].file,
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_GUITexture[i] = NULL;


		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_GUITexture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	//g_registeredPlayer = NULL;

	// �v���C���[�̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 500.0f, 20.0f, 0.0f };
	g_TexNo = 0;


	for (int i = 0; i < GUI_ICON_MAX; i++)
	{
		g_GUIImages[i].use = FALSE;

		g_GUIImages[i].iconID = i;

		switch (g_GUIImages[i].iconID) {

		case GUI_ICON_HP:
			g_GUIImages[i].texNo = 0;
			g_GUIImages[i].patternNo = 0;
			g_GUIImages[i].pos = { 25, 30 };
			g_GUIImages[i].width = 32;
			g_GUIImages[i].height = 32;
			g_GUIImages[i].texWidth = 160;
			g_GUIImages[i].texHeight = 32;
			g_GUIImages[i].divideX = 5;
			g_GUIImages[i].divideY = 1;
			break;
		case GUI_ICON_TIME:
			g_GUIImages[i].texNo = 0;
			g_GUIImages[i].patternNo = 1;
			g_GUIImages[i].pos = { SCREEN_WIDTH - 85, 30 };
			g_GUIImages[i].width = 32;
			g_GUIImages[i].height = 32;
			g_GUIImages[i].texWidth = 160;
			g_GUIImages[i].texHeight = 32;
			g_GUIImages[i].divideX = 5;
			g_GUIImages[i].divideY = 1;
			break;
		case GUI_ICON_KEY:
			g_GUIImages[i].texNo = 0;
			g_GUIImages[i].patternNo = 2;
			g_GUIImages[i].pos = { SCREEN_WIDTH - 75, SCREEN_HEIGHT - 90 };
			g_GUIImages[i].width = 32;
			g_GUIImages[i].height = 32;
			g_GUIImages[i].texWidth = 160;
			g_GUIImages[i].texHeight = 32;
			g_GUIImages[i].divideX = 5;
			g_GUIImages[i].divideY = 1;
			break;
		case GUI_ICON_MKEY:
			g_GUIImages[i].texNo = 0;
			g_GUIImages[i].patternNo = 3;
			g_GUIImages[i].pos = { SCREEN_WIDTH - 75, SCREEN_HEIGHT - 40 };
			g_GUIImages[i].width = 32;
			g_GUIImages[i].height = 32;
			g_GUIImages[i].texWidth = 160;
			g_GUIImages[i].texHeight = 32;
			g_GUIImages[i].divideX = 5;
			g_GUIImages[i].divideY = 1;
			break;
		case GUI_ICON_MONEY:
			g_GUIImages[i].texNo = 0;
			g_GUIImages[i].patternNo = 4;
			g_GUIImages[i].pos = { 25, SCREEN_HEIGHT - 40 };
			g_GUIImages[i].width = 32;
			g_GUIImages[i].height = 32;
			g_GUIImages[i].texWidth = 160;
			g_GUIImages[i].texHeight = 32;
			g_GUIImages[i].divideX = 5;
			g_GUIImages[i].divideY = 1;
			break;
		case GUI_ICON_TM_BG:
			g_GUIImages[i].texNo = 1;
			g_GUIImages[i].patternNo = 0;
			g_GUIImages[i].pos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
			g_GUIImages[i].width = SCREEN_WIDTH;
			g_GUIImages[i].height = SCREEN_HEIGHT;
			g_GUIImages[i].texWidth = 1920;
			g_GUIImages[i].texHeight = 1200;
			g_GUIImages[i].divideX = 1;
			g_GUIImages[i].divideY = 1;
			break;
		case GUI_ICON_TM_SIMBOL:
			g_GUIImages[i].texNo = 2;
			g_GUIImages[i].patternNo = 0;
			g_GUIImages[i].pos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
			g_GUIImages[i].width = 1920;
			g_GUIImages[i].height = 1200;
			g_GUIImages[i].texWidth = 1920;
			g_GUIImages[i].texHeight = 1200;
			g_GUIImages[i].divideX = 1;
			g_GUIImages[i].divideY = 1;
			break;

		}

	}

	g_Score = 0;	// �X�R�A�̏�����

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGUI(void)
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < FONTTEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_GUITexture[i])
		{
			g_GUITexture[i]->Release();
			g_GUITexture[i] = NULL;
		}
	}

	g_Font.Reset();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGUI(void)
{

	/*for (int t = 0; t < TEXTOBJECTS_MAX; t++)
	{

		if (!g_Texts[t].use)
			continue;

		for (int c = 0; c < TEXTCHAR_MAX; c++) {

			BMPCHAR* fontChar = g_Texts[t].textChars[c];

			if (fontChar->id < 0)
				continue;



			fontChar =   
		}
	}*/

#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGUI(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	

	for (int t = 0; t < TEXTOBJECTS_MAX; t++) 
	{
		
		if (!g_Texts[t].use)
			continue;

		int textLineIndex = 0;

		float textCharIndex = 0;
	
		for (int c = 0; c < TEXTCHAR_MAX; c++) {

			BMPCHAR* fontChar = g_Texts[t].textChars[c];

			if (fontChar == NULL)
				continue;

			if (fontChar->id == 10)
			{
				textLineIndex++;
				textCharIndex = 0;
				continue;
			}

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[fontChar->page]);

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Texts[t].x + textCharIndex;	// �X�R�A�̕\���ʒuX
			float py = g_Texts[t].y + (textLineIndex * g_Font.lineHeight * g_Texts[t].scale);			// �X�R�A�̕\���ʒuY

			px -= fontChar->xoffset/2 * g_Texts[t].scale;
			py += fontChar->yoffset/2 * g_Texts[t].scale;

			float pw = fontChar->width * g_Texts[t].scale;				// �X�R�A�̕\����
			float ph = fontChar->height * g_Texts[t].scale;				// �X�R�A�̕\������

			float tx = fontChar->textureU;		// �e�N�X�`���̕�
			float ty = fontChar->textureV;		// �e�N�X�`���̍���
			float tw = fontChar->textureWidth;			// �e�N�X�`���̍���X���W
			float th = fontChar->textureHeigt;			// �e�N�X�`���̍���Y���W

			textCharIndex += pw + (fontChar->xoffset * g_Texts[t].scale);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}
		
	}

	for (int gi = 0; gi < GUI_ICON_MAX; gi ++) {
	
		if (!g_GUIImages[gi].use)
			continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_GUITexture[g_GUIImages[gi].texNo]);

		//SetBlendState(BLEND_MODE_ALPHABLEND);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_GUIImages[gi].pos.x;
		float py = g_GUIImages[gi].pos.y;		// �X�R�A�̕\���ʒuY


		float pw = g_GUIImages[gi].width;				// �X�R�A�̕\����
		float ph = g_GUIImages[gi].height;				// �X�R�A�̕\������

		float tw = 1.0f / g_GUIImages[gi].divideX;// �e�N�X�`���̕�
		float th = 1.0f / g_GUIImages[gi].divideY;	// �e�N�X�`���̍���
		float tx = (float)(g_GUIImages[gi].patternNo % g_GUIImages[gi].divideX) * tw;	// �e�N�X�`���̍���X���W
		float ty = (float)(g_GUIImages[gi].patternNo / g_GUIImages[gi].divideX) * th;	// �e�N�X�`���̍���Y���W

		float alpha = 1.0f;

		float rotation = 0.0f;

		if (g_GUIImages[gi].iconID == GUI_ICON_TM_BG)
			alpha = 0.2f;

		if (g_GUIImages[gi].iconID == GUI_ICON_TM_SIMBOL) {
			
			//rotation = GetTimeMachineElapsedTime_ms();
			alpha = 0.3f;
		}

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha), rotation);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

}


void ParseFont(BMPFONT* font, char* file) {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(file);

	// �}�b�v�`�b�v�̏���
	if (result)
	{

		pugi::xml_node fontNode = doc.child("font");

		font->lineHeight = fontNode.child("common").attribute("lineHeight").as_int();
		font->base = fontNode.child("common").attribute("base").as_int();
		font->scaleW = fontNode.child("common").attribute("scaleW").as_int();
		font->scaleH = fontNode.child("common").attribute("scaleH").as_int();

		int fontPagesCount = 0;

		// �t�H���g�̃e�L�X�`���[�̏���
		for (pugi::xml_node xmlFontPage : fontNode.child("pages").children("page"))
		{

			BMPPAGE* fontPage = &font->pages[fontPagesCount];

			fontPage->id = xmlFontPage.attribute("id").as_int();

			char path[128] = {};
			const char* source = xmlFontPage.attribute("file").value();

			memcpy(path, g_FontFolder, strlen(g_FontFolder));
			memcpy(path + strlen(path), source, strlen(source));
			memcpy(fontPage->file, path, strlen(path));

			fontPagesCount++;
		}

		int fontCharsCount = 0;

		for (pugi::xml_node xmlFontPage : fontNode.child("chars").children("char"))
		{
			BMPCHAR* fontChar = &font->chars[fontCharsCount];

			fontChar->id = xmlFontPage.attribute("id").as_int();
			// <char id="32" x="303" y="2046" width="3" height="1" xoffset="-1" yoffset="31" xadvance="6" page="0" chnl="15" />
			fontChar->x = xmlFontPage.attribute("x").as_int();
			fontChar->y = xmlFontPage.attribute("y").as_int();
			fontChar->width = xmlFontPage.attribute("width").as_int();
			fontChar->height = xmlFontPage.attribute("height").as_int();
			fontChar->xoffset = xmlFontPage.attribute("xoffset").as_int();
			fontChar->yoffset = xmlFontPage.attribute("yoffset").as_int();
			fontChar->xadvance = xmlFontPage.attribute("xadvance").as_int();
			fontChar->page = xmlFontPage.attribute("page").as_int();
			fontChar->chnl = xmlFontPage.attribute("chnl").as_int();

			fontChar->textureU = (float)fontChar->x / (float)font->scaleW;
			fontChar->textureV = (float)fontChar->y / (float)font->scaleH;
			fontChar->textureWidth = (float)fontChar->width / (float)font->scaleW;
			fontChar->textureHeigt = (float)fontChar->height / (float)font->scaleH;

			fontCharsCount++;

		}

		BMPCHAR* lineBreakChar = &font->chars[fontCharsCount];
		lineBreakChar->id = 10;
		lineBreakChar->x = 0;
		lineBreakChar->y = 0;
		lineBreakChar->width = 0;
		lineBreakChar->height = 0;
		lineBreakChar->xoffset = 0;
		lineBreakChar->yoffset = 0;
		lineBreakChar->xadvance = 0;
		lineBreakChar->page = 0;
		lineBreakChar->chnl = 0;

		lineBreakChar->textureU = 0;
		lineBreakChar->textureV = 0;
		lineBreakChar->textureWidth = 0;
		lineBreakChar->textureHeigt = 0;

	}

	/*if (g_registeredPlayer != NULL)
	{



	}*/

}

//=============================================================================
// �X�R�A�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
//=============================================================================
BMPTEXT* GetUnusedText()
{

	for (int i = 0; i < TEXTOBJECTS_MAX; i++)
	{

		if (!g_Texts[i].use)
		{
			g_Texts[i].use = TRUE;
			return &g_Texts[i];
		}

	}

	return NULL;
}

void SetText(BMPTEXT* bmpText, wchar_t* text) { 

	memset(bmpText->textChars, 0, TEXTCHAR_MAX);

	int charLength = wcslen(text);
	memcpy(bmpText->richText, text, charLength*sizeof(wchar_t));

	for (int i = 0; i < charLength; i++) {

		uint32_t unicodeId = text[i];

		for (int c = 0; c < FONTCHAR_MAX; c++) {

			if (g_Font.chars[c].id == unicodeId) {
				bmpText->textChars[i] = &g_Font.chars[c];
				break;
			}
		}

	}

	

	int ok = 1;

}

void ClearGUI(void) 
{
	for (int i = 0; i < TEXTOBJECTS_MAX; i++) 
	{
		
		g_Texts[i].use = FALSE;
	
	}
}

//void RegisterPlayer(PLAYER* player) {
//	g_registeredPlayer = player;
//}
//void UnregisterPlayer(PLAYER* player) {
//	g_registeredPlayer = NULL;
//}

void SetPlayerGUI(BOOL enable) {
	g_GUIImages[GUI_ICON_HP].use = enable;
	g_GUIImages[GUI_ICON_TIME].use = enable;
	g_GUIImages[GUI_ICON_KEY].use = enable;
	g_GUIImages[GUI_ICON_MKEY].use = enable;
	g_GUIImages[GUI_ICON_MONEY].use = enable;
}

void SetTMGUI(BOOL enable) {

	g_GUIImages[GUI_ICON_TM_BG].use = enable;
	g_GUIImages[GUI_ICON_TM_SIMBOL].use = enable;

}

