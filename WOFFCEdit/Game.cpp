//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "DisplayObject.h"

#include <string>


using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()

{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
	m_displayList.clear();
	
	//initial Settings
	//modes
	m_grid = false;

	//functional
	m_movespeed = 15.0f;


}

Game::~Game()
{

#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);


    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	//m_deviceResources->WindowSizeChanged()

	GetClientRect(window, &m_ScreenDimensions);

#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

void Game::ChangeGridState()
{
	m_grid = !m_grid;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick(InputCommands *Input)
{
	//copy over the input commands so we have a local version to use elsewhere.
	m_InputCommands = *Input;
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

    Render();
}

DirectX::SimpleMath::Vector3 Game::GetRotationFromDirection(DirectX::SimpleMath::Vector3 Dir)
{
	DirectX::SimpleMath::Vector3 UpVec = Vector3(0.0f, 1.0f, 0.0f);

	Dir.Normalize();


	float dotProduct = UpVec.Dot(Dir);

	if (dotProduct >= 1.0f) {
		return Vector3::Zero;
	}
	else if (dotProduct <= -1.0f) {
		return Vector3(180.0f, 0.0f, 0.0f);
	}
	else {
		float angle = acosf(dotProduct) * 180.0f / DirectX::XM_PI;
		Vector3 axis = UpVec.Cross(Dir);
		axis.Normalize();
		return angle * axis;
	}
}

void Game::handleInput(float dt) {
	
	float dForward = 0.0f;
	float dRight = 0.0f;
	
	//when rmb is down and focusing is off update rot
	if (m_InputCommands.RMBdown && (!Camera_.focusingCamera || (Camera_.focusingCamera && Camera_.arcBallCam)))
	{

		Camera_.UpdateCameraRotation(m_InputCommands.rotate[0], m_InputCommands.rotate[1], dt);

	}
	//movement blocked when camera is being focused
	if(!Camera_.focusingCamera){

		
		
		if (m_InputCommands.forward)
		{
			dForward += m_movespeed * dt;
		}
		if (m_InputCommands.back)
		{
			dForward -= m_movespeed * dt;
		}
		if (m_InputCommands.right)
		{
			dRight += m_movespeed * dt;
		}
		if (m_InputCommands.left)
		{
			dRight -= m_movespeed * dt;
		}
	}
	Camera_.UpdateCameraPosition(dForward, dRight);

	//update arcball distance
	if (m_InputCommands.zoomIn_AB && Camera_.ArcBallDistance > 1.0f) {
		Camera_.ArcBallDistance -= dt * 3.0f;
	}
	if (m_InputCommands.zoomOut_AB) {
		Camera_.ArcBallDistance += dt * 3.0f;
	}

	if (m_InputCommands.focus && !m_InputCommands.RMBdown) {
		Camera_.focusingCamera = true;
		Camera_.arcBallCam = false;
	}


	
	//update intersect
	if (MyAction == FLATTEN_TERRAIN || MyAction == RAISE_TERRAIN || MyAction == PAINT_TERRAIN || MyAction == LOWER_TERRAIN || MyAction == MOVE_OBJECT) {
		m_displayChunk.mouseIntersect(Camera_.m_camPosition, getClickingVector());
	}

	if (m_InputCommands.LMBdown)
	{

		switch (MyAction) {
		
		case SELECT_OBJECT:
			if (!alreadyPicked && MousePicking() >= 0) {
				
				*selectedIDobject = MousePicking();
				alreadyPicked = true;
			}
			
			break;
		case MOVE_OBJECT:
			
			if ( alreadyPicked && *selectedIDobject >= 0)
			{
				m_displayList[*selectedIDobject].m_position = m_displayChunk.planeIntersectPoint + m_displayChunk.planeIntersectPointNormal * m_displayList[*selectedIDobject].m_scale * 0.52f;
				m_displayList[*selectedIDobject].m_orientation = GetRotationFromDirection(m_displayChunk.planeIntersectPointNormal);
			}
			else if(!alreadyPicked && MousePicking() >= 0) {
				*selectedIDobject = MousePicking();
				alreadyPicked = true;
			}
			//else {
			//	*selectedIDobject = -1;
			//}
			
			break;

		case ROTATE_OBJECT:
			if (alreadyPicked && *selectedIDobject >= 0)
			{
				m_displayList[*selectedIDobject].m_orientation.y += (m_InputCommands.MouseXY[0] - lastPos[0]) * 100.0f*dt;
				m_displayList[*selectedIDobject].m_orientation.x += (m_InputCommands.MouseXY[1] - lastPos[1]) * 100.0f*dt;

				

			}
			else if (!alreadyPicked && MousePicking() >= 0) {
				*selectedIDobject = MousePicking();
				alreadyPicked = true;
			}
			//else {
			//	*selectedIDobject = -1;
			//}

			break;

		case SCALE_OBJECT:
			if (alreadyPicked && *selectedIDobject >= 0)
			{
				float amount = (m_InputCommands.MouseXY[1] - lastPos[1]);
#
				m_displayList[*selectedIDobject].m_scale.x += amount * -0.8f * dt;
				m_displayList[*selectedIDobject].m_scale.y += amount * -0.8f * dt;
				m_displayList[*selectedIDobject].m_scale.z += amount * -0.8f * dt;


			}
			else if (!alreadyPicked && MousePicking() >= 0) {
				*selectedIDobject = MousePicking();
				alreadyPicked = true;
			}
			//else {
			//	*selectedIDobject = -1;
			//}

			break;
		case RAISE_TERRAIN:
			m_displayChunk.raiseGround(dt);
			break;
		case LOWER_TERRAIN:
			m_displayChunk.lowerGround(dt);
			break;

		case PAINT_TERRAIN:
			m_displayChunk.paintGround(dt, paintType);
			break;
		
		case FLATTEN_TERRAIN:
			m_displayChunk.levelGround(dt);
			break;

		default:
			break;
		}
		
	}
	else {alreadyPicked = false;}


	lastPos[0] = m_InputCommands.MouseXY[0];
	lastPos[1] = m_InputCommands.MouseXY[1];


}



// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	//TODO  any more complex than this, and the camera should be abstracted out to somewhere else
	//camera motion is on a plane, so kill the 7 component of the look direction
	
	float dt = timer.GetElapsedSeconds();
	timeIt += dt;
    

	//handle cam focus - could be made as a func + pointer in cam class in future work
	if (Camera_.focusingCamera) {

		//pointer error fix
		if (*selectedIDobject >= 0) {

			Camera_.UpdateCameraFocus((m_displayList[*selectedIDobject].m_position), m_displayList[*selectedIDobject].m_scale.x, dt);
		}
		else {
			Camera_.focusingCamera = false;
			Camera_.arcBallCam = false;
		}

	}

	handleInput(timer.GetElapsedSeconds());



	


	//apply camera vectors
    m_view = Matrix::CreateLookAt(Camera_.m_camPosition, Camera_.m_camLookAt, Vector3::UnitY);

    m_batchEffect->SetView(m_view);
    m_batchEffect->SetWorld(Matrix::Identity);


	m_displayChunk.m_terrainEffect->SetView(m_view);
	m_displayChunk.m_terrainEffect->SetWorld(Matrix::Identity);


#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

   
}
#pragma endregion

void Game::drawNormals()
{


	auto context = m_deviceResources->GetD3DDeviceContext();
	m_batchEffect->Apply(context);
	context->IASetInputLayout(m_batchInputLayout.Get());
	//render circle 
	m_batch->Begin();
	
	//provided function
		for (size_t i = 0; i < TERRAINRESOLUTION; i++)
		{
			for (size_t j = 0; j < TERRAINRESOLUTION; j++)
			{


				DirectX::XMFLOAT3 ps_ = m_displayChunk.m_terrainGeometry[i][j].position;
				DirectX::XMFLOAT3 nr_ = m_displayChunk.m_terrainGeometry[i][j].normal;

				XMVECTORF32 v1_pos1_ = { ps_.x,ps_.y,ps_.z };
				XMVECTORF32 v1_pos2_ = { ps_.x + 3 * nr_.x,ps_.y + 3 * nr_.y,ps_.z + 3 * nr_.z };
				XMVECTORF32 NormalColor = { nr_.x,nr_.y, nr_.z,1.0f };
				VertexPositionColor v1_(v1_pos1_, NormalColor);
				VertexPositionColor v2_(v1_pos2_, NormalColor);

				m_batch->DrawLine(v1_, v2_);
			}
		}
	


	//show triangle normal
	if (true) {
		Vector3 PIP = m_displayChunk.planeIntersectPoint;
		Vector3 PIN = m_displayChunk.planeIntersectPointNormal;

		XMVECTORF32 NormalC = { m_displayChunk.planeIntersectPointNormal.x,m_displayChunk.planeIntersectPointNormal.y, m_displayChunk.planeIntersectPointNormal.z,1.0f };
		VertexPositionColor INL1(XMVECTORF32{ PIP.x,PIP.y,PIP.z }, NormalC);
		VertexPositionColor INL2(XMVECTORF32{ PIP.x + PIN.x * 5,PIP.y + PIN.y * 5,PIP.z + PIN.z * 5 }, NormalC);

		m_batch->DrawLine(INL1, INL2);
	}
	m_batch->End();

}

void Game::drawCircleOnTerrain(float radius) {


	if (MyAction != FLATTEN_TERRAIN && MyAction != RAISE_TERRAIN && MyAction != PAINT_TERRAIN && MyAction != LOWER_TERRAIN) {
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
	m_batchEffect->Apply(context);
	context->IASetInputLayout(m_batchInputLayout.Get());
	//render circle 
	m_batch->Begin();

	Vector3 pI = m_displayChunk.planeIntersectPoint;

	XMVECTORF32 LinePosDown = { pI.x,pI.y,pI.z};
	XMVECTORF32 LinePosUp = { pI.x,pI.y + 5 + sin(timeIt*2),pI.z};

	VertexPositionColor v1_upper(XMVectorAdd(LinePosDown, g_XMZero), Colors::White);
	VertexPositionColor v2_upper(XMVectorAdd(LinePosUp, g_XMZero), Colors::Red);
	m_batch->DrawLine(v1_upper, v2_upper);



	int vertexCount = 24;
	double segRotationAngle = (360.0 / vertexCount) * (3.14159265 / 180);

	Vector2 center_ = Vector2(pI.x, pI.z);


	// Set the starting point for the initial generated vertex. We'll be rotating this point around the origin in the loop
	Vector2 start_ = Vector2(0.0 - radius, 0.0);


	for (int i = 1; i < vertexCount + 1; i++)
	{
		// Calculate the angle to rotate the starting point around the origin
		double finalSegRotationAngle = (i * segRotationAngle);

		Vector3 resultPos = Vector3(0, 0, 0);
		Vector3 resultPos_next = Vector3(0, 0, 0);

		// Rotate the start point around the origin (0, 0) by the finalSegRotationAngle (see https://en.wikipedia.org/wiki/Rotation_(mathematics) section on two dimensional rotation)
		resultPos.x = cos(finalSegRotationAngle) * start_.x - sin(finalSegRotationAngle) * start_.y;
		resultPos.z = cos(finalSegRotationAngle) * start_.y + sin(finalSegRotationAngle) * start_.x;

		// Rotate the start point around the origin (0, 0) by the finalSegRotationAngle (see https://en.wikipedia.org/wiki/Rotation_(mathematics) section on two dimensional rotation)
		resultPos_next.x = cos(finalSegRotationAngle + segRotationAngle) * start_.x - sin(finalSegRotationAngle + segRotationAngle) * start_.y;
		resultPos_next.z = cos(finalSegRotationAngle + segRotationAngle) * start_.y + sin(finalSegRotationAngle + segRotationAngle) * start_.x;

		// Set the point relative to our defined center (in this case the center of the screen)
		resultPos.x += center_.x;
		resultPos.z += center_.y;
		resultPos.y = 0.5f + m_displayChunk.getYatPos(resultPos);

		// Set the point relative to our defined center (in this case the center of the screen)
		resultPos_next.x += center_.x;
		resultPos_next.z += center_.y;
		resultPos_next.y = 0.5f + m_displayChunk.getYatPos(resultPos_next);

		


		XMVECTORF32 v1_pos1 = { resultPos.x,resultPos.y,resultPos.z };
		XMVECTORF32 v1_pos2 = { resultPos_next.x,resultPos_next.y,resultPos_next.z };


		VertexPositionColor v1(XMVectorAdd(v1_pos1, g_XMZero), Colors::Red);
		VertexPositionColor v2(XMVectorAdd(v1_pos2, g_XMZero), Colors::White);

		m_batch->DrawLine(v1, v2);
	}


	m_batch->End();
	////END
}


void Game::drawUIText() {
	//CAMERA POSITION ON HUD
	m_sprites->Begin();
	WCHAR   Buffer[256];


	//std::wstring var = L"Cam X: " + std::to_wstring(Camera_.m_camOrientation.x) + L"Cam Y: " + std::to_wstring(Camera_.m_camOrientation.y) + L"Cam Z: " + std::to_wstring(Camera_.m_camOrientation.z);
	//m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(0, 100), Colors::Yellow);
	//
	//std::wstring var2 = L"Mouse X: " + std::to_wstring(m_InputCommands.MouseXY[0]) + L"Mouse Y: " + std::to_wstring(m_InputCommands.MouseXY[1]);
	//m_font->DrawString(m_sprites.get(), var2.c_str(), XMFLOAT2(100, 100), Colors::Yellow);

	std::wstring var3;
	switch (MyAction)
	{
	case RAISE_TERRAIN:
		var3 = L"Selected Action: Raise Terrain";
		break;
	case LOWER_TERRAIN:
		var3 = L"Selected Action: Lower Terrain";
		break;
	case FLATTEN_TERRAIN:
		var3 = L"Selected Action: Flatten Terrain";
		break;
	case PAINT_TERRAIN:
		var3 = L"Selected Action: Paint Terrain";
		break;
	case SELECT_OBJECT:
		var3 = L"Selected Action: Object Picking";
		break;
	case MOVE_OBJECT:
		var3 = L"Selected Action: Move Object";
		break;
	case SCALE_OBJECT:
		var3 = L"Selected Action: Scale Object";
		break;
	case ROTATE_OBJECT:
		var3 = L"Selected Action: Rotate Object";
		break;
	default:
		break;
	}

	if (Camera_.arcBallCam) {
		var3 += L", ARCBALL Camera ON";
	}
	m_font->DrawString(m_sprites.get(), var3.c_str(), XMFLOAT2(0,0), Colors::White);
   
	m_sprites->End();
}

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

	if (m_grid)
	{
		// Draw procedurally generated dynamic grid
		const XMVECTORF32 xaxis = { 512.f, 0.f, 0.f };
		const XMVECTORF32 yaxis = { 0.f, 0.f, 512.f };
		DrawGrid(g_XMZero, 10.0f, 500.0f);
	}


	if (shouldShowNormals) {
		drawNormals();
	}




	//RENDER OBJECTS FROM SCENEGRAPH
	int numRenderObjects = m_displayList.size();
	for (int i = 0; i < numRenderObjects; i++)
	{
		m_deviceResources->PIXBeginEvent(L"Draw model");
		const XMVECTORF32 scale = { m_displayList[i].m_scale.x, m_displayList[i].m_scale.y, m_displayList[i].m_scale.z };
		
		

		const XMVECTORF32 translate = { m_displayList[i].m_position.x, m_displayList[i].m_position.y, m_displayList[i].m_position.z };

		

		//convert degrees into radians for rotation matrix
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i].m_orientation.y *3.1415 / 180,
															m_displayList[i].m_orientation.x *3.1415 / 180,
															m_displayList[i].m_orientation.z *3.1415 / 180);

		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);


		//SELECTED OBJECT CODE
		m_displayList[i].m_model->UpdateEffects([&](IEffect* effect)
		{
				auto fog = dynamic_cast<IEffectFog*>(effect);
				if (fog)
				{

					if (*selectedIDobject == i) {
						fog->SetFogEnabled(true);
					}
					else {
						fog->SetFogEnabled(false);
										}
					fog->SetFogStart(0.0f); // assuming RH coordiantes
					fog->SetFogEnd(0.0f);
					fog->SetFogColor(Colors::Yellow);
				}
		});
		

		m_displayList[i].m_model->Draw(context, *m_states, local, m_view, m_projection, renderInWireframe);	//last variable in draw,  make TRUE for wireframe

		m_deviceResources->PIXEndEvent();
	}
    m_deviceResources->PIXEndEvent();

	//RENDER TERRAIN
	context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(),0);
	context->RSSetState(m_states->CullNone());

	if (renderInWireframe) {
		context->RSSetState(m_states->Wireframe());		//uncomment for wireframe
	}

	

	//Render the batch,  This is handled in the Display chunk becuase it has the potential to get complex
	m_displayChunk.RenderBatch(m_deviceResources);



	//draw the circle indicating the terrain edit brush
	drawCircleOnTerrain(m_displayChunk.terrainEditRadius - 0.1f + sin(timeIt * 2.0f) * 0.2f);



	//draw UI text
	drawUIText();



	



	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

void XM_CALLCONV Game::DrawGrid(DirectX::FXMVECTOR origin, float cellSize, float gridExtent)
{
    
	//if (gridExtent < cellSize) {
	//	return;
	//}
	
	DirectX::GXMVECTOR MainColor =  Colors::White;
	DirectX::GXMVECTOR SecondaryColor =  Colors::Gray;


	m_deviceResources->PIXBeginEvent(L"Draw grid");

    auto context = m_deviceResources->GetD3DDeviceContext();
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    context->RSSetState(m_states->CullCounterClockwise());
	//context->colo

    m_batchEffect->Apply(context);

    context->IASetInputLayout(m_batchInputLayout.Get());

    m_batch->Begin();


	for (float i = int(gridExtent / cellSize) * -cellSize; i < gridExtent; i+= cellSize)
	{

		XMVECTORF32 v1_pos1 = {i, 0, -gridExtent };
		XMVECTORF32 v1_pos2 = {i, 0, gridExtent };

		VertexPositionColor v1(XMVectorAdd(v1_pos1, origin), MainColor);
		VertexPositionColor v2(XMVectorAdd(v1_pos2, origin), MainColor);

		m_batch->DrawLine(v1, v2);


		XMVECTORF32 v1_pos1_ = { -gridExtent, 0, i };
		XMVECTORF32 v1_pos2_ = {  gridExtent, 0, i  };

		VertexPositionColor v1_(XMVectorAdd(v1_pos1_, origin), MainColor);
		VertexPositionColor v2_(XMVectorAdd(v1_pos2_, origin), MainColor);

		m_batch->DrawLine(v1_, v2_);

		//m_batch->draw

		for (int j = 1; j < 10; j ++)
		{

			XMVECTORF32 greyPosX1 = { i + j*(cellSize/10.0f), 0.0f, -gridExtent };
			XMVECTORF32 greyPosX2 = { i + j*(cellSize/10.0f), 0.0f, gridExtent };

			VertexPositionColor greyv1(XMVectorAdd(greyPosX1, origin), SecondaryColor);
			VertexPositionColor greyv2(XMVectorAdd(greyPosX2, origin), SecondaryColor);

			m_batch->DrawLine(greyv1, greyv2);


			XMVECTORF32 greyPosZ1 = {-gridExtent, 0.0f, i + j * (cellSize / 10.0f) };
			XMVECTORF32 greyPosZ2 = {gridExtent, 0.0f, i + j * (cellSize / 10.0f)};

			VertexPositionColor greyv1_(XMVectorAdd(greyPosZ1, origin), SecondaryColor);
			VertexPositionColor greyv2_(XMVectorAdd(greyPosZ2, origin), SecondaryColor);

			m_batch->DrawLine(greyv1_, greyv2_);

		}

	}




    m_batch->End();

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;
    //m_deviceResources->get
    CreateWindowSizeDependentResources();
}

void Game::BuildDisplayList(std::vector<SceneObject> * SceneGraph)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto devicecontext = m_deviceResources->GetD3DDeviceContext();

	if (!m_displayList.empty())		//is the vector empty
	{
		m_displayList.clear();		//if not, empty it
	}

	//for every item in the scenegraph
	int numObjects = SceneGraph->size();
	for (int i = 0; i < numObjects; i++)
	{
		
		//create a temp display object that we will populate then append to the display list.
		DisplayObject newDisplayObject;
		
		//load model
		std::wstring modelwstr = StringToWCHART(SceneGraph->at(i).model_path);							//convect string to Wchar
		newDisplayObject.m_model = Model::CreateFromCMO(device, modelwstr.c_str(), *m_fxFactory, true);	//get DXSDK to load model "False" for LH coordinate system (maya)

		//Load Texture
		std::wstring texturewstr = StringToWCHART(SceneGraph->at(i).tex_diffuse_path);								//convect string to Wchar
		HRESULT rs;
		rs = CreateDDSTextureFromFile(device, texturewstr.c_str(), nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource

		//if texture fails.  load error default
		if (rs)
		{
			CreateDDSTextureFromFile(device, L"database/data/Error.dds", nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource
		}

		//apply new texture to models effect
		newDisplayObject.m_model->UpdateEffects([&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{	
			auto lights = dynamic_cast<BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(newDisplayObject.m_texture_diffuse);			
			}
		});

		//set position
		newDisplayObject.m_position.x = SceneGraph->at(i).posX;
		newDisplayObject.m_position.y = SceneGraph->at(i).posY;
		newDisplayObject.m_position.z = SceneGraph->at(i).posZ;
		
		//setorientation
		newDisplayObject.m_orientation.x = SceneGraph->at(i).rotX;
		newDisplayObject.m_orientation.y = SceneGraph->at(i).rotY;
		newDisplayObject.m_orientation.z = SceneGraph->at(i).rotZ;

		//set scale
		newDisplayObject.m_scale.x = SceneGraph->at(i).scaX;
		newDisplayObject.m_scale.y = SceneGraph->at(i).scaY;
		newDisplayObject.m_scale.z = SceneGraph->at(i).scaZ;

		//set wireframe / render flags
		newDisplayObject.m_render		= SceneGraph->at(i).editor_render;
		newDisplayObject.m_wireframe	= SceneGraph->at(i).editor_wireframe;

		newDisplayObject.m_light_type		= SceneGraph->at(i).light_type;
		newDisplayObject.m_light_diffuse_r	= SceneGraph->at(i).light_diffuse_r;
		newDisplayObject.m_light_diffuse_g	= SceneGraph->at(i).light_diffuse_g;
		newDisplayObject.m_light_diffuse_b	= SceneGraph->at(i).light_diffuse_b;
		newDisplayObject.m_light_specular_r = SceneGraph->at(i).light_specular_r;
		newDisplayObject.m_light_specular_g = SceneGraph->at(i).light_specular_g;
		newDisplayObject.m_light_specular_b = SceneGraph->at(i).light_specular_b;
		newDisplayObject.m_light_spot_cutoff = SceneGraph->at(i).light_spot_cutoff;
		newDisplayObject.m_light_constant	= SceneGraph->at(i).light_constant;
		newDisplayObject.m_light_linear		= SceneGraph->at(i).light_linear;
		newDisplayObject.m_light_quadratic	= SceneGraph->at(i).light_quadratic;
		
		m_displayList.push_back(newDisplayObject);
		
	}
		
		
		
}

void Game::BuildDisplayChunk(ChunkObject * SceneChunk)
{
	//populate our local DISPLAYCHUNK with all the chunk info we need from the object stored in toolmain
	//which, to be honest, is almost all of it. Its mostly rendering related info so...
	m_displayChunk.PopulateChunkData(SceneChunk);		//migrate chunk data
	m_displayChunk.LoadHeightMap(m_deviceResources);
	m_displayChunk.m_terrainEffect->SetProjection(m_projection);
	m_displayChunk.InitialiseBatch();


}

void Game::SaveDisplayChunk(ChunkObject * SceneChunk)
{
	m_displayChunk.SaveHeightMap();			//save heightmap to file.
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif


#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);

    m_fxFactory = std::make_unique<EffectFactory>(device);
	m_fxFactory->SetDirectory(L"database/data/"); //fx Factory will look in the database directory
	m_fxFactory->SetSharing(false);	//we must set this to false otherwise it will share effects based on the initial tex loaded (When the model loads) rather than what we will change them to.

    m_sprites = std::make_unique<SpriteBatch>(context);

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

    m_batchEffect = std::make_unique<BasicEffect>(device);
    m_batchEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(
            device->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_batchInputLayout.ReleaseAndGetAddressOf())
        );
    }

    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");

//    m_shape = GeometricPrimitive::CreateTeapot(context, 4.f, 8);

    // SDKMESH has to use clockwise winding with right-handed coordinates, so textures are flipped in U
    m_model = Model::CreateFromSDKMESH(device, L"tiny.sdkmesh", *m_fxFactory);
	

    // Load textures
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr, m_texture1.ReleaseAndGetAddressOf())
    );

    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"windowslogo.dds", nullptr, m_texture2.ReleaseAndGetAddressOf())
    );

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        1000.0f
    );

    m_batchEffect->SetProjection(m_projection);
	
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_batch.reset();
    m_batchEffect.reset();
    m_font.reset();
    m_shape.reset();
    m_model.reset();
    m_texture1.Reset();
    m_texture2.Reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

std::wstring StringToWCHART(std::string s)
{

	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}



DirectX::SimpleMath::Vector3 Game::getClickingVector() {

	DirectX::SimpleMath::Vector3 ret_;

	const XMVECTOR nearSource = XMVectorSet(m_InputCommands.MouseXY[0], m_InputCommands.MouseXY[1], 0.0f, 1.0f);
	const XMVECTOR farSource = XMVectorSet(m_InputCommands.MouseXY[0], m_InputCommands.MouseXY[1], 1.0f, 1.0f);


	//Unproject the points on the near and far plane, with respect to the matrix we just created.
	XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, m_world);
	XMVECTOR farPoint = XMVector3Unproject(farSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, m_world);

	//turn the transformed points into our picking vector. 
	XMVECTOR pickingVector = farPoint - nearPoint;
	pickingVector = XMVector3Normalize(pickingVector);
	
	ret_.x = XMVectorGetX(pickingVector);
	ret_.y = XMVectorGetY(pickingVector);
	ret_.z = XMVectorGetZ(pickingVector);
	
	
	return ret_;

}


int Game::MousePicking() {

	int selectedID = -1;
	float pickedDistance = 0;

	float closestDistance = 1000000000;

	//setup near and far planes of frustum with mouse X and mouse y passed down from Toolmain. 
	//they may look the same but note, the difference in Z
	const XMVECTOR nearSource = XMVectorSet(m_InputCommands.MouseXY[0], m_InputCommands.MouseXY[1], 0.0f, 1.0f);
	const XMVECTOR farSource = XMVectorSet(m_InputCommands.MouseXY[0], m_InputCommands.MouseXY[1], 1.0f, 1.0f);

	//Loop through entire display list of objects and pick with each in turn. 
	for (int i = 0; i < m_displayList.size(); i++)
	{
		//Get the scale factor and translation of the object
		const XMVECTORF32 scale = { m_displayList[i].m_scale.x,		m_displayList[i].m_scale.y,		m_displayList[i].m_scale.z };
		const XMVECTORF32 translate = { m_displayList[i].m_position.x,	m_displayList[i].m_position.y,	m_displayList[i].m_position.z };
		
		//convert euler angles into a quaternion for the rotation of the objecft
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i].m_orientation.y * 3.1415 / 180,
			m_displayList[i].m_orientation.x * 3.1415 / 180,
			m_displayList[i].m_orientation.z * 3.1415 / 180);

		//create set the matrix of the selected object in the world based on the translation, scale and rotation.
		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);

		//Unproject the points on the near and far plane, with respect to the matrix we just created.
		XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);
		XMVECTOR farPoint = XMVector3Unproject(farSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);

		//turn the transformed points into our picking vector. 
		XMVECTOR pickingVector = farPoint - nearPoint;
		pickingVector = XMVector3Normalize(pickingVector);

		//loop through mesh list for object
		for (int y = 0; y < m_displayList[i].m_model.get()->meshes.size(); y++)
		{
			//checking for ray intersection
			if (m_displayList[i].m_model.get()->meshes[y]->boundingBox.Intersects(nearPoint, pickingVector, pickedDistance))
			{

				if (closestDistance > pickedDistance) {
					closestDistance = pickedDistance;
					selectedID = i;
				}

			}
		}
	}


	//if we got a hit.  return it.  
	return selectedID;
}
