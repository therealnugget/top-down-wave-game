#pragma once
#include "physics.hpp"
#include "usefulTypedefs.hpp"
#ifdef DEBUG_BUILD
#include <source_location>
#include <format>
#include <sstream>
#endif
//static
class Shapes {
public:
	const static std::string circlePath, trianglePath, blueSqrPath, greenSqrPath;
	typedef enum {
		circle,
		triangle,
		blueSqr,
		greenSqr,
		num_shapes,
	} TypeOfShape;
	static inline const char* GetTexture(TypeOfShape typeOfShape) {
		switch (typeOfShape) {
		case TypeOfShape::circle:
			return circlePath.c_str();
			break;
		case TypeOfShape::triangle:
			return trianglePath.c_str();
			break;
		case TypeOfShape::blueSqr:
			return blueSqrPath.c_str();
			break;
		case TypeOfShape::greenSqr:
			return greenSqrPath.c_str();
			break;
		default:
			ThrowError("shape was not of type with label defined.");
			break;
		}
	}
	static inline Node<RigidBody*>** CreateShapes(const uint numShapes, FVector2 position = Main::halfDisplaySize, IntVec2 size = IntVec2::One, float _mass = 1.f, TypeOfShape typeOfShape = TypeOfShape::blueSqr, std::initializer_list<FVector2> _centreOfRot = std::initializer_list<FVector2>(), FVector2 _centreOfRotNarrowPVert = FVector2::Zero, FVector2 renderOffset = FVector2::Zero, bool isTrigger = false, bool moveable = true) {
		Node<RigidBody*>** _this = new Node<RigidBody*> *[numShapes];
		for (int i = 0; i < numShapes; i++) {
			auto data = SubRBData();
			auto animPath = std::string("debug_square") + GetTexture(typeOfShape);
			data.animPaths = { animPath.c_str() };
			data.endPaths = { Main::empty_cc };
			data.startPos = position;
			data.size = size;
			data.mass = _mass;
			data.centreOfRot = _centreOfRot;
			data.centreOfRotNPVert = _centreOfRotNarrowPVert;
			data.isTrigger = isTrigger;
			data.moveable = moveable;
			data.renderOffset = renderOffset;
			_this[i] = Physics::SubscribeEntity(new RigidBody(data));
		}
		return _this;
	}
};