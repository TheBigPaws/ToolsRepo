#pragma once
#include "DeviceResources.h"
#include "StepTimer.h"
#include "SceneObject.h"
#include "DisplayObject.h"
#include "DisplayChunk.h"
#include "ChunkObject.h"
#include "InputCommands.h"
#include <vector>


class CameraMain
{

public:

	CameraMain();
	~CameraMain();

	//------------  Update Functions
	void UpdateCameraRotation(int MouseDeltaX, int MouseDeltaY, float deltaTime);
	void UpdateCameraPosition(float Dforward, float dRight);
	void UpdateCameraFocus(Vector3 objectPos, float scale, float dt);

	//------------  Basic Attributes
	DirectX::SimpleMath::Vector3		m_camPosition;
	DirectX::SimpleMath::Vector3		m_camOrientation;
	DirectX::SimpleMath::Vector3		m_camLookAt;
	DirectX::SimpleMath::Vector3		m_camLookDirection;
	DirectX::SimpleMath::Vector3		m_camRight;
	float m_camRotRate;

	//------------  Focused Camera Mode Variables
	bool arcBallCam = false;
	bool focusingCamera = false;
	float ArcBallDistance = 5.0f;
};

