#include "pch.h"
#include "CameraMain.h"


CameraMain::CameraMain() {

	m_camRotRate = 0.3f;

	//camera
	m_camPosition.x = 0.0f;
	m_camPosition.y = 3.7f;
	m_camPosition.z = -3.5f;


	m_camLookAt.x = 0.0f;
	m_camLookAt.y = 0.0f;
	m_camLookAt.z = 0.0f;

	m_camLookDirection.x = 0.0f;
	m_camLookDirection.y = 0.0f;
	m_camLookDirection.z = 0.0f;

	m_camRight.x = 0.0f;
	m_camRight.y = 0.0f;
	m_camRight.z = 0.0f;

	m_camOrientation.x = 0.0f;
	m_camOrientation.y = 0.0f;
	m_camOrientation.z = 0.0f;

}


CameraMain::~CameraMain() {



}

void CameraMain::UpdateCameraRotation(int MouseDeltaX, int MouseDeltaY, float deltaTime) {


	//m_camPosition -= m_camRight * m_movespeed;
	m_camOrientation.y += deltaTime * m_camRotRate * MouseDeltaX;
	m_camOrientation.z -= deltaTime * m_camRotRate * MouseDeltaY;

	if (m_camOrientation.z > 90 * 3.14f / 180) {
		m_camOrientation.z = 90 * 3.14f / 180;
	}
	if (m_camOrientation.z < -90 * 3.14f / 180) {
		m_camOrientation.z = -90 * 3.14f / 180;
	}



}

void CameraMain::UpdateCameraPosition(float Dforward, float dRight){


	m_camLookDirection.x = cos(m_camOrientation.y) * cos(m_camOrientation.z);
	m_camLookDirection.y = sin(m_camOrientation.z);
	m_camLookDirection.z = sin(m_camOrientation.y) * cos(m_camOrientation.z);
	
	
	m_camLookDirection.Normalize();
	
	//create right vector from look Direction
	m_camLookDirection.Cross(DirectX::SimpleMath::Vector3::UnitY, m_camRight);

	m_camPosition += Dforward * m_camLookDirection + dRight * m_camRight;
	
	////update lookat point
	m_camLookAt = m_camPosition + m_camLookDirection;


}

