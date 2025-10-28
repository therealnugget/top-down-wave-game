#pragma once
#include "usefulTypedefs.hpp"
#include "multicast delegates.hpp"
//define this for error checking and debug functions
#define DEBUG_BUILD
#include <iostream>
#include <SDL.h>
#include <tuple>
#include <string>
#include <vector>
#include <source_location>
#include <unordered_map>
template<typename T>
struct Vector2;
typedef Vector2<float> FVector2;
class Behaviour {
private:
	rbList* rbNode;
	void Update();
protected:
	Behaviour(CollisionCallback);
	~Behaviour();
};
using namespace std;
template<typename T>
struct Node {
private:
	//c# doesn't know that declaring multiple ptrs with multiple asterisks on the same line is objectively better
	Node* next, * prev;
public:
	Node(T& val) : value(val), next(nullptr), prev(nullptr) {}
	Node() {}
	inline Node* GetNext() {
		return next;
	}
	inline Node* GetPrev() {
		return prev;
	}
	T value;
	static void RemoveAllNodes(Node** head, void (*)(T*), bool freeNodes = true);
	static void RemoveAllNodes(Node** head, bool freeNodes = true);
	static Node* &AddAtHead(T&, Node** head);
	static Node* &AddAtHeadEmpty(Node** head, Node*);
	static Node* &AddAtHead(T&, Node** head, Node *preConstructed);
	//the ptr returned is only useful if you don't delete the node. 
	static Node *Remove(Node** head, Node* remove, bool freeNode = true);
	static Node *RemoveHead(Node* head, bool freeNode = true);
	static Node *RemoveTail(Node** tail, Node* remove, bool freeNode = true);
	static void Reverse(Node** head);
	//needs to be static because we can't assign the instance of the class containing the current function to a value, so either way we have to pass in a ptr to the ptr to the node.
	static inline void Advance(Node<T>** node) {
		*node = (*node)->next;
	}
	static inline void Disadvance(Node<T>** node) {
		*node = (*node)->prev;
	}
	inline void Iterate(std::function<void(Node<T>*)> del) {
		Node* head = this;
		while (head) {
			del(head);
			Node::Advance(&head);
		}
	}
	//O(n). a wrapper for a linked list from an initializer list.
	static inline Node*& FromInitializer(std::initializer_list<T> vec) {
		Node* _this = nullptr;
		for (auto element: vec) {
			Node::AddAtHead(element, &_this);
		}
		return _this;
	}
	//O(n)
	inline int Length() {
		int _this = 0;
		Node<T> *curNode = this;
		while (curNode) {
			_this++;
			Node<T>::Advance(&curNode);
		}
		return _this;
	}
};
template<typename T>
Node<T>*& Node<T>::AddAtHeadEmpty(Node<T>** head, Node<T> *preconstructedNode) {
	preconstructedNode->prev = nullptr;
	preconstructedNode->next = *head;
	if (*head) (*head)->prev = preconstructedNode;
	*head = preconstructedNode;
	return preconstructedNode;
}
template <typename T>
Node<T>* &Node<T>::AddAtHead(T &val, Node<T>** head) {
	Node<T>* _this = new Node<T>();
	return AddAtHead(val, head, _this);
}
template <typename T>
Node<T>* &Node<T>::AddAtHead(T &val, Node<T>** head, Node<T> *preConstructedNode) {
	preConstructedNode->value = val;
	preConstructedNode->prev = nullptr;
	preConstructedNode->next = *head;
	if (*head) (*head)->prev = preConstructedNode;
	*head = preConstructedNode;
	return preConstructedNode;
}
template<typename T>
class EmptyStack {
private:
	Node<T>* head;
public:
	EmptyStack(): head(nullptr) {}
	inline Node<T>* GetHead() {
		return head;
	}
	Node<T> *PushNode(Node<T> *);
	Node<T> *PopNode();
	void Flush(bool freeNodes = true);
	inline bool const IsEmpty() {
		return head == nullptr;
	}
};
template<typename T>
void EmptyStack<T>::Flush(bool freeNodes) {
	Node<T>::RemoveAllNodes(&head, freeNodes);
}
template<typename T>
Node<T>* EmptyStack<T>::PushNode(Node<T>* preconstructedNode) {
	return Node<T>::AddAtHeadEmpty(&head, preconstructedNode);
}
template <typename T>
Node<T> *EmptyStack<T>::PopNode() {
	auto oldHead = head;
	Node<T>::Advance(&head);
	return Node<T>::RemoveHead(oldHead, false);
}
template<typename T>
class Stack {
private:
	Node<T>* head;
public:
	Stack(): head(nullptr) {}
	inline Node<T>* GetHead() {
		return head;
	}
	Node<T>* Push(T val);
	Node<T>* Push(T val, Node<T> *);
	T &Pop();
	Node<T> *PopNode();
	void Flush(bool freeNodes = true);
	inline bool const IsEmpty() {
		return head == nullptr;
	}
};
template<typename T>
void Stack<T>::Flush(bool freeNodes) {
	Node<T>::RemoveAllNodes(&head, freeNodes);
}
template<typename T>
Node<T>* Stack<T>::Push(T val) {
	return Node<T>::AddAtHead(val, &head);
}
template<typename T>
Node<T>* Stack<T>::Push(T val, Node<T> *preconstructedNode) {
	return Node<T>::AddAtHead(val, &head, preconstructedNode);
}
template <typename T>
T &Stack<T>::Pop() {
	auto oldHead = head;
	Node<T>::Advance(&head);
	return Node<T>::RemoveHead(oldHead, false)->value;
}
template<typename T>
class QueueNode {
private:
	Node<T> *head;
	Node<T> *tail;
public:
	QueueNode(): head(nullptr), tail(nullptr) {}
	//push to head
	Node<T> *Push(T);
	//remove from tail
	Node<T> *PopTail();
	inline T Front() {
		return head->value;
	}
	inline bool IsEmpty() {
		return !static_cast<bool>(head);
	}
	void Iterate(void (*)(Node<T> *));
};
template<typename T>
void QueueNode<T>::Iterate(void (*del)(Node<T>*)) {
	Node<T>* curNode = head;
	while (curNode) {
		del(curNode);
		Node<T>::Advance(&curNode);
	}
}
template<typename T>
Node<T> *QueueNode<T>::Push(T val) {
	Node<T>::AddAtHead(val, &head);
	if (!head->GetPrev()) tail = head;
	return head;
}
template<typename T>
Node<T> *QueueNode<T>::PopTail() {
	return Node<T>::RemoveTail(&tail, tail);
}
template<typename T>
struct Branch {
public:
	Branch(T _value) : value(_value), left(nullptr), right(nullptr) {}
	T value;
	Branch* left, * right;
	static Branch<T> *AddBranch(T val, Branch<T> *root);
	Branch<T> *AddBranch(T val);
#define TraversalArgs void (*traversalDel)(Branch<T>*), Branch<T> *root
	//depth first: (good for tall and short trees. use on tees sorted smallest left largest right.)
	static void InOrder(TraversalArgs);
	static void PreOrder(TraversalArgs);
	static void PostOrder(TraversalArgs);
	//breadth first (good for wide and narrow trees. use on trees sorted like https://www.geeksforgeeks.org/level-order-tree-traversal/.). func recursive. time complexity O(n), auxliary space O(1) if the recursion stack is considered the space is O(n).
	static void LevelOrder(TraversalArgs, int level);
	static void StartLevelOrder(TraversalArgs);
	static uint Height(Branch<T> *);
	//time complexity O(n), auxilary space O(n).
	static void LevelOrderQueue(TraversalArgs);
//private:
};
void ThrowError(const char* initErrMessage);
static inline void ThrowBranchNotAssigned() {
	ThrowError("could not find free branch");
}
//templates are just more error-checked definitions more or less. this func adds smaller vals to l and greater to r. TODO: make overload that accepts value by reference.
template<typename T>
Branch<T> *Branch<T>::AddBranch(T val, Branch<T> *root) {
	if (!root) {
		return new Branch<T>(val);
	}
	if (val > root->value) {
		root->right = AddBranch(val, root->right);
		return root;
	}
	root->left = AddBranch(val, root->left);
	return root;
}
template<typename T>
Branch<T> *Branch<T>::AddBranch(T val) {
	return AddBranch(val, this);
}
template<typename T>
void Branch<T>::InOrder(void (*del)(Branch<T>*), Branch<T>* root) {
	if (!root) return;
	InOrder(del, root->left);
	del(root);
	InOrder(del, root->right);
}
template<typename T>
void Branch<T>::PreOrder(void (*del)(Branch<T>*), Branch<T>* root) {
	if (!root) return;
	del(root);
	InOrder(del, root->left);
	InOrder(del, root->right);
}
template<typename T>
void Branch<T>::PostOrder(void (*del)(Branch<T>*), Branch<T>* root) {
	if (!root) return;
	InOrder(del, root->left);
	InOrder(del, root->right);
	del(root);
}
template<typename T>
void Branch<T>::LevelOrder(void (*del)(Branch<T>*), Branch<T>* root, int level) {
	if (!root) return;
	if (level == 1) {
		del(root);
		return;
	}
	if (level < 1) return;
	int levelTakeOne = level - 1;
	LevelOrder(del, root->left, levelTakeOne);
	LevelOrder(del, root->right, levelTakeOne);

}
template<typename T>
void Branch<T>::StartLevelOrder(void (*traversalDel)(Branch<T>*), Branch<T>* root) {
	uint height = Height(root);
	for (uint i = 1; i <= height; i++) {
		LevelOrder(traversalDel, root, i);
	}
}
template<typename T>
uint Branch<T>::Height(Branch<T>* root) {
	if (!root) return 0;
	uint lHeight = Height(root->left), rHeight = Height(root->right);
	if (lHeight > rHeight) return lHeight + 1;
	return rHeight + 1;
}
template<typename T>
void Branch<T>::LevelOrderQueue(void (*traversalDel)(Branch<T>*), Branch<T>* root) {
	//the braces in single-line if statements are common convention, due to multiple-statement-one-line definitions.
	if (!root) {
		return;
	}
	QueueNode<Branch<T> *> queue = QueueNode<Branch<T> *>();
	queue.Push(root);
	while (!queue.Empty()) {
		Branch<T>* branch = queue.Front();
		traversalDel(branch);
		queue.Pop();
		if (branch->left) {
			queue.Push(branch->left);
		}
		if (!branch->right) continue;
		queue.Push(branch->right);
	}
}
typedef Vector2<int> IntVec2;
typedef struct AABB;
template<typename T>
class MultiDelegate;
template<>
class MultiDelegate<void>;
template<typename T>
void MultiDelegate<T>::operator ()(T param) {
	for (auto& del : delegates) {
		del(param);
	}
}
class Main {
public:
	//whether the key was pressed this frame regardless of it's state last frame
	static inline bool GetKey(int key) {
		return pressingKey[key];
	}
	//whether the key was pressed this frame but not last frame
	static inline bool KeyPressed(int key) {
#define KeyBP GetKey(key)
		return KeyBP && !PressedKeyLastFrm(key);
	}
	//whether the key was pressed last frame but not this frame
	static inline bool KeyUp(int key) {
		return pressedKey[key] && !pressingKey[key];
	}
	//whether the key was pressed last frame AND this frame
	static inline bool KeyHeld(int key) {
		return KeyBP && PressedKeyLastFrm(key);
	}
	//order unimportant.
	static enum direction {
		down = 0,
		right = 1,
		left = 2,
		up = 3,
		num_directions = 4,
	};
	static std::vector<const char *> dirPaths;
	static enum Tag {
		player = 0,
		debug = 1,
	};
	static FVector2 fInputVec;
	static FVector2 fInputVec2;
	static IntVec2 iInputVec;
	static inline bool GetModKey(int key) {
		return pressingModKey[key];
	}
	inline static bool PressedKeyLastFrm(int key) {
		return pressedKey[key];
	}
	inline static bool PressedModKeyLastFrm(int key) {
		return pressedModKey[key];
	}
#define ModKeyBP GetModKey(key)
	static inline bool ModKeyPressed(int key) {
		return ModKeyBP && !PressedModKeyLastFrm(key);
	}
	static inline bool ModKeyHeld(int key) {
		return ModKeyBP && PressedModKeyLastFrm(key);
	}
	static SDL_Renderer* renderer;
	static SDL_DisplayMode DM;//not direct messages XD
	//makes more sense to have this as an fvector for fvector size initializations.
	static FVector2 DisplaySize;
	static FVector2 halfDisplaySize;
	static float DeltaTime();
	static float timeScale;
	static Uint64 pastTime;
	inline static void StartDTCounter() {
		pastTime = SDL_GetPerformanceCounter();
	}
	static bool CheckPauseState();
	static void RegisterInput();
	static void ClearInput();
	static void EarlyUpdate();
	static MultiDelegate<float> dtUpdates;
	static MultiDelegate<void> Updates;
	static void LateUpdate();
	static void Start();
	static void Finalize();
	static FVector3 renderDrawColor;
	template<typename T>
	static inline void Swap(T& a, T& b) {
		T t = a;
		a = b;
		b = t;
	}
	static inline void OneMemory(void* dst, size_t size) {
		memset(dst, 1, size);
	}
	template <typename First, typename ...Rest, typename FirstAssign, typename ...RestAssign>
	static void AssignTuple(First&, Rest &..., FirstAssign&, RestAssign &...);
	template <typename First, typename Rest, typename FirstAssign, typename RestAssign>
	static void AssignTuple(First&, Rest&, FirstAssign&, RestAssign&);
	template <typename ToAssign, typename AssignTo>
	inline static void AssignTuple(ToAssign& toAss, AssignTo& assTo) {
		toAss = assTo;
	}
	inline static char* BoolToStr(bool b) {
		return reinterpret_cast<char*>(reinterpret_cast<uintptr_t>("true") * b + reinterpret_cast<uintptr_t>("false") * !b);
	}
#ifdef DEBUG_BUILD
	inline static void PrintInt(int i, const char *name) {
		cout << name << " is " << i << '\n';
	}
#endif
	inline static char* IntToStr(int i, ushort maxIntChar = 100) {
		char* _this = new char[maxIntChar];
		sprintf(_this, "%d", i);
		return _this;
	}
	inline static char* IntToStr(size_t i, ushort maxIntChar = 100) {
		char* _this = new char[maxIntChar];
		sprintf(_this, "%llu", i);
		return _this;
	}
	inline static char* IntToStr(uint i, ushort maxIntChar = 100) {
		char* _this = new char[maxIntChar];
		sprintf(_this, "%llu", i);
		return _this;
	}
	static inline void AssignIfLess(float& check, float& assignConditionally) {
		if (check >= assignConditionally) return;
		assignConditionally = check;
	}
	static inline void AssignIfMore(float& check, float& assignConditionally) {
		if (check <= assignConditionally) return;
		assignConditionally = check;
	}
	//it is NOT the case that both x and y have to be bigger for "assignConditionally" to be assigned. either-or per-component.
	static void AssignIfMore(Vector2<float>& check, Vector2<float>& assignConditionally);
	//it is NOT the case that both x and y have to be smaller for "assignConditionally" to be assigned. either-or per-component.
	static void AssignIfLess(Vector2<float>& check, Vector2<float>& assignConditionally);
	//inclusive max, min. WILL NOT swap args when they are the wrong way around in non-debug builds (i.e., when the DEBUG_BUILD macro isn't defined)
	static inline int GetRandInt(int min, int max) {
		if (max < min) {
			int temp = std::move(min);
			min = std::move(max);
			max = std::move(temp);
		}
		return static_cast<int>((static_cast<double>(rand()) / static_cast<double>(RAND_MAX)) * (max - min) + min);
	}
	//inclusive max, min. WILL NOT swap args when they are the wrong way around in non-debug builds (i.e., when the DEBUG_BUILD macro isn't defined)
	static inline float GetRandFloat(float min, float max) {
#ifdef DEBUG_BUILD
		if (max < min) {
			float temp = std::move(min);
			min = std::move(max);
			max = std::move(temp);
		}
#endif
		return static_cast<float>((static_cast<double>(rand()) / static_cast<double>(RAND_MAX)) * (max - min) + min);
	}
	//inclusive max, min. WILL NOT swap args when they are the wrong way around in non-debug builds (i.e., when the DEBUG_BUILD macro isn't defined)
	static FVector2 GetRandFVec(FVector2 min, FVector2 max);
	template<typename T>
	static inline std::initializer_list<T> VecToInitList(std::vector<T>& vec) {
		return std::initializer_list<T>(vec._Unchecked_begin(), vec._Unchecked_end());
	}
	static inline int GetAnimOffset(int animation) {
		return Main::num_directions * animation;
	}
	static const std::string empty_string;
	static const char * const empty_cc;
private:
	//order important for bit-wise operations.
	static enum inpDirection: int {
		input_down = 0, //0000
		input_first = input_down,
		input_up = 1,   //0001
		input_right = 2,//0010
		input_left = 3, //0011
		input_last = input_left,
		num_inp_dirs = 4,
	};
	//whether the key in the direction is being pressed this frame but not last frame
	static bool dirKeyPress[num_inp_dirs];
	//whether the key in the direction is being pressed this frame
	static bool getDirKey[num_inp_dirs];
	static bool dirKeyUp[num_inp_dirs];
	static bool processedKey[num_inp_dirs];
	static bool cancelKey[num_inp_dirs];
	//the key that opposed "cancelKey" in a specific direction to make it cancel
	static int cancelOpKey[num_inp_dirs];
	//whether the key on the axis is being pressed this frame
	static bool keyDownHorizon, keyDownVert;
	//whether the axis is cancelled (i.e. vertical axis is cancelled by any horizontal input, vertical axis is cancelled by any horizontal input.)
	static bool cancelH, cancelV;
	static Node<int>* setPressed;
	static bool pressingKey[];//whether the key was pressed this frame
	static bool pressedKey[];//^^ that of last frame
	static bool pressingModKey[];
	static bool pressedModKey[];
	static void SetPastKey(int*);
	inline static void AddKeyPress(int key) {
		Node<int>::AddAtHead(key, &setPressed);
	}
	static void SetCancelState(int dir);
	static void SetKeyState(int dir);
	static bool GetOpAxisVal(bool h, bool v, bool dirIsHorizon);
	static inline bool GetOpAxisVal(bool h, bool v, int dir) {
		return GetOpAxisVal(h, v, static_cast<bool>(dir & 2));
	}
	static void AssignDirKeyFromInfo(bool *assign, int dir, int key1, int key2, bool (*keyInfo)(int));
	static void SetAxisBool(bool& h, bool& v, bool dirBools[num_inp_dirs]);
	static inline void AssignDirKeyPress(int dir, int key1, int key2) {
		AssignDirKeyFromInfo(dirKeyPress, dir, key1, key2, KeyPressed);
	}
	static inline void AssignGetDirKey(int dir, int key1, int key2) {
		AssignDirKeyFromInfo(getDirKey, dir, key1, key2, GetKey);
	}
	static inline void AssignDirKeyUp(int dir, int key1, int key2) {
		AssignDirKeyFromInfo(dirKeyUp, dir, key1, key2, KeyUp);
	}
	static inline void AssignDirKey(int dir, int key1, int key2) {
		AssignDirKeyPress(dir, key1, key2);
		AssignGetDirKey(dir, key1, key2);
		AssignDirKeyUp(dir, key1, key2);
	}
	static bool fullScreen;
};
template <typename First, typename ...Rest, typename FirstAssign, typename ...RestAssign>
void Main::AssignTuple(First& first, Rest &... rest, FirstAssign& firstAss, RestAssign &... restAss) {
	first = firstAss;
	AssignTuple(rest..., restAss...);
}
template <typename First, typename Rest, typename FirstAssign, typename RestAssign>
void Main::AssignTuple(First& first, Rest& rest, FirstAssign& firstAss, RestAssign& restAss) {
	first = firstAss;
	rest = restAss;
}
template <typename T>
Node<T>* Node<T>::Remove(Node<T>** head, Node<T>* remove, bool freeNode) {
	Node<T>* next = remove->next, * prev = remove->prev;
	if (next) next->prev = prev;
	if (prev) prev->next = next;
	else {
		*head = next;
	}
	if (!freeNode) return remove;
	delete remove;
	//i'm pretty sure it returns nullptr anyway, but it's better practice to explicitly return nullptr
	return nullptr;
}
template <typename T>
Node<T>* Node<T>::RemoveHead(Node<T>* head, bool freeNode) {
	Node<T>* next = head->next, * prev = head->prev;
	if (next) next->prev = prev;
	if (prev) prev->next = next;
	if (!freeNode) return head;
	delete head;
	//i'm pretty sure it returns nullptr anyway, but it's better practice to explicitly return nullptr
	return nullptr;
}
template <typename T>
Node<T>* Node<T>::RemoveTail(Node<T>** tail, Node<T>* remove, bool freeNode) {
	Node<T>* next = remove->next, * prev = remove->prev;
	if (next) next->prev = prev;
	else {
		*tail = next;
	}
	if (prev) prev->next = next;
	if (!freeNode) return remove;
	delete remove;
	//i'm pretty sure it returns nullptr anyway, but it's better practice to explicitly return nullptr
	return nullptr;
}
template <typename T>
void Node<T>::Reverse(Node<T>** head) {
	Node<T>* curNode = *head;
	while (curNode) {
		if (!curNode->next) {
			*head = curNode;
		}
		Main::Swap(curNode->next, curNode->prev);
		Node::Disadvance(&curNode);
	}
}
template <typename T>
void Node<T>::RemoveAllNodes(Node<T>** head, void (*removeDel)(T*), bool freeNodes) {
	Node<T>* curNode = *head;
	Node<T>* pastNode;
	while (curNode) {
		removeDel(&curNode->value);
		pastNode = curNode;
		Node::Advance(&curNode);
		Node::Remove(head, pastNode, freeNodes);
	}
}
template <typename T>
void Node<T>::RemoveAllNodes(Node<T>** head, bool freeNodes) {
	Node<T>* curNode = *head;
	Node<T>* pastNode;
	while (curNode) {
		pastNode = curNode;
		Node::Advance(&curNode);
		Node::Remove(head, pastNode, freeNodes);
	}
}
/*
template <typename First, typename ...Rest, typename FirstAssign, typename ...RestAssign>
void Main::AssignTuple(First &first, Rest &... rest, FirstAssign &firstAss, RestAssign &... restAss) {
	first = firstAss;
	AssignTuple(rest..., restAss...);
}
template <typename First, typename Rest, typename FirstAssign, typename RestAssign>
void Main::AssignTuple(First &first, Rest &rest, FirstAssign &firstAss, RestAssign &restAss) {
	first = firstAss;
	rest = restAss;
}*/