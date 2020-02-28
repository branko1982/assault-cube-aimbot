#include "cheat.h"
#include "Logger.hpp"
#include <thread>
#include <cstdint>
#include <string>

#include <windows.h>
#include <gl\GL.h>
#include "glDraw.h"
#include "gltext.h"

#include "worldToScreen.h"
#include "CFont.hpp"
float PI = 3.141592f;

static_memory_addresses_t static_addresses;
entity_offsets_t entity_offsets;
instruction_addresses_t instruction_addresses;
settings_t settings;



bool is_player_visible_relative_to_another_player(vec3 from, vec3 to)
{
	static DWORD is_player_visible = instruction_addresses.is_player_visible; //this is NOT memory address. It's instruction address Check what's under this address in IDA
	__asm
	{
		push to.z
		push to.y
		push to.x
		push from.z
		push from.y
		push from.x
		xor eax, eax
		call is_player_visible
		add esp, 24
	}
}


void aimbot()
{
	bool local_player_exists = ((*(uintptr_t*)static_addresses.pointer_local_player_3) != 0);

	if (local_player_exists) {


		entity_t local_player;
		entity_t my_entity_list[32]; //game was crashing without entity list big enough


		local_player.has_aimbot_locked_on_enemy = false;
		local_player.base_address = *(uintptr_t*)(static_addresses.pointer_local_player_3);

		local_player.p_hp = (int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.hp);
		local_player.p_armor = (int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.armor);
		local_player.p_name = (char*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.name);
		local_player.p_position_x = (float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_x);
		local_player.p_position_y = (float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_y);
		local_player.p_position_z = (float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_z);
		local_player.p_aim_x = (float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.aim_x);
		local_player.p_aim_y = (float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.aim_y);
		local_player.p_primary_weapon_ammo = (int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.primary_weapon_ammo);
		local_player.p_secondary_weapon_ammo = (int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.secondary_weapon_ammo);
		local_player.p_team = (int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.team);


		//copy the information about player from RAM to your own structure and work with data that way
		//its better than accessing memory directly and risking access violations

		local_player.hp = *(int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.hp);
		local_player.armor = *(int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.armor);
		memcpy((void*)(local_player.copied_name), (void*)((static_addresses.pointer_local_player_3) + entity_offsets.name), 16);
		local_player.position.x = *(float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_x);
		local_player.position.y = *(float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_y);
		local_player.position.z = *(float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_z);
		local_player.aim_x = *(float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.aim_x);
		local_player.aim_y = *(float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.aim_y);
		local_player.primary_weapon_ammo = *(int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.primary_weapon_ammo);
		local_player.secondary_weapon_ammo = *(int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.secondary_weapon_ammo);
		local_player.team = *(int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.team);




		bool is_local_player_alive = (local_player.hp > 0);

		if (is_local_player_alive) {

			if (settings.no_recoil) {
				DWORD OldProt;
				VirtualProtect((void*)static_addresses.local_player_recoil, 4, PAGE_EXECUTE_READWRITE, &OldProt);
				*(unsigned int*)(static_addresses.local_player_recoil) = 0;
			}

			if (GetAsyncKeyState(VK_LBUTTON)) {

				uintptr_t entity_list = *(uintptr_t*)static_addresses.pointer_entity_list; //array of the pointers each pointing to indivual players

				if (entity_list != 0) {

					int number_of_players = *(uintptr_t*)static_addresses.number_of_players; //array of the pointers each pointing to indivual players
					number_of_players -= 1; //local player does not count

					//we fill the entity list
					if (number_of_players > 0) {
						for (int x = 0; x < number_of_players; x++) {

							uintptr_t address_of_single_bot_entity_pointer = entity_list + (x + 1) * 4;
							uintptr_t single_bot_entity = *(uintptr_t*)address_of_single_bot_entity_pointer;

							//despite name, continue does not continue executing code. It jumps to next iteration and cancels current one
							//continue can be used only in loops
							if (single_bot_entity == 0) {
								continue;
							}

							entity_t bot_entity;
							bot_entity.base_address = single_bot_entity;

							bot_entity.hp = *(int*)(single_bot_entity + entity_offsets.hp);
							bot_entity.armor = *(int*)(single_bot_entity + entity_offsets.armor);

							memcpy((void*)(bot_entity.copied_name), (void*)((single_bot_entity + entity_offsets.name)), 16);
							//bot_entity.copied_name = *(char*)(single_bot_entity + entity_offsets.name); wrong

							bot_entity.position.x = *(float*)(single_bot_entity + entity_offsets.position_x);
							bot_entity.position.y = *(float*)(single_bot_entity + entity_offsets.position_y);
							bot_entity.position.z =  *(float*)(single_bot_entity + entity_offsets.position_z);
							bot_entity.aim_x = *(float*)(single_bot_entity + entity_offsets.aim_x);
							bot_entity.aim_y = *(float*)(single_bot_entity + entity_offsets.aim_y);
							bot_entity.primary_weapon_ammo = *(int*)(single_bot_entity + entity_offsets.primary_weapon_ammo);
							bot_entity.secondary_weapon_ammo = *(int*)(single_bot_entity + entity_offsets.secondary_weapon_ammo);
							bot_entity.team = *(int*)(single_bot_entity + entity_offsets.team);

							my_entity_list[x] = bot_entity;

						}

						static entity_t closest_enemy;//must be static

						float current_distance = 100000.f;

						if (local_player.has_aimbot_locked_on_enemy) {
							bool is_enemy_alive = (closest_enemy.hp > 0);
							if (!is_enemy_alive) {
								local_player.has_aimbot_locked_on_enemy = false;
							}
						}

						//loops trough enemies, find correct one
						if (!local_player.has_aimbot_locked_on_enemy) {

							for (int x = 0; x < number_of_players - 1; x++) {

								entity_t current_entity_in_loop = my_entity_list[x];


								if (settings.aimbot_visibility_check) {
									bool is_visible = is_player_visible_relative_to_another_player(local_player.position, current_entity_in_loop.position);

									if (!is_visible) {
										continue;
									}
								}

								int game_mode = *(int*)(static_addresses.game_mode);
								bool team_check_required = (game_mode == 0 || game_mode == 4 || game_mode == 5 || game_mode == 7 || game_mode == 11 || game_mode == 13 || game_mode == 14 || game_mode == 16 || game_mode == 17 || game_mode == 20 || game_mode == 21);
								bool team_is_valid = true;

								if (team_check_required) {
									if (current_entity_in_loop.team != local_player.team) {
										team_is_valid = true;
									}
									else {
										team_is_valid = false;
									}
								}
								if (!team_is_valid) {
									continue;
								}

								bool is_enemy_alive = (current_entity_in_loop.hp > 0);

								if (!is_enemy_alive) {
									continue;
								}

								float enemy_distance = sqrt(pow(local_player.position.x - current_entity_in_loop.position.x, 2) + pow(local_player.position.y - current_entity_in_loop.position.y, 2) + pow(local_player.position.z - current_entity_in_loop.position.z, 2));

								float target_aim_x = -atan2f(current_entity_in_loop.position.x - local_player.position.x, current_entity_in_loop.position.y - local_player.position.y) / PI * 180.0f + 180.0f;
								float target_aim_y = atan((current_entity_in_loop.position.z - local_player.position.z) / enemy_distance) * 180 / PI;


								if (settings.aimbot_use_field_of_view) {
									bool aimbot_aim_within_field_of_view = false;

									float difference_x = (local_player.aim_x - target_aim_x);
									float difference_y = (local_player.aim_y - target_aim_y);

									if ((difference_x) <= 45.f && (difference_x) >= -45.f && (difference_y) <= 45.f && (difference_y) >= -45.f) {
										aimbot_aim_within_field_of_view = true;
									}

									if (!aimbot_aim_within_field_of_view) {
										continue;
									}
								}
								if (enemy_distance < current_distance) {
									closest_enemy = current_entity_in_loop;
									current_distance = enemy_distance;
									local_player.has_aimbot_locked_on_enemy = true;
								}
							}
						}

						if (local_player.has_aimbot_locked_on_enemy) {

							float target_aim_x = -atan2f(closest_enemy.position.x - local_player.position.x, closest_enemy.position.y - local_player.position.y) / PI * 180.0f + 180.0f;
							float target_aim_y = atan((closest_enemy.position.z - local_player.position.z) / current_distance) * 180 / PI;

							if (settings.aimbot_smoothing) {

								float difference_x = (local_player.aim_x - target_aim_x) / 5;
								float difference_y = (local_player.aim_y - target_aim_y) / 5;

								if (target_aim_x > local_player.aim_x) {
									if (difference_x > 0) {
										*local_player.p_aim_x += difference_x;
									}
									if (difference_x < 0) {
										*local_player.p_aim_x -= difference_x;
									}
								}
								if (target_aim_x < local_player.aim_x) {
									if (difference_x > 0) {
										*local_player.p_aim_x -= difference_x;
									}
									if (difference_x < 0) {
										*local_player.p_aim_x += difference_x;
									}
								}
								if (target_aim_y > local_player.aim_y) {
									if (difference_y > 0) {
										*local_player.p_aim_y += difference_y;
									}
									if (difference_y < 0) {
										*local_player.p_aim_y -= difference_y;
									}
								}

								if (target_aim_y < local_player.aim_y) {
									if (difference_y > 0) {
										*local_player.p_aim_y -= difference_y;
									}
									if (difference_y < 0) {
										*local_player.p_aim_y += difference_y;
									}
								}
							}
							else
							{
								*local_player.p_aim_x = target_aim_x;
								*local_player.p_aim_y = target_aim_y;
							}
						}
					}
					else {
						local_player.has_aimbot_locked_on_enemy = false;
					}
				}
			}

		}
	}
}
void esp()
{
	bool local_player_exists = ((*(uintptr_t*)static_addresses.pointer_local_player_3) != 0);

	if (local_player_exists) {


		entity_t local_player;
		entity_t my_entity_list[32];


		local_player.has_aimbot_locked_on_enemy = false;
		local_player.base_address = *(uintptr_t*)(static_addresses.pointer_local_player_3);

		local_player.p_hp = (int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.hp);
		local_player.p_armor = (int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.armor);
		local_player.p_name = (char*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.name);
		local_player.p_position_x = (float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_x);
		local_player.p_position_y = (float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_y);
		local_player.p_position_z = (float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_z);
		local_player.p_aim_x = (float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.aim_x);
		local_player.p_aim_y = (float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.aim_y);
		local_player.p_primary_weapon_ammo = (int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.primary_weapon_ammo);
		local_player.p_secondary_weapon_ammo = (int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.secondary_weapon_ammo);
		local_player.p_team = (int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.team);


		local_player.hp = *(int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.hp);
		local_player.armor = *(int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.armor);
		memcpy((void*)(local_player.copied_name), (void*)((static_addresses.pointer_local_player_3) + entity_offsets.name), 16);
		local_player.position.x = *(float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_x);
		local_player.position.y = *(float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_y);
		local_player.position.z = *(float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.position_z);
		local_player.aim_x = *(float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.aim_x);
		local_player.aim_y = *(float*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.aim_y);
		local_player.primary_weapon_ammo = *(int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.primary_weapon_ammo);
		local_player.secondary_weapon_ammo = *(int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.secondary_weapon_ammo);
		local_player.team = *(int*)((*(uintptr_t*)static_addresses.pointer_local_player_3) + entity_offsets.team);

		bool is_local_player_alive = (local_player.hp > 0);

		if (is_local_player_alive) {

			//this code will display name team and hp of bots
			uintptr_t entity_list = *(uintptr_t*)static_addresses.pointer_entity_list; //array of the pointers each pointing to indivual players

			if (entity_list != 0) {

				int number_of_players = *(uintptr_t*)static_addresses.number_of_players; //array of the pointers each pointing to indivual players
				number_of_players -= 1; //local player does not count
				//we fill the entity list


				if (number_of_players > 0) {
					//fill my_entity_list array with relevant pointers
					for (int x = 0; x < number_of_players; x++) {

						uintptr_t address_of_single_bot_entity_pointer = entity_list + (x + 1) * 4;
						uintptr_t single_bot_entity = *(uintptr_t*)address_of_single_bot_entity_pointer;

						if (single_bot_entity == 0) {
							continue;
						}

						entity_t bot_entity;

						bot_entity.base_address = single_bot_entity;

						bot_entity.hp = *(int*)(single_bot_entity + entity_offsets.hp);
						bot_entity.armor = *(int*)(single_bot_entity + entity_offsets.armor);
						memcpy((void*)(bot_entity.copied_name), (void*)((single_bot_entity + entity_offsets.name)), 16);

						//bot_entity.copied_name = *(char*)(single_bot_entity + entity_offsets.name); wrong
						bot_entity.position.x = *(float*)(single_bot_entity + entity_offsets.position_x);
						bot_entity.position.y = *(float*)(single_bot_entity + entity_offsets.position_y);
						bot_entity.position.z = *(float*)(single_bot_entity + entity_offsets.position_z);
						bot_entity.aim_x = *(float*)(single_bot_entity + entity_offsets.aim_x);
						bot_entity.aim_y = *(float*)(single_bot_entity + entity_offsets.aim_y);
						bot_entity.primary_weapon_ammo = *(int*)(single_bot_entity + entity_offsets.primary_weapon_ammo);
						bot_entity.secondary_weapon_ammo = *(int*)(single_bot_entity + entity_offsets.secondary_weapon_ammo);
						bot_entity.team = *(int*)(single_bot_entity + entity_offsets.team);

						my_entity_list[x] = bot_entity;

					}
					//drawing of player information here
				}

				number_of_players = *(uintptr_t*)static_addresses.number_of_players; //array of the pointers each pointing to indivual players
				number_of_players -= 1; //local player does not count

				for (int x = 0; x < number_of_players; x++) {

					uintptr_t address_of_single_bot_entity_pointer = entity_list + (x + 1) * 4;
					uintptr_t single_bot_entity = *(uintptr_t*)address_of_single_bot_entity_pointer;

					if (single_bot_entity == 0) {
						continue;
					}



					//DWORD OldProt;
					//VirtualProtect((void*)my_entity_list[x].hp, 4, PAGE_EXECUTE_READ, &OldProt);

					bool is_enemy_alive = (my_entity_list[x].hp > 0);

					if (is_enemy_alive) {

						float* matrix = (float*)(static_addresses.view_matrix);

						vec2 draw_2d;
						draw_2d.x = 0;
						draw_2d.y = 0;

						vec3 entity_pos;
						entity_pos.x = my_entity_list[x].position.x;
						entity_pos.y = my_entity_list[x].position.y;
						entity_pos.z = my_entity_list[x].position.z;


						int resolution_width = *(int*)(static_addresses.resolution_width);
						int resolution_height = *(int*)(static_addresses.resolution_height);

						WorldToScreen(entity_pos, &draw_2d, matrix, resolution_width, resolution_height);

						draw_2d.x = draw_2d.x - 20.f;
						draw_2d.y = draw_2d.y - 20.f;

						vec3 my_position;
						my_position.x = local_player.position.x;
						my_position.y = local_player.position.y;
						my_position.z = local_player.position.z;

						vec3 enemy_position;
						enemy_position.x = my_entity_list[x].position.x;
						enemy_position.y = my_entity_list[x].position.y;
						enemy_position.z = my_entity_list[x].position.z;


						bool is_player_visible = is_player_visible_relative_to_another_player(my_position, enemy_position);

						if (is_player_visible) {
							int game_mode = *(int*)(static_addresses.game_mode);
							bool team_check_required = (game_mode == 0 || game_mode == 4 || game_mode == 5 || game_mode == 7 || game_mode == 11 || game_mode == 13 || game_mode == 14 || game_mode == 16 || game_mode == 17 || game_mode == 20 || game_mode == 21);

							float enemy_distance = sqrt(pow(local_player.position.x - my_entity_list[x].position.x, 2) + pow(local_player.position.y - my_entity_list[x].position.y, 2) + pow(local_player.position.z - my_entity_list[x].position.z, 2));

							//calculations of player box dimensions for ESP. It will be better if you do your own

							float box_width = resolution_width / 20.f / (enemy_distance / 30.f);
							float box_height = resolution_height / 8.571f / (enemy_distance / 30.f);
							float health_pixel_height = 70.f / 100.f * my_entity_list[x].hp / (enemy_distance / 30.f);

							GLubyte health_color[3] = { 0,240,0 };

							if (team_check_required) {
								if (my_entity_list[x].team != local_player.team) {

									GL::DrawOutline(draw_2d.x, draw_2d.y, 2, health_pixel_height, 1.f, health_color);

									GLubyte color[3] = { 255,0,0 };
									GL::DrawOutline(draw_2d.x, draw_2d.y, box_width, box_height, 1.f, color);
								}
								else {
									GL::DrawOutline(draw_2d.x, draw_2d.y, 2, health_pixel_height, 1.f, health_color);

									GLubyte color[3] = { 0,255,0 };
									GL::DrawOutline(draw_2d.x, draw_2d.y, box_width, box_height, 1.f, color);
								}
							}
							else
							{
								GL::DrawOutline(draw_2d.x, draw_2d.y, 2, health_pixel_height, 1.f, health_color);

								GLubyte color[3] = { 255,0,0 };
								GL::DrawOutline(draw_2d.x, draw_2d.y, box_width, box_height, 1.f, color);
							}

							CFont font(15, 128, 128, 128); //make "pen"
							font.Print(draw_2d.x, draw_2d.y - 10.f, my_entity_list[x].copied_name); //prints "Hello World!" string at (100,100) screen coordinates
						}
					}
				}
			}
		}
	}

}

unsigned long __stdcall cheat_main()
{

	if (settings.aimbot_active) {
		aimbot();
	}
	if (settings.esp_active) {
		esp();
	}
	return 0;
}