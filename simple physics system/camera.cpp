#include "camera.hpp"
#include "physics.hpp"
IntVec2 Camera::cameraPosition = {};
int Camera::GetCamPosX() {
	return cameraPosition.x;
}
int Camera::GetCamPosY() {
	return cameraPosition.y;
}
IntVec2 Camera::GetCamPos() {
	return cameraPosition;
}