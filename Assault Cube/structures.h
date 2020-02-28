#include <cstddef>

#pragma once


#pragma once

struct vec2
{
	float x;
	float y;
};

struct vec3
{
	float x;
	float y;
	float z;
};

struct vec4
{
	float x;
	float y;
	float z;
	float w;
};




struct settings_t {
	bool aimbot_active = true;
	bool aimbot_smoothing = true;
	bool aimbot_visibility_check = true;
	bool aimbot_use_field_of_view = true;
	bool no_recoil = true;
	bool esp_active = true;
};

//thank god this game is programmed in such a way, that it contains static addresses./

struct static_memory_addresses_t {
	int pointer_local_player_1 = 0x00509B74;
	int pointer_local_player_2 = 0x0050f4f4;
	int pointer_local_player_3 = 0x0051E20c;
	int pointer_entity_list = 0x0050f4f8;
	// important notes
	// * entity is list array of pointers (each pointer 4 bytes long), where each pointer is poiting to bot/player.
	// * if there are no other players, the pointer to entity_list is 0. Always check if it's or is not null
	// * also, in the array of pointers, eg the entity list itself, the value of first offset is always 0. therefore, to get the address of the first bot, first add 4 bytes to address of entity list
	int player_names = 0x4FEDAC;
	int number_of_players = 0x0050A22C;
	int game_mode = 0x50F49C; //I have to yet find out how to get this address
	int trace_line = 0x048a310;
	int local_player_recoil = 0x004EE444;
	int view_matrix = 0x501AE8;
	int resolution_width = 0x00510C94;
	int resolution_height = 0x00510C98;

};

struct instruction_addresses_t
{
	int is_player_visible = 0x48ABD0;
};



struct entity_offsets_t {
	int position_x = 0x4; //float
	int position_y = 0x8; //float
	int position_z = 0xC; //float
	int aim_x = 0x40; //int
	int aim_y = 0x44;//int
	int hp = 0xf8; //int
	int armor = 0xf; //int
	int primary_weapon_ammo = 0x150;//int
	int secondary_weapon_ammo = 0x13c; //int
	int name = 549; //char[16]
	int team = 0x032C;
	int velocity = 0x10;
	int state = 0x0082;
};



struct entity_t {
	float* p_position_x;
	float* p_position_y;
	float* p_position_z;
	float* p_aim_x;
	float* p_aim_y;
	int* p_hp;
	int* p_armor;
	int* p_primary_weapon_ammo;
	int* p_secondary_weapon_ammo;
	char* p_name;
	int* p_team;

	float velocity;
	bool has_aimbot_locked_on_enemy; //my variable, not in cheat engine
	unsigned char* p_state;
	uintptr_t base_address;

	vec3 position;
	float aim_x;
	float aim_y;
	int hp;
	int armor;
	int primary_weapon_ammo;
	int secondary_weapon_ammo;
	char copied_name[16] = {0};
	int team;


};
