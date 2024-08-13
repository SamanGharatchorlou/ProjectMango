#pragma once

#include "Core/TypeDefs.h"

template<class T>
struct node
{
	T data;
	node* next = nullptr;
	node* prev = nullptr;
};


template<class T>
class LinkedList
{
	typedef node<T>* Node;

public:
	LinkedList() : head(nullptr), tail(nullptr) { };

	void insert_back(T data);
	void pop_back();

	bool remove(T data);

	T& get(int i) const;
	T& back() const { return tail->data; }

	node<T>* get_tail() const { return tail; }


	u32 size() const;

private:
	node<T>* head;
	node<T>* tail;
};


template <class T>
T& LinkedList<T>::get(int i) const
{
	int index = 0;
	node<T>* current_node = head;

	while (current_node)
	{
		if (index == i)
		{
			return current_node->data;
		}
		else
		{
			current_node = current_node->next;
			index++;
		}
	}

	ASSERT("Index %d out of bounds, linked list has size %d", i, size());
	return head->data;
}


template <class T>
void LinkedList<T>::insert_back(T data)
{
	// Create a new node to be inserted at the back
	Node newNode = new node<T>();
	newNode->data = data;

	if (!head)
	{
		head = newNode;
		tail = newNode;
	}
	else
	{
		// point the previous tails next to the new tail
		Node prev_tail = tail;
		prev_tail->next = newNode;

		// new node becomes the tail and points to the previous one
		tail = newNode;
		tail->prev = prev_tail;
	}
}

template <class T>
void LinkedList<T>::pop_back()
{
	Node tail_to_delete = tail;

	// if we're at the head we might not have a prev
	if (tail = tail->prev)
		tail->next = nullptr;
	else
		head = nullptr;

	delete tail_to_delete;
}


template <class T>
bool LinkedList<T>::remove(T data)
{
	Node node = head;
	while (node)
	{
		if (node->data == data)
		{
			Node prev_node = node->prev;
			Node next_node = node->next;

			// we could be at the head where there is no prev node
			if(prev_node)
				prev_node->next = next_node;
			
			// we could be at the tail where there is no next node
			if(next_node)
				next_node->prev = prev_node;

			delete node;
			return true;
		}

		node = node->next;
	}

	// No node with the given data was found
	return false;
}


template <class T>
u32 LinkedList<T>::size() const
{
	u32 size = 0;

	node<T>* node = head;
	while (node)
	{
		size++;
		node = node->next;
	}

	return size;
}