#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#ifndef _H_MODEL
#define _H_MODEL

#include "rapidjson/document.h"
#include <vector>

#include "Helpers.h"

namespace model2 {

	struct Action {
		double target_velocity_x;
		double target_velocity_y;
		double target_velocity_z;
		double jump_speed;
		bool use_nitro;

		Action() {
			this->target_velocity_x = 0.0;
			this->target_velocity_y = 0.0;
			this->target_velocity_z = 0.0;
			this->jump_speed = 0.0;
			this->use_nitro = false;
		}

		rapidjson::Value to_json(rapidjson::Document::AllocatorType& allocator) const {
			rapidjson::Value json;
			json.SetObject();
			json.AddMember("target_velocity_x", target_velocity_x, allocator);
			json.AddMember("target_velocity_y", target_velocity_y, allocator);
			json.AddMember("target_velocity_z", target_velocity_z, allocator);
			json.AddMember("jump_speed", jump_speed, allocator);
			json.AddMember("use_nitro", use_nitro, allocator);
			return json;
		}
	};

	struct Arena {
		double width;
		double height;
		double depth;
		double bottom_radius;
		double top_radius;
		double corner_radius;
		double goal_top_radius;
		double goal_width;
		double goal_height;
		double goal_depth;
		double goal_side_radius;

		void read(const rapidjson::Value& json) {
			width = json["width"].GetDouble();
			height = json["height"].GetDouble();
			depth = json["depth"].GetDouble();
			bottom_radius = json["bottom_radius"].GetDouble();
			top_radius = json["top_radius"].GetDouble();
			corner_radius = json["corner_radius"].GetDouble();
			goal_top_radius = json["goal_top_radius"].GetDouble();
			goal_width = json["goal_width"].GetDouble();
			goal_height = json["goal_height"].GetDouble();
			goal_depth = json["goal_depth"].GetDouble();
			goal_side_radius = json["goal_side_radius"].GetDouble();
		}
	};

	struct Entity {
		Vec3 position;
		double radius;
		Vec3 velocity;
		double radius_change_speed;
		double mass;
		double arena_e;
	};

	struct Ball : public Entity {
		double radius;

		void read(const rapidjson::Value& json) {
			position.x = json["x"].GetDouble();
			position.y = json["y"].GetDouble();
			position.z = json["z"].GetDouble();
			velocity.x = json["velocity_x"].GetDouble();
			velocity.y = json["velocity_y"].GetDouble();
			velocity.z = json["velocity_z"].GetDouble();
			radius = json["radius"].GetDouble();
		}
	};

	struct NitroPack : public Entity {
		int id;
		bool alive;
		int respawn_ticks;

		void read(const rapidjson::Value& json) {
			id = json["id"].GetInt();
			position.x = json["x"].GetDouble();
			position.y = json["y"].GetDouble();
			position.z = json["z"].GetDouble();
			radius = json["radius"].GetDouble();
			alive = json["respawn_ticks"].IsNull();
			if (!alive) {
				respawn_ticks = json["respawn_ticks"].GetInt();
			}
		}
	};

	struct Player {
		int id;
		bool me;
		bool strategy_crashed;
		int score;

		void read(const rapidjson::Value& json) {
			id = json["id"].GetInt();
			me = json["me"].GetBool();
			strategy_crashed = json["strategy_crashed"].GetBool();
			score = json["score"].GetInt();
		}
	};

	struct Robot : public Entity {
		int id;
		int player_id;
		bool is_teammate;
		double nitro_amount;
		bool touch;
		Vec3 touch_normal;

		void read(const rapidjson::Value& json) {
			id = json["id"].GetInt();
			player_id = json["player_id"].GetInt();
			is_teammate = json["is_teammate"].GetBool();
			position.x = json["x"].GetDouble();
			position.y = json["y"].GetDouble();
			position.z = json["z"].GetDouble();
			velocity.x = json["velocity_x"].GetDouble();
			velocity.y = json["velocity_y"].GetDouble();
			velocity.z = json["velocity_z"].GetDouble();
			radius = json["radius"].GetDouble();
			nitro_amount = json["nitro_amount"].GetDouble();
			touch = json["touch"].GetBool();
			if (touch) {
				touch_normal.x = json["touch_normal_x"].GetDouble();
				touch_normal.y = json["touch_normal_y"].GetDouble();
				touch_normal.z = json["touch_normal_z"].GetDouble();
			}
		}
	};

	struct Game {
		int current_tick;
		std::vector<Player> players;
		std::vector<Robot> robots;
		std::vector<NitroPack> nitro_packs;
		Ball ball;

		void read(const rapidjson::Value& json) {
			current_tick = json["current_tick"].GetInt();

			rapidjson::Value::ConstArray json_players = json["players"].GetArray();
			players.resize(json_players.Size());
			for (size_t i = 0; i < players.size(); i++) {
				players[i].read(json_players[i]);
			}

			rapidjson::Value::ConstArray json_robots = json["robots"].GetArray();
			robots.resize(json_robots.Size());
			for (size_t i = 0; i < robots.size(); i++) {
				robots[i].read(json_robots[i]);
			}

			rapidjson::Value::ConstArray json_nitro_packs = json["nitro_packs"].GetArray();
			nitro_packs.resize(json_nitro_packs.Size());
			for (size_t i = 0; i < nitro_packs.size(); i++) {
				nitro_packs[i].read(json_nitro_packs[i]);
			}

			ball.read(json["ball"]);
		}
	};

	struct Rules {
		int max_tick_count;
		Arena arena;
		int team_size;
		long long seed;
		double ROBOT_MIN_RADIUS;
		double ROBOT_MAX_RADIUS;
		double ROBOT_MAX_JUMP_SPEED;
		double ROBOT_ACCELERATION;
		double ROBOT_NITRO_ACCELERATION;
		double ROBOT_MAX_GROUND_SPEED;
		double ROBOT_ARENA_E;
		double ROBOT_RADIUS;
		double ROBOT_MASS;
		int TICKS_PER_SECOND;
		int MICROTICKS_PER_TICK;
		int RESET_TICKS;
		double BALL_ARENA_E;
		double BALL_RADIUS;
		double BALL_MASS;
		double MIN_HIT_E;
		double MAX_HIT_E;
		double MAX_ENTITY_SPEED;
		double MAX_NITRO_AMOUNT;
		double START_NITRO_AMOUNT;
		double NITRO_POINT_VELOCITY_CHANGE;
		double NITRO_PACK_X;
		double NITRO_PACK_Y;
		double NITRO_PACK_Z;
		double NITRO_PACK_RADIUS;
		double NITRO_PACK_AMOUNT;
		int NITRO_PACK_RESPAWN_TICKS;
		double GRAVITY;

		void read(const rapidjson::Value& json) {
			max_tick_count = json["max_tick_count"].GetInt();
			arena.read(json["arena"]);
			team_size = json["team_size"].GetInt();
			seed = json["seed"].GetInt64();
			ROBOT_MIN_RADIUS = json["ROBOT_MIN_RADIUS"].GetDouble();
			ROBOT_MAX_RADIUS = json["ROBOT_MAX_RADIUS"].GetDouble();
			ROBOT_MAX_JUMP_SPEED = json["ROBOT_MAX_JUMP_SPEED"].GetDouble();
			ROBOT_ACCELERATION = json["ROBOT_ACCELERATION"].GetDouble();
			ROBOT_NITRO_ACCELERATION = json["ROBOT_NITRO_ACCELERATION"].GetDouble();
			ROBOT_MAX_GROUND_SPEED = json["ROBOT_MAX_GROUND_SPEED"].GetDouble();
			ROBOT_ARENA_E = json["ROBOT_ARENA_E"].GetDouble();
			ROBOT_RADIUS = json["ROBOT_RADIUS"].GetDouble();
			ROBOT_MASS = json["ROBOT_MASS"].GetDouble();
			TICKS_PER_SECOND = json["TICKS_PER_SECOND"].GetInt();
			MICROTICKS_PER_TICK = json["MICROTICKS_PER_TICK"].GetInt();
			RESET_TICKS = json["RESET_TICKS"].GetInt();
			BALL_ARENA_E = json["BALL_ARENA_E"].GetDouble();
			BALL_RADIUS = json["BALL_RADIUS"].GetDouble();
			BALL_MASS = json["BALL_MASS"].GetDouble();
			MIN_HIT_E = json["MIN_HIT_E"].GetDouble();
			MAX_HIT_E = json["MAX_HIT_E"].GetDouble();
			MAX_ENTITY_SPEED = json["MAX_ENTITY_SPEED"].GetDouble();
			MAX_NITRO_AMOUNT = json["MAX_NITRO_AMOUNT"].GetDouble();
			START_NITRO_AMOUNT = json["START_NITRO_AMOUNT"].GetDouble();
			NITRO_POINT_VELOCITY_CHANGE = json["NITRO_POINT_VELOCITY_CHANGE"].GetDouble();
			NITRO_PACK_X = json["NITRO_PACK_X"].GetDouble();
			NITRO_PACK_Y = json["NITRO_PACK_Y"].GetDouble();
			NITRO_PACK_Z = json["NITRO_PACK_Z"].GetDouble();
			NITRO_PACK_RADIUS = json["NITRO_PACK_RADIUS"].GetDouble();
			NITRO_PACK_AMOUNT = json["NITRO_PACK_AMOUNT"].GetDouble();
			NITRO_PACK_RESPAWN_TICKS = json["NITRO_PACK_RESPAWN_TICKS"].GetInt();
			GRAVITY = json["GRAVITY"].GetDouble();
		}
	};

}


#endif
