#pragma once

template<class T>
class ActionStack
{
public:
	bool HasAction() const { return stack.size() > 0; }

	void Push(T* state)
	{
		if(stack.size() > 0)
		{
			stack.back()->Pause();
		}

		stack.push_back(state);
		stack.back()->Init();
	}

	void Pop()
	{
		if(stack.size() > 0)
		{
			stack.back()->Exit();
			delete stack.back();
			stack.pop_back();
		}

		if(stack.size() > 0)
			stack.back()->Resume();
	}

	void Replace(T* item)
	{		
		if(stack.size() > 0)
		{
			stack.back()->Exit();
			delete stack.back();
			stack.pop_back();
		}
		
		stack.push_back(item);
		stack.back()->Init();
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
	
	ActionStack() { }
	ActionStack(const ActionStack&) = delete;
	ActionStack& operator=(ActionStack&& as) noexcept
	{
		if (this != &as)
		{
			for (auto* p : stack) delete p;
			stack = std::move(as.stack);
			as.stack.clear();
		}
		return *this;
	}

	// move constructor
    ActionStack(ActionStack&& other) noexcept
        : stack(std::move(other.stack))
    {
        // leave other.stack empty
        other.stack.clear();
    }

	std::vector<T*> stack;
};
