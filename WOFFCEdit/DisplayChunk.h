#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "ChunkObject.h"

//geometric resoltuion - note,  hard coded.
#define TERRAINRESOLUTION 128

enum TerrainEditType{ RAISE,LOWER,FLATTEN,NOTHING};


using namespace DirectX::SimpleMath;

class DisplayChunk
{
public:
	DisplayChunk();
	~DisplayChunk();
	void PopulateChunkData(ChunkObject * SceneChunk);
	void RenderBatch(std::shared_ptr<DX::DeviceResources>  DevResources);
	void InitialiseBatch();	//initial setup, base coordinates etc based on scale
	void LoadHeightMap(std::shared_ptr<DX::DeviceResources>  DevResources);
	void SaveHeightMap();			//saves the heigtmap back to file.
	void UpdateTerrain();			//updates the geometry based on the heigtmap
	void GenerateHeightmap();		//creates or alters the heightmap
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>>  m_batch;
	std::unique_ptr<DirectX::BasicEffect>       m_terrainEffect;

	ID3D11ShaderResourceView *					m_texture_diffuse;				//diffuse texture
	ID3D11ShaderResourceView *					m_texture_diffuse2;				//diffuse texture
	Microsoft::WRL::ComPtr<ID3D11InputLayout>   m_terrainInputLayout;


	void raiseGround(float dt);
	void lowerGround(float dt);
	void levelGround(float dt);

	float getYatPos(Vector3 pos_);

	void selectVertex(Vector3 camPos, Vector3 camLookDir);
	bool RayIntersectsTriangle(Vector3 rayOrigin, Vector3 rayVector, Vector3 v1_t, Vector3 v2_t, Vector3 v3_t, Vector3& outIntersectionPoint); // <- https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	Vector3 planeIntersectPoint;
	bool isIntersecting = false;

	int selectedTriaIndex = -1;

	TerrainEditType currentEditType = NOTHING;


	Vector2 getIndicesOfTriangleUnderPos(Vector3 pos);


	float terrainEditRadius = 10.0f;
	float terrainEditSpeed = 10.0f;

	//float terrainAlphas[2 * (TERRAINRESOLUTION - 1) * (TERRAINRESOLUTION - 1)];
private:
	
	DirectX::VertexPositionNormalTexture m_terrainGeometry[TERRAINRESOLUTION][TERRAINRESOLUTION];
	BYTE m_heightMap[TERRAINRESOLUTION*TERRAINRESOLUTION];
	void CalculateTerrainNormals();

	float	m_terrainHeightScale;
	int		m_terrainSize;				//size of terrain in metres
	float	m_textureCoordStep;			//step in texture coordinates between each vertex row / column
	float   m_terrainPositionScalingFactor;	//factor we multiply the position by to convert it from its native resolution( 0- Terrain Resolution) to full scale size in metres dictated by m_Terrainsize
	
	std::string m_name;
	int m_chunk_x_size_metres;
	int m_chunk_y_size_metres;
	int m_chunk_base_resolution;
	std::string m_heightmap_path;
	std::string m_tex_diffuse_path;
	std::string m_tex_splat_alpha_path;
	std::string m_tex_splat_1_path;
	std::string m_tex_splat_2_path;
	std::string m_tex_splat_3_path;
	std::string m_tex_splat_4_path;
	bool m_render_wireframe;
	bool m_render_normals;
	int m_tex_diffuse_tiling;
	int m_tex_splat_1_tiling;
	int m_tex_splat_2_tiling;
	int m_tex_splat_3_tiling;
	int m_tex_splat_4_tiling;

};

