#pragma once
#include <Novice.h>
#include<Vector2.h>
class Slime
{
	public:
		void Initialize();
		void Update();
		void Draw();

private:
	Vector2 pos[50];
	Vector2 velocity[50];
	Vector2 radius;


};

