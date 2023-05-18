#include "pch.h"
#include "CameraMain.h"


CameraMain::CameraMain() {

	//initialize variables

	m_camRotRate = 0.1f;

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

//update camera rotations
void CameraMain::UpdateCameraRotation(int MouseDeltaX, int MouseDeltaY, float deltaTime) {


	m_camOrientation.y += deltaTime * m_camRotRate * MouseDeltaX;
	m_camOrientation.z -= deltaTime * m_camRotRate * MouseDeltaY;

	//clamp looking up and down
	if (m_camOrientation.z > 90 * 3.14f / 180) {
		m_camOrientation.z = 90 * 3.14f / 180;
	}
	if (m_camOrientation.z < -90 * 3.14f / 180) {
		m_camOrientation.z = -90 * 3.14f / 180;
	}

	//cutoff extra angles (e.g. 365 becomes 360)
	if (m_camOrientation.y > 360 * 3.14f / 180) {
		m_camOrientation.y -= 360 * 3.14f / 180;
	}					 
	if (m_camOrientation.y < -360 * 3.14f / 180) {
		m_camOrientation.y += 360 * 3.14f / 180;
	}

	
}

//update camera position
void CameraMain::UpdateCameraPosition(float Dforward, float dRight){

	//update look vector, must be here otherwise the application will crash
	m_camLookDirection.x = cos(m_camOrientation.y) * cos(m_camOrientation.z);
	m_camLookDirection.y = sin(m_camOrientation.z);
	m_camLookDirection.z = sin(m_camOrientation.y) * cos(m_camOrientation.z);
	m_camLookDirection.Normalize();

	//create right vector from look Direction
	m_camLookDirection.Cross(DirectX::SimpleMath::Vector3::UnitY, m_camRight);
	m_camRight.Normalize();

	//update position and lookat point
	m_camPosition += Dforward * m_camLookDirection + dRight * m_camRight;
	m_camLookAt = m_camPosition + m_camLookDirection;
}

//given an object and its scale (scale influences cam distance), interpolate the camera so it ends up looking at the desired (object) position
//this is used both for focus, as well as the arcbal camera
void CameraMain::UpdateCameraFocus(Vector3 objectPos, float scale, float dt)
{


	if (scale <= 0.0f) {
		return;
	}


	//define necessary variables
	float desired_distance_from_object = scale * ArcBallDistance;
	Vector3 desired_camera_location = objectPos - m_camLookDirection * desired_distance_from_object;
	Vector3 total_todo_movement_vector = (desired_camera_location - m_camPosition);
	float current_distance_from_DCL = total_todo_movement_vector.Length();
	
	float focus_speed = current_distance_from_DCL + 2.0f;
	if (focus_speed < 1.0f) { focus_speed = 1.0f; }
	
	
	float immediate_set_threshold = 0.05f;

	if (!arcBallCam) {
		//exit condition for only focusing
		focusingCamera = false;
	}
	else {
		focus_speed *= current_distance_from_DCL;
	}

	total_todo_movement_vector.Normalize();
	if (current_distance_from_DCL > immediate_set_threshold) {
		m_camPosition += dt * total_todo_movement_vector * 5.0f * focus_speed;
		focusingCamera = true;		//reset of the exit condition if the destination hasn't been reached yet

	}
	else {
		m_camPosition = desired_camera_location;
	}
}
		