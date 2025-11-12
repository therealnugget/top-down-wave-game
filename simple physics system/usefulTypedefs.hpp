#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
typedef unsigned int uint;
typedef unsigned short ushort;
struct RigidBody;
template<typename T>
struct Node;
typedef Node<Node<RigidBody*>*> rbListList;
typedef Node<RigidBody*> rbList;
typedef std::tuple<float, float, float> FVector3;
typedef std::vector<const char*> CCList;
class Collision;
typedef std::function<void(Collision&)> CollisionCallback;