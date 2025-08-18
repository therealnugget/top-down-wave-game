#pragma once
typedef unsigned int uint;
typedef unsigned short ushort;
struct RigidBody;
template<typename T>
struct Node;
typedef Node<Node<RigidBody*>*> rbListList;
typedef Node<RigidBody*> rbList;