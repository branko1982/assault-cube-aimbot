#pragma once
//assautl cubes uses  Uses OpenGL & SDL. so it  cannot be hooked by direct

#include "structures.h"

struct traceresult_s
{
	vec3 end;
	bool collided;
};

void aimbot();
void esp();
bool is_player_visible_relative_to_another_player(vec3 from, vec3 to);
unsigned long __stdcall cheat_main();