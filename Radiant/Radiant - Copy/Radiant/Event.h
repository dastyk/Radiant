#ifndef _EVENT_H_
#define _EVENT_H_

#include <list>

/*
What is this file? A delegate class that can be used to pass free functions or
member functions to an event. See more at http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
There doesn't seem to exist a much simpler way. std::function was considered,
but due to lack of an equality operator they cannot be compared, ergo they also
cannot be unsubscribed. Game Coding Complete says that C#-style events would be
nice, but C++ doesn't support them out of the box. They do note however that it
can be done using alot of template magic, with emphasis on alot. This solution
works, but the syntax is horrible.

The event itself is just a collection of delegates that should be called when
the event occurs. Triggering the event is done by the one owning the event.
*/

template <typename T> class Delegate;

template <typename ReturnType, typename... ParamTypes>
class Delegate<ReturnType (ParamTypes...)>
{
	typedef ReturnType ( *Type )(void* callee, ParamTypes...);

public:
	Delegate(void* callee, Type function) : _callee(callee), _callbackFunction(function) {}

	template <class T, ReturnType (T::*TMethod)(ParamTypes...)>
	static Delegate Make( T* callee )
	{
		Delegate d( callee, &methodCaller<T, TMethod> );
		return d;
	}

	template <ReturnType (*funcPtr)(ParamTypes...)>
	static Delegate Make()
	{
		Delegate d( nullptr, &functionCaller<funcPtr> );
		return d;
	}

	ReturnType operator()( ParamTypes... x ) const
	{
		return (*_callbackFunction)(_callee, x...);
	}

	bool operator==( const Delegate& other ) const
	{
		return (_callee == other._callee) && (_callbackFunction == other._callbackFunction);
	}

private:
	void* _callee;
	Type _callbackFunction;

	template <class T, ReturnType (T::*TMethod)(ParamTypes...)>
	static ReturnType methodCaller( void* callee, ParamTypes... x )
	{
		T* p = static_cast<T*>(callee);
		return (p->*TMethod)(x...);
	}

	template <ReturnType (*func)(ParamTypes...)>
	static ReturnType functionCaller( void* unused, ParamTypes... x )
	{
		return (*func)(x...);
	}
};

template <typename T> class Event;

template <typename ReturnType, typename... Arguments>
class Event<ReturnType (Arguments...)>
{
public:
	Event()
	{

	}

	void operator()( Arguments... params ) // Potential issue: could be called from outside owning class?	
	{
		for ( auto& func : _callbacks )
		{
			func( std::forward<Arguments>(params)... );
		}
	}

	void operator+=(const Delegate<ReturnType(Arguments...)>& func)
	{
		_callbacks.push_back( func );
	}

	void operator-=(const Delegate<ReturnType(Arguments...)>& func)
	{
		_callbacks.remove( func );
	}
	
private:
	Event& operator=( const Event& rhs );
	Event( const Event& other );

private:
	std::list<Delegate<ReturnType(Arguments...)>> _callbacks;
};

#endif