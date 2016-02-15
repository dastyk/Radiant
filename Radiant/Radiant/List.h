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
	int _nrOfElements;


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
	_nrOfElements = 0;
}

template <typename dataType>
List<dataType>::~List()
{
	while (_nrOfElements > 0)
	{
		RemoveCurrentElement();
	}
}

template <typename dataType>
void List<dataType>::AddElementToList(dataType* element, int ID)
{
	Node* node = new Node(element, ID);

	//if (!node)
	//{
	//	std::string toString = typeid(dataType).name();
	//	std::wstring dataTypeName = S2WS(toString);
	//	
	//	throw ErrorMsg(8000001, L"Could not allocate memory for " + dataTypeName + L" in function AddElementToList");
	//}

	if (_nrOfElements == 0)
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

	_nrOfElements++;
}

template <typename dataType>
dataType* List<dataType>::GetCurrentElement()
{
	if (_nrOfElements == 0)
	{
		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000002, L"No elements in List for " + dataTypeName + L" GetCurrentElement.");
	}
	return _current->_data;
}

template<typename dataType>
dataType * List<dataType>::GetElementByID(int ID)
{
	if (_nrOfElements == 0)
	{

		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000003, L"No elements in List for " + dataTypeName + L" GetElementByID");
	}
	Node* walker = _current;
	for (int i = 0; i < _nrOfElements; i++)
	{
		if (walker->_ID == ID)
		{
			return walker->_data;
		}
		walker = walker->_next;
	}

	std::string toString = typeid(dataType).name();
	std::wstring dataTypeName = S2WS(toString);
	throw ErrorMsg(8000004, L"The element not found in List for " + dataTypeName + L" GetElementByID");
}

template <typename dataType>
void List<dataType>::RemoveCurrentElement()
{
	if (_nrOfElements == 0)
	{
		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000005, L"No elements in List for " + dataTypeName + L" RemoveCurrentElement.");
	}
	if (_nrOfElements == 1)
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
	_nrOfElements--;
}

template<typename dataType>
void List<dataType>::RemoveElementByID(int ID)
{
	if (_nrOfElements == 0)
	{

		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000006, L"No elements in List for " + dataTypeName + L"RemoveElementByID, ID: " + ID);
	}
	if (_nrOfElements == 1 && _current->_ID == ID)
	{
		delete _current;
		_current = nullptr;
	}
	else if (_nrOfElements == 1)
	{
		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000007, L"The element with ID: " + ID + L" not found in List for " + dataTypeName + L" RemoveElementByID");
	}
	Node* walker = _current;
	for (int i = 0; i < _nrOfElements; i++)
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
		throw ErrorMsg(8000007, L"The element with ID: " + ID + L" not found in List for " + dataTypeName + L" RemoveElementByID");
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
	if (_nrOfElements == 0)
	{
		std::string toString = typeid(dataType).name();
		std::wstring dataTypeName = S2WS(toString);
		throw ErrorMsg(8000008, L"No elements in List for " + dataTypeName + L" MoveCurrent.");
	}

	if (_current->_next)
	{
		_current = _current->_next;
	}
}

#endif