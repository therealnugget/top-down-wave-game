#pragma once
template<typename T>
struct type_id {
private:
	static constexpr int id = 0;
public:
	static constexpr const void* value = &type_id<T>::id;
};
template<typename T>
constexpr const void* type_id_v = type_id<T>::value;