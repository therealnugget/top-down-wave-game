#pragma once
template<typename T>
struct Vector2;
typedef Vector2<int> IntVec2;
//static
class Camera final{
private:
	static IntVec2 cameraPosition;
public:
	static IntVec2 GetCamPos(void);
	static IntVec2 GetCamExtentWorld(float);
	static int GetCamPosX(void);
	static int GetCamPosY(void);
	friend class Player;
};