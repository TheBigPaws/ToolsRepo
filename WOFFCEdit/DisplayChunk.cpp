#include <string>
#include "DisplayChunk.h"
#include "Game.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;

DisplayChunk::DisplayChunk()
{
	//terrain size in meters. note that this is hard coded here, we COULD get it from the terrain chunk along with the other info from the tool if we want to be more flexible.
	m_terrainSize = 512;
	m_terrainHeightScale = 0.25;  //convert our 0-256 terrain to 64
	m_textureCoordStep = 1.0 / (TERRAINRESOLUTION-1);	//-1 becuase its split into chunks. not vertices.  we want tthe last one in each row to have tex coord 1
	m_terrainPositionScalingFactor = m_terrainSize / (TERRAINRESOLUTION-1);


	for (int i = 0; i < 2 * TERRAINRESOLUTION * TERRAINRESOLUTION; i++) {
		terrainTexturesHolder_IDCS[i] = 0;
	}

}


DisplayChunk::~DisplayChunk()
{
}

void DisplayChunk::PopulateChunkData(ChunkObject * SceneChunk)
{
	m_name = SceneChunk->name;
	m_chunk_x_size_metres = SceneChunk->chunk_x_size_metres;
	m_chunk_y_size_metres = SceneChunk->chunk_y_size_metres;
	m_chunk_base_resolution = SceneChunk->chunk_base_resolution;
	m_heightmap_path = SceneChunk->heightmap_path;
	m_tex_diffuse_path = SceneChunk->tex_diffuse_path;



	m_tex_splat_alpha_path = SceneChunk->tex_splat_alpha_path;
	m_tex_splat_1_path = SceneChunk->tex_splat_1_path;
	m_tex_splat_2_path = SceneChunk->tex_splat_2_path;
	m_tex_splat_3_path = SceneChunk->tex_splat_3_path;
	m_tex_splat_4_path = SceneChunk->tex_splat_4_path;
	m_render_wireframe = SceneChunk->render_wireframe;
	m_render_normals = SceneChunk->render_normals;
	m_tex_diffuse_tiling = SceneChunk->tex_diffuse_tiling;
	m_tex_splat_1_tiling = SceneChunk->tex_splat_1_tiling;
	m_tex_splat_2_tiling = SceneChunk->tex_splat_2_tiling;
	m_tex_splat_3_tiling = SceneChunk->tex_splat_3_tiling;
	m_tex_splat_4_tiling = SceneChunk->tex_splat_4_tiling;
}

void DisplayChunk::RenderBatch(std::shared_ptr<DX::DeviceResources>  DevResources)
{


	auto context = DevResources->GetD3DDeviceContext();
	context->IASetInputLayout(m_terrainInputLayout.Get());
	float lastTXT = terrainTexturesHolder_IDCS[0];
	m_terrainEffect->SetTexture(m_texture_diffuse[lastTXT]);
	m_terrainEffect->Apply(context);

	m_batch->Begin();

	int index_;

	for (size_t i = 0; i < TERRAINRESOLUTION-1; i++)	//looping through QUADS.  so we subtrack one from the terrain array or it will try to draw a quad starting with the last vertex in each row. Which wont work
	{
		

		for (size_t j = 0; j < TERRAINRESOLUTION-1; j++)//same as above
		{

			//bool changeBatches = false;
			index_ = 2 * ((TERRAINRESOLUTION * i) + j);

			//m_batch->DrawQuad(m_terrainGeometry[i][j], m_terrainGeometry[i][j + 1], m_terrainGeometry[i + 1][j + 1], m_terrainGeometry[i + 1][j]); //bottom left bottom right, top right top left.

			if (terrainTexturesHolder_IDCS[index_] != lastTXT) {
				m_batch->End();
				lastTXT = terrainTexturesHolder_IDCS[index_];
				m_terrainEffect->SetTexture(m_texture_diffuse[lastTXT]);
				m_terrainEffect->Apply(context);
				m_batch->Begin();
			}
			m_batch->DrawTriangle(m_terrainGeometry[i][j], m_terrainGeometry[i][j + 1], m_terrainGeometry[i + 1][j + 1]); //bottom left bottom right, top right top left.
			if (terrainTexturesHolder_IDCS[index_ + 1] != lastTXT) {
				m_batch->End();
				lastTXT = terrainTexturesHolder_IDCS[index_ + 1];
				m_terrainEffect->SetTexture(m_texture_diffuse[lastTXT]);
				m_terrainEffect->Apply(context);
				m_batch->Begin();
			}
			m_batch->DrawTriangle(m_terrainGeometry[i][j], m_terrainGeometry[i + 1][j], m_terrainGeometry[i + 1][j + 1]); //bottom left bottom right, top right top left.

		}

	}




m_batch->End();

}

void DisplayChunk::InitialiseBatch()
{

	//m_chunk.tex_diffuse_path = "database/data/rock.dds";

	//build geometry for our terrain array
	//iterate through all the vertices of our required resolution terrain.
	int index = 0;

	for (size_t i = 0; i < TERRAINRESOLUTION; i++)
	{
		for (size_t j = 0; j < TERRAINRESOLUTION; j++)
		{

			index = (TERRAINRESOLUTION * i) + j;
			m_terrainGeometry[i][j].position = Vector3(j * m_terrainPositionScalingFactor - (0.5 * m_terrainSize), (float)(m_heightMap[index]) * m_terrainHeightScale, i * m_terrainPositionScalingFactor - (0.5 * m_terrainSize));	//This will create a terrain going from -64->64.  rather than 0->128.  So the center of the terrain is on the origin
			m_terrainGeometry[i][j].normal = Vector3(0.0f, 1.0f, 0.0f);						//standard y =up
			m_terrainGeometry[i][j].textureCoordinate = Vector2(((float)m_textureCoordStep * j) * m_tex_diffuse_tiling, ((float)m_textureCoordStep * i) * m_tex_diffuse_tiling);				//Spread tex coords so that its distributed evenly across the terrain from 0-1

		}
	}
	//terr reso = 128
	//-256 , -256 to 256, 256
	//terrain pos scaling is 4
	//terraub=inSize is 512



	CalculateTerrainNormals();


}

float DisplayChunk::getYatPos(Vector3 pos_) {
	float ret_ = 0.0f;

	Vector2 indices_ = getIndicesOfTriangleUnderPos(pos_);

	//RayIntersectsTriangle(Vector3(pos_.x,pos_.y + 10,pos_.z),Vector3(0,-1000,0),)
	Vector3 outPos;
	if (RayIntersectsTriangle(Vector3(pos_.x, pos_.y + 1000, pos_.z), Vector3(0, -1000, 0), m_terrainGeometry[int(indices_.x)][int(indices_.y)].position, m_terrainGeometry[int(indices_.x) + 1][int(indices_.y)].position, m_terrainGeometry[int(indices_.x) + 1][int(indices_.y) + 1].position, outPos)) {

		ret_ = outPos.y;
	}
	if (RayIntersectsTriangle(Vector3(pos_.x, pos_.y + 1000, pos_.z), Vector3(0, -1000, 0), m_terrainGeometry[int(indices_.x)][int(indices_.y)].position, m_terrainGeometry[int(indices_.x)][int(indices_.y) + 1].position, m_terrainGeometry[int(indices_.x) + 1][int(indices_.y) + 1].position, outPos)) {

		ret_ = outPos.y;
	}
	return ret_;
}

void DisplayChunk::mouseIntersect(DirectX::SimpleMath::Vector3 camPos, DirectX::SimpleMath::Vector3 mouseVector) {



	isIntersecting = false;
	//selectedTriaIndex = -1;
	//float closestDistance = 999999;


	//for (size_t i = 0; i < TERRAINRESOLUTION - 1; i++)
	//{
	//	for (size_t j = 0; j < TERRAINRESOLUTION - 1; j++)
	//	{
	//
	//		int index_ = 2 * ((TERRAINRESOLUTION * i) + j);
	//		Vector3 outPos;
	//		if (RayIntersectsTriangle(camPos, camLookDir, m_terrainGeometry[i][j].position, m_terrainGeometry[i + 1][j].position, m_terrainGeometry[i + 1][j + 1].position, outPos) && (Vector3(m_terrainGeometry[i][j].position) - camPos).Length() < closestDistance) {
	//
	//			closestDistance = (outPos - camPos).Length();
	//			planeIntersectPoint = outPos;
	//			isIntersecting = true;
	//
	//			selectedTriaIndex = index_;
	//
	//		}
	//		if (RayIntersectsTriangle(camPos, camLookDir, m_terrainGeometry[i][j].position, m_terrainGeometry[i][j + 1].position, m_terrainGeometry[i + 1][j + 1].position, outPos) && (Vector3(m_terrainGeometry[i][j].position) - camPos).Length() < closestDistance) {
	//
	//			closestDistance = (outPos - camPos).Length();
	//			planeIntersectPoint = outPos;
	//			isIntersecting = true;
	//			selectedTriaIndex = index_ + 1;
	//		}
	//
	//	}
	//}


	//MORE EFFICIENT :)) - make a ray defined by campos and lookdir, slowly stretch it and look for first intersection from up
	bool outOfBounds = false;
	int forwardMoveIt = 1;
	Vector3 nowV;
	while(!outOfBounds){
		nowV = camPos + forwardMoveIt * mouseVector;

		Vector2 idxs_ = getIndicesOfTriangleUnderPos(nowV);
		Vector3 outPos;

		if (RayIntersectsTriangle(camPos, mouseVector, m_terrainGeometry[int(idxs_.x)][int(idxs_.y)].position, m_terrainGeometry[int(idxs_.x + 1)][int(idxs_.y)].position, m_terrainGeometry[int(idxs_.x + 1)][int(idxs_.y + 1)].position, outPos)) {

			//closestDistance = (outPos - camPos).Length();
			planeIntersectPoint = outPos;
			isIntersecting = true;
			//Vector3 myNormal = getTriaNormal(m_terrainGeometry[int(idxs_.x)][int(idxs_.y)].position, m_terrainGeometry[int(idxs_.x + 1)][int(idxs_.y)].position, m_terrainGeometry[int(idxs_.x + 1)][int(idxs_.y + 1)].position);

			return;

		}
		if (RayIntersectsTriangle(camPos, mouseVector, m_terrainGeometry[int(idxs_.x)][int(idxs_.y)].position, m_terrainGeometry[int(idxs_.x)][int(idxs_.y + 1)].position, m_terrainGeometry[int(idxs_.x + 1)][int(idxs_.y + 1)].position, outPos)) {

			//closestDistance = (outPos - camPos).Length();
			planeIntersectPoint = outPos;
			isIntersecting = true;
			//Vector3 myNormal = getTriaNormal(m_terrainGeometry[int(idxs_.x)][int(idxs_.y)].position, m_terrainGeometry[int(idxs_.x)][int(idxs_.y + 1)].position, m_terrainGeometry[int(idxs_.x + 1)][int(idxs_.y + 1)].position);

			return;
		}
		forwardMoveIt++;

		//exit if out of range
		if (abs(nowV.x) > 512 || abs(nowV.z) > 512) {
			outOfBounds = true;
		}
	}
}

Vector2 DisplayChunk::getIndicesOfTriangleUnderPos(Vector3 pos) {
	Vector2 retV;

	//j*m_terrainPositionScalingFactor-(0.5*m_terrainSize)
	int i_ = (pos.z + m_terrainSize * 0.5) / m_terrainPositionScalingFactor;

	int j_ = (pos.x + m_terrainSize*0.5)/ m_terrainPositionScalingFactor;

	retV.x = i_;
	retV.y = j_;

	return retV;
}

void DisplayChunk::raiseGround(float dt) {

	if (!isIntersecting) {
		return;
	}

	for (size_t i = 0; i < TERRAINRESOLUTION; i++)
	{
		for (size_t j = 0; j < TERRAINRESOLUTION; j++)
		{

			float Pdist = (planeIntersectPoint - m_terrainGeometry[i][j].position).Length();

			if (Pdist< terrainEditRadius) {
				m_terrainGeometry[i][j].position.y += (1.0f - Pdist/ terrainEditRadius) * dt* terrainEditSpeed;
			}

		}
	}
	CalculateTerrainNormals();

}




void DisplayChunk::lowerGround(float dt) {

	if (!isIntersecting) {
		return;
	}

	for (size_t i = 0; i < TERRAINRESOLUTION; i++)
	{
		for (size_t j = 0; j < TERRAINRESOLUTION; j++)
		{

			float Pdist = (planeIntersectPoint - m_terrainGeometry[i][j].position).Length();

			if (Pdist < terrainEditRadius) {
				m_terrainGeometry[i][j].position.y -= (1.0f - Pdist / terrainEditRadius) * dt * terrainEditSpeed;
			}

		}
	}
	CalculateTerrainNormals();
}

void DisplayChunk::levelGround(float dt) {
	if (!isIntersecting) {
		return;
	}

	for (size_t i = 0; i < TERRAINRESOLUTION; i++)
	{
		for (size_t j = 0; j < TERRAINRESOLUTION; j++)
		{

			DirectX::SimpleMath::Vector3 distanceVector = planeIntersectPoint - m_terrainGeometry[i][j].position;
			distanceVector.y = 0;

			float Pdist = distanceVector.Length();


			if (Pdist < terrainEditRadius) {

				float Ydir = planeIntersectPoint.y - m_terrainGeometry[i][j].position.y;

				if (abs(Ydir) > 0) {
					m_terrainGeometry[i][j].position.y += dt * terrainEditSpeed * (Ydir / abs(Ydir));

					if (abs(planeIntersectPoint.y - m_terrainGeometry[i][j].position.y) < 0.25f) {
						m_terrainGeometry[i][j].position.y = planeIntersectPoint.y;
					}

				}

			}

		}
	}
	CalculateTerrainNormals();

}


void DisplayChunk::paintGround(float dt, int paintType) {
	if (!isIntersecting) {
		return;
	}

	for (size_t i = 0; i < TERRAINRESOLUTION - 1; i++)
	{
		for (size_t j = 0; j < TERRAINRESOLUTION - 1; j++)
		{

			//Vector2 triaIndices
			int index_ = 2 * ((TERRAINRESOLUTION * i) + j);


			Vector3 avgPos_ONE = m_terrainGeometry[i][j].position + m_terrainGeometry[i][j + 1].position + m_terrainGeometry[i+1][j+1].position;
			avgPos_ONE /= 3;
			
			Vector3 avgPos_TWO = m_terrainGeometry[i][j].position +  m_terrainGeometry[i + 1][j].position + m_terrainGeometry[i + 1][j + 1].position;
			avgPos_TWO /= 3;


			DirectX::SimpleMath::Vector3 distanceVector = planeIntersectPoint - avgPos_ONE;
			DirectX::SimpleMath::Vector3 distanceVector_TWO = planeIntersectPoint - avgPos_TWO;
			float Pdist = distanceVector.Length();
			float Pdist_TWO = distanceVector_TWO.Length();



			if (Pdist < terrainEditRadius && paintType != terrainTexturesHolder_IDCS[index_]) {
				terrainTexturesHolder_IDCS[index_] = paintType;
			}
			if (Pdist_TWO < terrainEditRadius && paintType != terrainTexturesHolder_IDCS[index_ + 1]) {
				terrainTexturesHolder_IDCS[index_ + 1] = paintType;
			}
		}
	}
}



void DisplayChunk::LoadHeightMap(std::shared_ptr<DX::DeviceResources>  DevResources)
{

	auto device = DevResources->GetD3DDevice();
	auto devicecontext = DevResources->GetD3DDeviceContext();

	//load in heightmap .raw
	FILE *pFile = NULL;

	// Open The File In Read / Binary Mode.

	pFile = fopen(m_heightmap_path.c_str(), "rb");
	// Check To See If We Found The File And Could Open It
	if (pFile == NULL)
	{
		// Display Error Message And Stop The Function
		MessageBox(NULL, L"Can't Find The Height Map!", L"Error", MB_OK);
		return;
	}

	// Here We Load The .RAW File Into Our pHeightMap Data Array
	// We Are Only Reading In '1', And The Size Is (Width * Height)
	fread(m_heightMap, 1, TERRAINRESOLUTION*TERRAINRESOLUTION, pFile);

	fclose(pFile);

	//load in texture diffuse
	
	//load the diffuse texture
	//m_tex_diffuse_path = "database/data/rock.dds";

	std::wstring texturewstr;


	//link for cobble
	//https://www.artstation.com/artwork/1GA8K

	//link for grass
	//https://www.pinterest.com/wiredfarm/grass/

	//link for trash
	//https://www.pinterest.cl/pin/290482244715194117/
	for (int i = 0; i < 4; i++) {
		switch (i) {
		case 0:
			texturewstr = StringToWCHART(m_tex_diffuse_path);
			break;
		case 1:
			texturewstr = StringToWCHART("database/data/wowCobble.dds");
			break;
		case 2:
			texturewstr = StringToWCHART("database/data/GrassTXT.dds");
			break;
		case 3:
			texturewstr = StringToWCHART("database/data/TrashTXT.dds");
			break;
		default:
			texturewstr = StringToWCHART(m_tex_diffuse_path);
			break;
		}
	
	

		HRESULT rs;
		ID3D11ShaderResourceView* ref_SRV;
		m_texture_diffuse.push_back(ref_SRV);
		rs = CreateDDSTextureFromFile(device, texturewstr.c_str(), NULL, &m_texture_diffuse.back());	//load tex into Shader resource	view and resource


	}
	


	//std::wstring texturewstr2 = StringToWCHART("database/data/wowCobble.dds");
	//HRESULT rs2;
	//rs2 = CreateDDSTextureFromFile(device, texturewstr2.c_str(), NULL, &m_texture_diffuse2);	//load tex into Shader resource	view and resource

	//m_texture_diffuse->GetResource()->

	//setup terrain effect
	m_terrainEffect = std::make_unique<BasicEffect>(device);
	m_terrainEffect->EnableDefaultLighting();
	m_terrainEffect->SetLightingEnabled(true);
	m_terrainEffect->SetTextureEnabled(true);
	m_terrainEffect->SetTexture(m_texture_diffuse.back());

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_terrainEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	//setup batch
	DX::ThrowIfFailed(
		device->CreateInputLayout(VertexPositionNormalTexture::InputElements,
			VertexPositionNormalTexture::InputElementCount,
			shaderByteCode,
			byteCodeLength,
			m_terrainInputLayout.GetAddressOf())
		);

	m_batch = std::make_unique<PrimitiveBatch<VertexPositionNormalTexture>>(devicecontext);

	
}

void DisplayChunk::SaveHeightMap()
{
/*	for (size_t i = 0; i < TERRAINRESOLUTION*TERRAINRESOLUTION; i++)
	{
		m_heightMap[i] = 0;
	}*/

	FILE *pFile = NULL;

	// Open The File In Read / Binary Mode.
	pFile = fopen(m_heightmap_path.c_str(), "wb+");;
	// Check To See If We Found The File And Could Open It
	if (pFile == NULL)
	{
		// Display Error Message And Stop The Function
		MessageBox(NULL, L"Can't Find The Height Map!", L"Error", MB_OK);
		return;
	}

	fwrite(m_heightMap, 1, TERRAINRESOLUTION*TERRAINRESOLUTION, pFile);
	fclose(pFile);
	
}

void DisplayChunk::UpdateTerrain()
{
	//all this is doing is transferring the height from the heigtmap into the terrain geometry.
	int index;
	for (size_t i = 0; i < TERRAINRESOLUTION; i++)
	{
		for (size_t j = 0; j < TERRAINRESOLUTION; j++)
		{
			index = (TERRAINRESOLUTION * i) + j;
			m_terrainGeometry[i][j].position.y = (float)(m_heightMap[index])*m_terrainHeightScale;	
		}
	}
	CalculateTerrainNormals();

}

void DisplayChunk::GenerateHeightmap()
{
	//insert how YOU want to update the heigtmap here! :D
}

void DisplayChunk::CalculateTerrainNormals()
{
	int index1, index2, index3, index4;
	DirectX::SimpleMath::Vector3 upDownVector, leftRightVector, normalVector;



	for (int i = 0; i<(TERRAINRESOLUTION - 1); i++)
	{
		for (int j = 0; j<(TERRAINRESOLUTION - 1); j++)
		{
			upDownVector.x = (m_terrainGeometry[i + 1][j].position.x - m_terrainGeometry[i - 1][j].position.x);
			upDownVector.y = (m_terrainGeometry[i + 1][j].position.y - m_terrainGeometry[i - 1][j].position.y);
			upDownVector.z = (m_terrainGeometry[i + 1][j].position.z - m_terrainGeometry[i - 1][j].position.z);

			leftRightVector.x = (m_terrainGeometry[i][j - 1].position.x - m_terrainGeometry[i][j + 1].position.x);
			leftRightVector.y = (m_terrainGeometry[i][j - 1].position.y - m_terrainGeometry[i][j + 1].position.y);
			leftRightVector.z = (m_terrainGeometry[i][j - 1].position.z - m_terrainGeometry[i][j + 1].position.z);


			leftRightVector.Cross(upDownVector, normalVector);	//get cross product
			normalVector.Normalize();			//normalise it.

			m_terrainGeometry[i][j].normal = normalVector;	//set the normal for this point based on our result
		}
	}
}


//https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool DisplayChunk::RayIntersectsTriangle(Vector3 rayOrigin, Vector3 rayVector, Vector3 v1_t, Vector3 v2_t, Vector3 v3_t, Vector3& outIntersectionPoint)
{
	const float EPSILON = 0.0000001;
	Vector3 vertex0 = v1_t;
	Vector3 vertex1 = v2_t;
	Vector3 vertex2 = v3_t;
	Vector3 edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = rayVector.Cross(edge2);
	a = edge1.Dot(h);
	if (a > -EPSILON && a < EPSILON)
		return false;    // This ray is parallel to this triangle.
	f = 1.0 / a;
	s = rayOrigin - vertex0;
	u = f * s.Dot(h);
	if (u < 0.0 || u > 1.0)
		return false;
	q = s.Cross(edge1);
	v = f * rayVector.Dot(q);
	if (v < 0.0 || u + v > 1.0)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * edge2.Dot(q);
	if (t > EPSILON) // ray intersection
	{
		outIntersectionPoint = rayOrigin + rayVector * t;
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return false;
}