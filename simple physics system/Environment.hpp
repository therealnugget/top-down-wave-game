#pragma once
#include "main.hpp"
#include "physics.hpp"
//static
class Environment final {
private:
	//number of trees on screen at any given time
	static constexpr int numTrees = 15;
	static constexpr FVector2 treeSize = FVector2(110.f, 110.f);
	static Node<Entity*>* treeEntNodes[numTrees];
	static Node<std::function<void(void)>> *updateNode;
	static void Update(void);
	~Environment(void);
public:
	static void Init(void);
};