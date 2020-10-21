#pragma once

#include <vector>

// Forward declaration of observer class
class IObserver;

class IObservable
{
public:
	virtual void attach(IObserver* obs) = 0;
	virtual void detach(IObserver* obs) = 0;
	virtual void notify() = 0;
protected:
	std::vector<IObserver*> _Olist;
};