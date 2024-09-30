#ifndef LINKED_LIST_H
#define	LINKED_LIST_H
#include <memory>

#define OUT_OF_BOUNDS 1

template <class T>
class Node
{
public:
	T data;
	std::weak_ptr<Node<T>> prev;
	std::shared_ptr<Node<T>> next;
	bool newLine;

	Node():next(nullptr)
	{	
	}
	
	Node(T data, bool newLine=false): data(data),next(nullptr),newLine(newLine)
	{
	}

	~Node()
	{}
};

template<class T>
class DoubleLinkedList
{
	public:
	std::shared_ptr<Node<T>> head, tail, active;
	unsigned int size;

	DoubleLinkedList(): head(nullptr), tail(nullptr), active(nullptr),size(0)
	{}

	~DoubleLinkedList()
	{
		head = tail = active = nullptr;
	}

	void Insert(std::shared_ptr<Node<T>> node, T data, bool newLine = true)
	{
		std::shared_ptr<Node<T>> newNode = std::make_shared<Node<T>>(data,newLine);
		size++;
		if(node == nullptr)
		{
			newNode->next = head;
			if(head !=nullptr)
			{
				head->prev = newNode;
			}
			else
			{
				tail = newNode;
				active = newNode;
			}
			head = newNode;
			return;
		}
		std::shared_ptr<Node<T>> nxt = node->next;
		node->next = newNode;
		newNode->prev = node;
		if(nxt == nullptr)
		{
			tail = newNode;
		}
		else
		{
			nxt->prev = newNode;
			newNode->next = nxt;
		}

	}

	void Delete(std::shared_ptr<Node<T>> node)
	{
		if(active == node)
		{
			if(active!=tail)
				++active;
			else if(active!=head)
				--active;
			else
				active = nullptr;
		}
		std::shared_ptr<Node<T>> prev = (node->prev).lock(), next = node->next;
		if(prev!=nullptr)
		{
			prev->next = next;
		}
		else
		{
			head = next;
		}
		if(next != nullptr)
		{
			next->prev = prev;
		}
		else
		{
			tail = prev;
		}
	}

	template<typename U>
	friend inline void operator++(std::shared_ptr<Node<U>>& n);

	template<typename U>
	friend inline void operator--(std::shared_ptr<Node<U>>& n);

};

template <typename T>
inline void operator++(std::shared_ptr<Node<T>>& n)
{
	if(n->next == nullptr)
		throw OUT_OF_BOUNDS;
	n = n->next;
}

template <typename T>
inline void operator--(std::shared_ptr<Node<T>>& n)
{
	if((n->prev).lock() == nullptr)
		throw OUT_OF_BOUNDS;
	n = (n->prev).lock();
}

#endif