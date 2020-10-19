#include <iostream>
#include <memory>
#include <io.h>
#include <fcntl.h>

#include "client\render\renderer.h"

using namespace render;

class Board : public Renderable
{
public:
	Board() { test = new MeshFrame(5, 5, {0, 0, 0}); };
	~Board() { delete test; };

	inline void DebugDrawPoint(short x, short y) { test->DrawPoint(COLOR_GREEN, COLOR_BLACK, { x, y, 0 });
	}

	MeshFrame OnRender()
	{
		DebugDrawPoint(0, 0);
		DebugDrawPoint(0, 4);
		DebugDrawPoint(4, 4);
		DebugDrawPoint(4, 0);

		return *test;
	}
private:
	MeshFrame* test;
};

class SecBoard : public Renderable
{
public:
	SecBoard() { mesh = new MeshFrame(16, 5, { 10, 4, 0 }); };
	~SecBoard() { delete mesh; };

	inline void DebugDrawPoint(short x, short y) { mesh->DrawPoint(COLOR_PINK, COLOR_BLACK, { x, y, 0}); }

	MeshFrame OnRender()
	{
		mesh->DrawRect(15, 2, COLOR_PINK, COLOR_BLACK, { 0, 0, 0 });
		mesh->DrawText(L"Hello this is a giant line of txt that won't be able to be rendered!", COLOR_PINK, COLOR_BLACK, { 2, 1, 0 });

		return *mesh;
	}
private:
	MeshFrame* mesh;
};

int main(int argc, char** argv)
{
	Board* t1 = new Board();
	SecBoard* l2 = new SecBoard();

	Renderer* rPtr = Renderer::GetInstance();

	rPtr->init(80, 20);
	rPtr->draw(t1);
	rPtr->draw(l2);
	rPtr->flush();
		 
	return 0;
}