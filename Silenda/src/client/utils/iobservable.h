#pragma once

// Forward declaration of observer class
class IObserver;

class IObservable
{
public:
	~IObservable() { _Olist.clear(); }

	virtual void attach(IObserver* obs) = 0;
	virtual void detach(IObserver* obs) = 0;
	virtual void notify(const unsigned char controller = 0) = 0;
protected:
	std::unordered_map<std::string, IObserver*> _Olist;
};