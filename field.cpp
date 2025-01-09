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
#include "pugixml.hpp"


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

#define	SIZE_WH			(1.0f)				// �n�ʂ̃T�C�Y

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexField(void);

void ParseTiles(MAPTILELAYER* mapTileLayer, const char* rawData);
void ParseMap(TILESET tilesets[], MAPTILELAYER mapTileLayers[], FIELDOBJECTGROUP objectGroups[], char* file);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_���
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static FIELD						g_Field;				// �|���S���f�[�^
static int							g_TexNo;				// �e�N�X�`���ԍ�

static char g_TilemapFolder[] = "data/TILEMAP/Crypt/";
static char g_dataFolder[] = "data/";

static char* g_TextureName[] = {
	"data/TEXTURE/field004.jpg",
};
static char* g_debugTextures[1] = {
	"data/TEXTURE/bar_white.png",
};
static int debugTextureIndex = -1;

static TILESET g_Tilesets[TILESET_MAX];
static MAPTILELAYER g_MapTileLayers[MAP_LAYER_MAX];
static FIELDOBJECTGROUP g_ObjectGroups[MAP_OBJLAYERS_MAX];

static TILE	 g_Tiles[TILES_PER_LAYER_MAX];		// �^�C���\����

static int	 g_TileCount = TILES_PER_LAYER_MAX;

// ���_�z��
static VERTEX_3D g_VertexArray[4 * MAX_POLYGON] = {
	{XMFLOAT3(-SIZE_WH/2, 0.0f,  SIZE_WH/2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
	{XMFLOAT3( SIZE_WH/2, 0.0f,  SIZE_WH/2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
	{XMFLOAT3(-SIZE_WH/2, 0.0f, -SIZE_WH/2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
	{XMFLOAT3( SIZE_WH/2, 0.0f, -SIZE_WH/2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitField(void)
{
	// ���_�o�b�t�@�̍쐬
	MakeVertexField();

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

	ParseMap(g_Tilesets, g_MapTileLayers, g_ObjectGroups, "data/MAP/level-0.tmx");

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

	// �ǂ̓����蔻��̕\��
	if (!MAP_DRAW_DEBUG_WALLS)
		return;

	XMFLOAT3 startPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	MAPOBJECT* checkPoints = GetMapObjectsFromLayer(MAPOBJLAYER_LOCATIONS);

	if (checkPoints != NULL)
	{
		startPosition = XMFLOAT3((checkPoints[0].x), 0.0f, (checkPoints[0].y));
	}

	MAPOBJECT* walls = GetMapObjectsFromLayer(MAPOBJLAYER_WALL);
	for (int w = 0; w < MAP_OBJECTS_PER_LAYER_MAX; w++)
	{

		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TILESET_MAX - 1]);

		

		float px = walls[w].x;
		float py = walls[w].y;	
		float pw = walls[w].width;		
		float ph = walls[w].height;

	

		if (pw == 0 && ph == 0)
			continue;

		// �A�j���[�V�����p
		//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
		//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
		//float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
		//float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		// �v���~�e�B�u�g�|���W�ݒ�
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// �}�e���A���ݒ�
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		XMMATRIX w_mtxScl, w_mtxRot, w_mtxTranslate, w_mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		w_mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		w_mtxScl = XMMatrixScaling(pw, 1.0f, ph);
		w_mtxWorld = XMMatrixMultiply(w_mtxWorld, w_mtxScl);

		// ��]�𔽉f
		w_mtxRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
		w_mtxWorld = XMMatrixMultiply(w_mtxWorld, w_mtxRot);

		// �ړ��𔽉f
		w_mtxTranslate = XMMatrixTranslation(px, 0.0f, py);
		w_mtxWorld = XMMatrixMultiply(w_mtxWorld, w_mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&w_mtxWorld);

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			//XMFLOAT4(0.0f, 0.0f, 1.0f, 0.2f), 0.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

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

void ParseMap(TILESET tilesets[], MAPTILELAYER mapTileLayers[], FIELDOBJECTGROUP objectLayers[], char* file)
{

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(file);

	// �}�b�v�`�b�v�̏���
	if (result)
	{

		int tilesetNodeCount = 0;

		// �}�b�v�`�b�v�̃e�L�X�`���[�̏���
		for (pugi::xml_node xmltileset : doc.child("map").children("tileset"))
		{
			TILESET tileset;
			tileset.id = tilesetNodeCount;
			tileset.firstGID = xmltileset.attribute("firstgid").as_int();

			char path[128] = {};
			const char* source = xmltileset.attribute("source").value();

			memcpy(path, g_TilemapFolder, strlen(g_TilemapFolder));

			memcpy(path + strlen(path), source, strlen(source));

			memcpy(tileset.source, path, strlen(path));

			pugi::xml_document tilesetDoc;
			pugi::xml_parse_result tilesetDocResult = tilesetDoc.load_file(tileset.source);

			if (tilesetDocResult) {

				pugi::xml_node tilesetInnerNode = tilesetDoc.child("tileset");

				const char* name = tilesetInnerNode.attribute("name").value();
				memcpy(tileset.name, name, strlen(name));

				tileset.tileWidth = tilesetInnerNode.attribute("tilewidth").as_float();
				tileset.tileHeight = tilesetInnerNode.attribute("tileheight").as_float();
				tileset.tileCount = tilesetInnerNode.attribute("tilecount").as_int();
				tileset.columns = tilesetInnerNode.attribute("columns").as_int();

				pugi::xml_node tilesetTextureNode = tilesetInnerNode.child("image");

				const char* textureSource = tilesetTextureNode.attribute("source").value();

				memcpy(tileset.textureSource, g_dataFolder, strlen(g_dataFolder));

				memcpy(tileset.textureSource + strlen(tileset.textureSource), textureSource + 6, strlen(textureSource));

				tileset.textureW = tilesetTextureNode.attribute("width").as_int();
				tileset.textureH = tilesetTextureNode.attribute("height").as_int();


				////�@�J�X�^���^�C��
				int customTileCount = 0;
				for (pugi::xml_node xmlCustomTile : tilesetInnerNode.children("tile"))
				{

					tileset.customTiles[customTileCount].id = xmlCustomTile.attribute("id").as_int() * MAP_SCALE;
					tileset.customTiles[customTileCount].x = xmlCustomTile.attribute("x").as_float() * MAP_SCALE;
					tileset.customTiles[customTileCount].y = xmlCustomTile.attribute("y").as_float() * MAP_SCALE;
					tileset.customTiles[customTileCount].width = xmlCustomTile.attribute("width").as_float();
					tileset.customTiles[customTileCount].height = xmlCustomTile.attribute("height").as_float();

					const char* tileTextureSource = xmlCustomTile.child("image").attribute("source").value();

					memcpy(tileset.customTiles[customTileCount].textureSource, g_dataFolder, strlen(g_dataFolder));

					memcpy(tileset.customTiles[customTileCount].textureSource + strlen(tileset.customTiles[customTileCount].textureSource), tileTextureSource + 6, strlen(tileTextureSource));

					tileset.customTiles[customTileCount].textureW = xmlCustomTile.child("image").attribute("width").as_int();
					tileset.customTiles[customTileCount].textureH = xmlCustomTile.child("image").attribute("height").as_int();

					// �[���Ŋ��邪�Ȃ�����
					if (tileset.customTiles[customTileCount].width <= 0)
						tileset.customTiles[customTileCount].width = tileset.customTiles[customTileCount].textureW;

					if (tileset.customTiles[customTileCount].height <= 0)
						tileset.customTiles[customTileCount].height = tileset.customTiles[customTileCount].textureH;

					tileset.customTiles[customTileCount].animDivideX = tileset.customTiles[customTileCount].textureW / tileset.customTiles[customTileCount].width;
					tileset.customTiles[customTileCount].animDivideY = tileset.customTiles[customTileCount].textureH / tileset.customTiles[customTileCount].height;
					tileset.customTiles[customTileCount].patternAnimNum = tileset.customTiles[customTileCount].animDivideX / tileset.customTiles[customTileCount].animDivideY;

					tileset.customTiles[customTileCount].width *= MAP_SCALE;
					tileset.customTiles[customTileCount].height *= MAP_SCALE;

					//���������ݒ�
					//Tiled���瓖�������̏������Ⴂ�̂ŁA�Q�[���Ɏg���邽�߂Ƀf�[�^����������B
					float tColX, tColY, tColW, tColH;
					tColX = xmlCustomTile.child("objectgroup").child("object").attribute("x").as_float() * MAP_SCALE;
					tColY = xmlCustomTile.child("objectgroup").child("object").attribute("y").as_float() * MAP_SCALE;
					tColW = xmlCustomTile.child("objectgroup").child("object").attribute("width").as_float() * MAP_SCALE;
					tColH = xmlCustomTile.child("objectgroup").child("object").attribute("height").as_float() * MAP_SCALE;

					if (tColW == 0)
						tColW = tileset.customTiles[customTileCount].width;

					if (tColH == 0)
						tColH = tileset.customTiles[customTileCount].height;

					COLLIDER2DBOX collider = {};

					float tColX2 = tileset.customTiles[customTileCount].width - (tColX + tColW);
					float tColY2 = tileset.customTiles[customTileCount].height - (tColY + tColH);

					collider.offsetX = (tColX - tColX2) / 2;
					collider.offsetY = (tColY - tColY2) / 2;
					collider.width = tColW;
					collider.height = tColH;

					tileset.customTiles[customTileCount].collider = collider;

					customTileCount++;
				}

				tilesets[tilesetNodeCount] = tileset;
				tilesetNodeCount++;

			}
		}

		int mapTileLayerNodeCount = 0;

		// �}�b�v�`�b�v�̃��C���[�̏���
		for (pugi::xml_node mapTileLayerNode : doc.child("map").children("layer"))
		{

			/*std::string str = "Level is: ";
			str.append(mapTileLayerNode.attribute("name").value());

			OutputDebugStringA(str.c_str());*/

			mapTileLayers[mapTileLayerNodeCount].id = mapTileLayerNode.attribute("id").as_int();

			const char* tLName = mapTileLayerNode.attribute("name").value();
			memcpy(mapTileLayers[mapTileLayerNodeCount].name, tLName, strlen(tLName));

			const char* tLClass = mapTileLayerNode.attribute("class").value();
			memcpy(mapTileLayers[mapTileLayerNodeCount].layerClass, tLClass, strlen(tLClass));

			mapTileLayers[mapTileLayerNodeCount].width = mapTileLayerNode.attribute("width").as_float();
			mapTileLayers[mapTileLayerNodeCount].height = mapTileLayerNode.attribute("height").as_float();

			char debug[128] = "";

			memcpy(debug, tLClass, strlen(tLClass));

			int result = strcmp(debug, MAP_DEBUG_KEY);

			if (result == 0 && !MAP_DRAW_DEBUG)
			{
				mapTileLayerNodeCount++;
				continue;
			}

			ParseTiles(&mapTileLayers[mapTileLayerNodeCount], mapTileLayerNode.child("data").child_value());

			mapTileLayerNodeCount++;
		}

		int objectGroupNodeCount = 0;

		for (pugi::xml_node objectGroupNode : doc.child("map").children("objectgroup"))
		{

			objectLayers[objectGroupNodeCount].id = objectGroupNode.attribute("id").as_int();

			const char* oGName = objectGroupNode.attribute("name").value();
			memcpy(objectLayers[objectGroupNodeCount].name, oGName, strlen(oGName));

			const char* oGClass = objectGroupNode.attribute("class").value();
			memcpy(objectLayers[objectGroupNodeCount].objectGroupClass, oGClass, strlen(oGClass));

			int objectsNodeCount = 0;

			for (pugi::xml_node fieldObjectNode : objectGroupNode.children("object")) {

				objectLayers[objectGroupNodeCount].fObjects[objectsNodeCount].id = fieldObjectNode.attribute("id").as_int();

				objectLayers[objectGroupNodeCount].fObjects[objectsNodeCount].gid = fieldObjectNode.attribute("gid").as_int();

				const char* oName = fieldObjectNode.attribute("name").value();
				memcpy(objectLayers[objectGroupNodeCount].fObjects[objectsNodeCount].name, oName, strlen(oName));

				const char* oClass = fieldObjectNode.attribute("type").value();
				memcpy(objectLayers[objectGroupNodeCount].fObjects[objectsNodeCount].objectType, oClass, strlen(oClass));

				objectLayers[objectGroupNodeCount].fObjects[objectsNodeCount].width = fieldObjectNode.attribute("width").as_float() * MAP_SCALE;
				objectLayers[objectGroupNodeCount].fObjects[objectsNodeCount].height = fieldObjectNode.attribute("height").as_float() * MAP_SCALE;

				objectLayers[objectGroupNodeCount].fObjects[objectsNodeCount].x = (fieldObjectNode.attribute("x").as_float() * MAP_SCALE) + objectLayers[objectGroupNodeCount].fObjects[objectsNodeCount].width / 2;
				objectLayers[objectGroupNodeCount].fObjects[objectsNodeCount].y = (fieldObjectNode.attribute("y").as_float() * MAP_SCALE) + objectLayers[objectGroupNodeCount].fObjects[objectsNodeCount].height / 2;


				objectsNodeCount++;

			}

			objectGroupNodeCount++;

		}

	}

}

void ParseTiles(MAPTILELAYER* mapTileLayer, const char* rawData)
{
	// �^�C������
	g_TileCount = 0;

	int converted = 0;
	const char* tok = rawData;
	int i = 0;

	int tileIndex_X = 0;
	int tileIndex_Y = 0;

	// �f�[�^�̉��
	do
	{
		int tileId;

		converted = sscanf(tok, "%d", &tileId);

		tok = strchr(tok, ',');

		if (tok == NULL)
			break;

		TILE nTile;

		nTile.id = tileId;
		nTile.use = tileId > 0 ? TRUE : FALSE; // �^�C�� ID �� 0 �̏ꍇ�A�^�C���͕\������܂���B

		if (nTile.use) {

			TILESET* relatedTileSet = GetTilesetFromTileID(nTile.id);

			if (relatedTileSet != NULL) {

				float newX = (tileIndex_X * relatedTileSet->tileWidth) + relatedTileSet->tileWidth / 2;
				float newY = (tileIndex_Y * relatedTileSet->tileHeight) + relatedTileSet->tileWidth / 2;

				nTile.pos = XMFLOAT3(newX * MAP_SCALE, newY * MAP_SCALE, 0.0f);
				nTile.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
				nTile.scl = XMFLOAT3(1.0f * MAP_SCALE, 1.0f * MAP_SCALE, 1.0f * MAP_SCALE);
				nTile.texNo = relatedTileSet->id;

				float tileSetTileWidth = relatedTileSet->tileWidth;
				float tileSetTileHeight = relatedTileSet->tileHeight;

				nTile.w = tileSetTileWidth * nTile.scl.x;
				nTile.h = tileSetTileHeight * nTile.scl.y;

				int textureIndex = nTile.id - relatedTileSet->firstGID;

				int textureUIndex = textureIndex % relatedTileSet->columns;

				int textureVIndex = floor(textureIndex / relatedTileSet->columns);

				int tileSetTextureWidth = relatedTileSet->textureW;
				int tileSetTextureHeight = relatedTileSet->textureH;

				nTile.textureU = (textureUIndex * tileSetTileWidth) / tileSetTextureWidth;
				nTile.textureV = (textureVIndex * tileSetTileHeight) / tileSetTextureHeight;
				nTile.textureWidth = tileSetTileWidth / tileSetTextureWidth;
				nTile.textureHeigt = tileSetTileHeight / tileSetTextureHeight;

			}

		}

		mapTileLayer->tiles[i] = nTile;
		g_TileCount++;

		tileIndex_X++;

		if (tileIndex_X > mapTileLayer->width - 1)
		{
			tileIndex_X = 0;
			tileIndex_Y++;
		}


		tok += 1;

		i++;

	} while (converted != 0);


}

//=============================================================================
// Field�\���̂̌��J�֐�
//=============================================================================
TILE* GetField(void)
{
	return &g_Tiles[0];
}


// �����Ă�G�l�~�[�̐�
int GetFieldCount(void)
{
	return g_TileCount;
}

TILESET* GetTilesetFromTileID(int tileId)
{
	for (int t = 0; t < TILESET_MAX; t++) {

		int tileIDRangeMinValue = g_Tilesets[t].firstGID;
		int tileIDrangeMaxValue = g_Tilesets[t].firstGID + g_Tilesets[t].tileCount - 1;

		if (tileId >= tileIDRangeMinValue && tileId <= tileIDrangeMaxValue)
		{
			return &g_Tilesets[t];
		}

	}

	return NULL;

}

TILESET* GetTilesetByName(const char* name)
{

	for (int t = 0; t < TILESET_MAX; t++) {

		int tileIDRangeMinValue = g_Tilesets[t].firstGID;
		int tileIDrangeMaxValue = g_Tilesets[t].firstGID + g_Tilesets[t].tileCount - 1;

		if (strcmp(name, g_Tilesets[t].name) == 0)
		{
			return &g_Tilesets[t];
		}

	}

	return NULL;

}

MAPOBJECT** GetMapObjectsByClass(const char* objectType)
{
	MAPOBJECT* fieldObjects[MAP_OBJECTS_PER_LAYER_MAX];

	int foundObjects = 0;

	for (int og = 0; og < MAP_OBJLAYERS_MAX; og++)
	{

		for (int o = 0; o < MAP_OBJECTS_PER_LAYER_MAX; o++)
		{
			if (strcmp(g_ObjectGroups[og].fObjects[o].objectType, objectType) == 0)
			{
				fieldObjects[foundObjects] = &g_ObjectGroups[og].fObjects[o];
				foundObjects++;
			}
		}

	}

	if (foundObjects == 0)
		return NULL;

	return fieldObjects;

}

MAPOBJECT* GetMapObjectsFromLayer(const char* objectLayer)
{
	MAPOBJECT* fieldObjects = NULL;

	int foundGroups = 0;

	for (int og = 0; og < MAP_OBJLAYERS_MAX; og++)
	{


		if (strcmp(g_ObjectGroups[og].objectGroupClass, objectLayer) == 0)
		{

			fieldObjects = g_ObjectGroups[og].fObjects;

			foundGroups++;

			// ���݁A�����ɂ͈�̃O���[�v�݂̂̕Ԃ����ł��܂��B
			break;
		}

	}

	return fieldObjects;

}

MAPOBJECT* GetMapObjectByNameFromLayer(const char* objectLayer, const char* objectName)
{
	MAPOBJECT* fieldObject = NULL;

	int foundGroups = 0;

	for (int og = 0; og < MAP_OBJLAYERS_MAX; og++)
	{


		if (strcmp(g_ObjectGroups[og].objectGroupClass, objectLayer) == 0)
		{

			for (int o = 0; o < MAP_OBJECTS_PER_LAYER_MAX; o++)
			{

				if (strcmp(g_ObjectGroups[og].objectGroupClass, objectLayer) == 0)
				{
					fieldObject = &g_ObjectGroups[og].fObjects[o];


					// ���݁A�����ɂ͍ŏ��̃I�u�W�F�N�g�݂̂̕Ԃ����ł��܂��B
					break;
				}

			}

			foundGroups++;

			// ���݁A�����ɂ͍ŏ��̃O���[�v�݂̂̕Ԃ����ł��܂��B
			break;
		}

	}

	return fieldObject;

}