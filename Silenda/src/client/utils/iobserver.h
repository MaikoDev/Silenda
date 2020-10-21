#pragma once

// Forward declaration of observable class
class IObservable;

class IObserver
{
public:
	virtual void update(IObservable* src) = 0;
};