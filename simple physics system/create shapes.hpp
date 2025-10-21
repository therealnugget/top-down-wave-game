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
	typedef enum {
		square,
		circle,
		triangle,
		blueSqr,
		greenSqr,
		num_shapes,
	} TypeOfShape;
	static inline const char* GetTexture(TypeOfShape typeOfShape) {
		switch (typeOfShape) {
		case TypeOfShape::square:
			return squarePath.c_str();
			break;
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
	static inline Node<RigidBody*>* CreateShape(std::vector<FVector2> narrowPhaseVertices = Physics::DefaultSquareVerticesAsList, FVector2 position = Main::halfDisplaySize, IntVec2 size = IntVec2::One, float _mass = 1.f, TypeOfShape typeOfShape = TypeOfShape::square, std::initializer_list<FVector2> _centreOfRot = std::initializer_list<FVector2>(), FVector2 _centreOfRotNarrowPVert = FVector2::Zero, IntVec2 _renderOffset = IntVec2::Zero, bool isTrigger = false, bool moveable = true) {
		RigidBody* _this = new RigidBody(position, FVector2::Zero, .0f, _mass, narrowPhaseVertices, _centreOfRotNarrowPVert, moveable, isTrigger, true, size, Main::empty_string, {"debug",}, std::unordered_map<const char*, std::variant<FVector2, FVector2*>>(), std::unordered_map<const char*, bool>(), _centreOfRot, _renderOffset, { GetTexture(typeOfShape) });
		return Physics::SubscribeEntity(_this);
	}
	static inline Node<RigidBody*>** CreateShapes(const uint numShapes, std::vector<FVector2> narrowPhaseVertices = Physics::DefaultSquareVerticesAsList, FVector2 position = Main::halfDisplaySize, IntVec2 size = IntVec2::One, float _mass = 1.f, TypeOfShape typeOfShape = TypeOfShape::square, std::initializer_list<FVector2> _centreOfRot = std::initializer_list<FVector2>(), FVector2 _centreOfRotNarrowPVert = FVector2::Zero) {
		Node<RigidBody*>** _this = new Node<RigidBody*> *[numShapes];
		for (int i = 0; i < numShapes; i++) {
			//IMPORTANT: giving the function the same texture many times might cause issues (i say "might" because i haven't checked.) if this function doesn't work, that may be why. contrarily, if this doesn't cause issues, the same texture should be reused for everything in the scene with that texture.
			_this[i] = CreateShape(narrowPhaseVertices, position, size, _mass, typeOfShape, _centreOfRot, _centreOfRotNarrowPVert);
		}
		return _this;
	}
	const static string squarePath, squareString, circlePath, trianglePath, blueSqrPath, greenSqrPath;
};