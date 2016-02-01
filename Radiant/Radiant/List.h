#ifndef _LIST_H_
#define _LIST_H_

#include "General.h"


template <typename dataType>

class List
{

private:
	class Node
	{
	public:
		dataType* _data;
		Node* _next;
		Node* _previous;
		int _ID;
		Node(dataType* newData, int ID)
		{
			this->_ID = ID;
			this->_data = newData;
			this->_next = nullptr;
			this->_previous = nullptr;
		}
		~Node()
		{
			if (_data)
			{
				delete _data;
				_data = nullptr;
			}
			if (_next)
			{
				this->_next = nullptr;
			}
			if (_previous)
			{
				this->_previous = nullptr;
			}
		}
	};
	Node* _current;
	int _nrOFElements;


public:
	List();
	~List();

	void AddElementToList(dataType* element, int ID);
	dataType* GetCurrentElement();
	dataType* GetElementByID(int ID);
	void RemoveCurrentElement();
	void RemoveElementByID(int ID);
	int Size();
	void MoveCurrent();

};

template <typename dataType>
List<dataType>::List()
{
	_current = nullptr;
	_nrOFElements = 0;
}

template <typename dataType>
List<dataType>::~List()
{
	if (_current)
	{
		delete _current;
		_current = nullptr;
	}
}

template <typename dataType>
void List<dataType>::AddElementToList(dataType* element, int ID)
{
	Node* node = new Node(element, ID);

	if (!node)
	{
		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		
		throw ErrorMsg(8000001, L"Could not allocate memory for " + dataTypeName + L" in function AddElementToList");
	}

	if (_nrOFElements == 0)
	{
		node->_next = node;
		node->_previous = node;
		_current = node;
	}
	else
	{
		node->_next = _current->_next;
		node->_previous = _current;
		_current->_next = node;
		node->_next->_previous = node;
		_current = node;

	}

	_nrOFElements++;
}

template <typename dataType>
dataType* List<dataType>::GetCurrentElement()
{
	if (_nrOFElements == 0)
	{
		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000002, L"No elements in List for " + dataTypeName + " GetCurrentElement.");
	}
	return _current->_data;
}

template<typename dataType>
dataType * List<dataType>::GetElementByID(int ID)
{
	if (_nrOFElements == 0)
	{

		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000003, L"No elements in List for " + dataTypeName + " GetElementByID, ID: " + ID);
	}
	Node* walker = _current;
	for (int i = 0; i < _nrOFElements; i++)
	{
		if (walker->_ID == ID)
		{
			return walker;
		}
		walker = walker->_next;
	}

	std::string toString = typeid(dataType).name();
	std::wstring dataTypeName = S2WS(toString);
	throw ErrorMsg(8000004, L"The element with ID: " + ID + " not found in List for " + dataTypeName + " GetElementByID");
}

template <typename dataType>
void List<dataType>::RemoveCurrentElement()
{
	if (_nrOFElements)
	{
		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000005, L"No elements in List for " + dataTypeName + " RemoveCurrentElement.");
	}
	if (_nrOFElements == 1)
	{
		delete _current;
		_current = nullptr;

	}
	else
	{
		Node* temp = _current;
		_current->_previous->_next = _current->_next;
		_current->_next->_previous = _current->_previous;
		_current = _current->_next;
		delete temp;
		temp = nullptr;
	}
	_nrOFElements--;
}

template<typename dataType>
void List<dataType>::RemoveElementByID(int ID)
{
	if (_nrOFElements)
	{

		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000006, L"No elements in List for " + dataTypeName + "RemoveElementByID, ID: " + ID);
	}
	if (_nrOfElements == 1 && _current->_ID == ID)
	{
		delete _current;
		_current = nullptr;
	}
	else if (_nrOFElements == 1)
	{
		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000007, L"The element with ID: " + ID + "not found in List for " + dataTypeName + " RemoveElementByID");
	}
	Node* walker = _current;
	for (int i = 0; i < _nrOFElements; i++)
	{
		if (walker->_ID == ID)
		{
			walker->_previous->_next = _walker->_next;
			walker->_next->_previous = _walker->_previous;
			delete walker;
			walker = nullptr;
			return;
		}
		walker = walker->_next;
	}
	else
	{
		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000007, L"The element with ID: " + ID + "not found in List for " + dataTypeName + " RemoveElementByID");
	}
}

template <typename dataType>
int List<dataType>::Size()
{
	return _nrOfElements;
}

template <typename dataType>
void List<dataType>::MoveCurrent()
{
	if (_nrOFElements == 0)
	{
		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000008, L"No elements in List for " + dataTypeName + " MoveCurrent.");
	}

	if (_current->_next)
	{
		_current = _current->_next;
	}
}

#endif