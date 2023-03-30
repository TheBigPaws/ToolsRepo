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

	void UpdateCameraRotation(int MouseDeltaX, int MouseDeltaY, float deltaTime);
	void UpdateCameraPosition(float Dforward, float dRight);


	DirectX::SimpleMath::Vector3		m_camPosition;
	DirectX::SimpleMath::Vector3		m_camOrientation;
	DirectX::SimpleMath::Vector3		m_camLookAt;
	DirectX::SimpleMath::Vector3		m_camLookDirection;
	DirectX::SimpleMath::Vector3		m_camRight;
	float m_camRotRate;


};

