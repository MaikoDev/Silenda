#pragma once

// Forward declaration of observable class
class IObservable;

class IObserver
{
public:
	virtual void update(IObservable* src, const unsigned char controller = 0) = 0;
};