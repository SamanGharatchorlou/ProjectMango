#pragma once

template<class T>
class ActionStack
{
public:
	bool HasAction() const { return stack.size() > 0; }

	void Push(T* state)
	{
		stack.push_back(state);
		stack.back()->Init();
	}

	void Pop()
	{
		if(stack.size() > 0)
		{
			stack.back()->Exit();
			stack.pop_back();
		}

		if(stack.size() == 0)
			int a = 4;
	}

	void Replace(T* item)
	{
		Pop();
		Push(item);
	}

	T& Top() { return *stack.back(); }
	const T& Top() const { return *stack.back(); }

	const T* Previous() const 
	{ 
		u32 size = stack.size();
		if(size > 1)
			return stack[size - 2];

		return nullptr;
	}

	std::vector<T*> stack;
};