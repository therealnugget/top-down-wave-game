#include "camera.hpp"
#include "physics.hpp"
#include "main.hpp"
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
IntVec2 Camera::GetCamExtentWorld(float multiplier) {
	return cameraPosition + Main::halfDisplaySize * multiplier;
}