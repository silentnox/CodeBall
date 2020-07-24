#pragma once

#ifndef _H_PREDICT
#define _H_PREDICT

#include "model/Rules.h"
#include "model/Game.h"
#include "model/Action.h"
#include "model/Robot.h"

#include "Helpers.h"

#include <tuple>
#include <algorithm>
#include <math.h>
#include <string>

const double ROBOT_MIN_RADIUS = 1;
const double  ROBOT_MAX_RADIUS = 1.05;
const double  ROBOT_MAX_JUMP_SPEED = 15;
const double  ROBOT_ACCELERATION = 100;
const double  ROBOT_NITRO_ACCELERATION = 30;
const double  ROBOT_MAX_GROUND_SPEED = 30;
const double  ROBOT_ARENA_E = 0;
const double  ROBOT_RADIUS = 1;
const double  ROBOT_MASS = 2;
const int TICKS_PER_SECOND = 60;
const int MICROTICKS_PER_TICK = 100;
const double  RESET_TICKS = 2 * TICKS_PER_SECOND;
const double  BALL_ARENA_E = 0.7;
const double  BALL_RADIUS = 2;
const double  BALL_MASS = 1;
const double  MIN_HIT_E = 0.4;
const double  MAX_HIT_E = 0.5;
const double  MAX_ENTITY_SPEED = 100;
const double  MAX_NITRO_AMOUNT = 100;
const double  START_NITRO_AMOUNT = 50;
const double  NITRO_POINT_VELOCITY_CHANGE = 0.6;
const double  NITRO_PACK_X = 20;
const double  NITRO_PACK_Y = 1;
const double  NITRO_PACK_Z = 30;
const double  NITRO_PACK_RADIUS = 0.5;
const double  NITRO_PACK_AMOUNT = 100;
const int  NITRO_PACK_RESPAWN_TICKS = 10 * TICKS_PER_SECOND;
const double  GRAVITY = 30;

const double ROBOT_ACCELERATION_INV = 1.0 / ROBOT_ACCELERATION;
const double TICK_DURATION = 1.0 / TICKS_PER_SECOND;

const double ROBOT_NITRO_SPEED = MAX_NITRO_AMOUNT * NITRO_POINT_VELOCITY_CHANGE;

const int GAME_DEFAULT_DURATION = 18000;

const int MAX_ROBOTS = 6;
const int MAX_NITRO_PACKS = 4;

const double MAX_JUMP_HEIGHT = 3.75 + ROBOT_RADIUS;

namespace Arena {
	const double aw = 60; // arena width
	const double ah = 20; // arena height 
	const double ad = 80; // arena depth
	const double br = 3; // bottom radius 
	const double tr = 7; // top radius
	const double cr = 13; // corner radius
	const double gtr = 3; // goal top radius
	const double gw = 30; // goal width
	const double gh = 10; // goal height
	const double gd = 10; // goal depth
	const double gsr = 1; // goal side radius
	const double ad2 = 40; // half arena depth
	const double aw2 = 30; // half arena width
	const double gw2 = 15; // half goal width
}

//const double MaxJumpHeight = 3.75 + ROBOT_RADIUS;
//const double AccelLength = 4.50;
//const double AccelLengthMax = 6.36;
//const int AccelTicks = 18;
//const int AccelTicksMax = 24;
//const int JumpTicks = 60;
//const double MaxJumpDist = 30.0;
//const double MinShift = 0.027;

struct Action {
	Vec3 target_velocity = Vec3( 0, 0, 0 );
	double jump_speed = 0.0;
	bool use_nitro = false;

	inline Action() {
	}
	inline Action( model::Action a ) {
		target_velocity.x = a.target_velocity_x;
		target_velocity.y = a.target_velocity_y;
		target_velocity.z = a.target_velocity_z;
		jump_speed = a.jump_speed;
		use_nitro = a.use_nitro;
	}
	inline Action( Vec3 v, double js = 0.0, bool nitro = false ) {
		target_velocity = v;
		jump_speed = js;
		use_nitro = nitro;
	}
	inline operator model::Action() const {
		model::Action a;
		a.target_velocity_x = target_velocity.x;
		a.target_velocity_y = target_velocity.y;
		a.target_velocity_z = target_velocity.z;
		a.jump_speed = jump_speed;
		a.use_nitro = use_nitro;
		return a;
	}

	inline bool IsEmpty() const {
		return target_velocity.IsZero() && jump_speed == 0 && use_nitro == false;
	}
};


struct Entity;

struct TracePoint {
	Vec3 position = Vec3(0,0,0);
	Vec3 velocity = Vec3(0,0,0);

	TracePoint() {};
	TracePoint( const Vec3 & pos, const Vec3 & vel ) :position( pos ), velocity( vel ) {};
	TracePoint( const Entity & e );

	TracePoint & operator=( const Entity & e );
};


struct Entity : public TracePoint {
	//Vec3 position = Vec3( 0, 0, 0 );
	double radius = 0;
	//Vec3 velocity = Vec3( 0, 0, 0 );
	double radius_change_speed = 0;
	double mass = 0.0;
	double arena_e = 0.0;
	
	bool touch = false;
	Vec3 touch_normal = Vec3( 0, 0, 0 );
	double dist = 0.0;

	int lastCollideArena = -1;
	int lastCollideRobot = -1;

	bool applyMove = true;
	bool enabled = true;

	inline Entity() {
	}

	//inline Entity( const Vec3 & p, const Vec3 & v ) :position( p ), velocity( v ) {};
	inline Entity( const Vec3 & p, const Vec3 & v ) :TracePoint(p,v) {};

	inline void SetPosition( const Vec3 & p ) {
		position = p;
	}
	inline void SetPosition( const Vec3 & p, const Vec3 & v ) {
		position = p;
		velocity = v;
	}
};

TracePoint::TracePoint( const Entity & e ) :position( e.position ), velocity( e.velocity ) {};

TracePoint & TracePoint::operator=( const Entity & e ) {
	position = e.position;
	velocity = e.velocity;
	return *this;
}

struct Ball : public Entity {
	inline Ball() {
		arena_e = BALL_ARENA_E;
		mass = BALL_MASS;
		radius = BALL_RADIUS;
		radius_change_speed = 0;
	}
	inline Ball( const model::Ball & b ) {
		position.x = b.x;
		position.y = b.y;
		position.z = b.z;
		velocity.x = b.velocity_x;
		velocity.y = b.velocity_y;
		velocity.z = b.velocity_z;
		radius = b.radius;
		radius_change_speed = 0;
		touch_normal = Vec3( 0, 0, 0 );

		arena_e = BALL_ARENA_E;
		mass = BALL_MASS;
	}
};

struct Robot : public Entity {
	int id = -1;
	int player_id = -1;
	bool is_teammate = false;
	double nitro_amount = 0;
	//bool touch = false;
	Action action;

	int lastHitBall = -1;

	inline Robot() {
		arena_e = ROBOT_ARENA_E;
		mass = ROBOT_MASS;
		radius = ROBOT_RADIUS;
		radius_change_speed = 0;
		touch = false;
		nitro_amount = 0;
	}
	inline Robot( Vec3 p, Vec3 v ) :Entity( p, v ) {};
	inline Robot( const model::Robot & r ) {
		position.x = r.x;
		position.y = r.y;
		position.z = r.z;
		velocity.x = r.velocity_x;
		velocity.y = r.velocity_y;
		velocity.z = r.velocity_z;
		radius = r.radius;

		radius_change_speed = 0;
		touch_normal = Vec3( 0, 0, 0 );

		arena_e = ROBOT_ARENA_E;
		mass = ROBOT_MASS;

		id = r.id;
		player_id = r.player_id;
		is_teammate = r.is_teammate;
		nitro_amount = r.nitro_amount;
		touch = r.touch;
	}
};

struct NitroPack : public Entity {
	int id = -1;
	bool alive = false;
	int respawn_ticks = 0;

	inline NitroPack() {
		radius = NITRO_PACK_RADIUS;
		respawn_ticks = NITRO_PACK_RESPAWN_TICKS;
	}
	inline NitroPack( const model::NitroPack & n ) {
		position.x = n.x;
		position.y = n.y;
		position.z = n.z;
		radius = n.radius;

		radius_change_speed = 0;
		touch_normal = Vec3( 0, 0, 0 );

		id = n.id;
		alive = n.alive;
		respawn_ticks = n.respawn_ticks;
	}
};


class Predict {
public:
	int MicroTicksPerTick = MICROTICKS_PER_TICK;
	double DeltaTime2 = TICK_DURATION / MicroTicksPerTick;

	Robot robots[MAX_ROBOTS];
	int numRobots = 0;

	NitroPack nitro_packs[MAX_NITRO_PACKS];
	int numNitroPacks = 0;

	bool NonAxial = true;

public:

	int StartTick = 0;
	int CurrentTick = 0;

	bool UseRobots = true;
	bool UseBall = true;

	bool AutoTicks = false;

	bool UseSimpleCollide = false;

	bool UseFixedHitE = false;
	double FixedHitE = 0.0;

	bool UseEntityCollide = true;

	Ball ball;

	int ScorePlayer1 = 0;
	int ScorePlayer2 = 0;

	int num1Ticks = 0;
	int num100Ticks = 0;

	inline Predict() {
	}

	inline Predict( const model::Game & in ) {
		FromGame( in );
	}

	Predict & operator=( const model::Game & in ) {
		FromGame( in );
		return *this;
	}

	void FromGame( const model::Game & in ) {
		for (const model::Robot & v : in.robots) {
			AddRobot( v );
		}
		for (const model::NitroPack & v : in.nitro_packs) {
			nitro_packs[numNitroPacks] = v;
			numNitroPacks++;
		}
		ScorePlayer1 = in.players[0].score;
		ScorePlayer2 = in.players[1].score;

		ball = Ball( in.ball );

		CurrentTick = in.current_tick;
		StartTick = in.current_tick;
	}

	inline void SetMicroTicks( int num ) {
		if (num == MicroTicksPerTick) return;
		MicroTicksPerTick = num;
		DeltaTime2 = TICK_DURATION / MicroTicksPerTick;
	}

	inline int GetMicroTicks() const {
		return MicroTicksPerTick;
	}

	inline void SetBall( const Ball & b ) {
		ball = b;
	}
	inline const Ball & GetBall() const {
		return ball;
	}
	inline Ball & GetBall() {
		return ball;
	}

	inline Robot & GetRobot( int index ) {
		assert( index < MAX_ROBOTS );
		return robots[index];
	}

	inline const Robot & GetRobot( int index ) const {
		assert( index < MAX_ROBOTS );
		return robots[index];
	}

	inline Robot & AddRobot() {
		assert( numRobots < MAX_ROBOTS );
		Robot & r = robots[numRobots];
		r = Robot();
		numRobots++;
		return r;
	}

	inline int GetNumRobots() const {
		return numRobots;
	}

	inline const NitroPack & GetNitroPack( int index ) const {
		return nitro_packs[index];
	}

	inline int GetNumNitroPacks() const {
		return numNitroPacks;
	}

	inline Robot & AddRobot( const Robot & in ) {
		assert( numRobots < MAX_ROBOTS );
		robots[numRobots] = in;
		Robot & r = robots[numRobots];
		numRobots++;
		return r;
	}

	inline Robot & FindRobot( int id ) {
		for (int i = 0; i < numRobots; i++) {
			if (robots[i].id == id) return robots[i];
		}
		assert( false );
	}

	inline bool IsScoredSelf() const {
		return (ball.position.z > Arena::ad2 + ball.radius);
	}

	inline bool IsScoredEnemy() const {
		return (ball.position.z < -Arena::ad2 - ball.radius);
	}

	inline bool IsScored() const {
		return (abs(ball.position.z) > Arena::ad2 + ball.radius);
	}

	inline static Vec2 GateCenter( bool isEnemy = false ) {
		int sign = (isEnemy) ? 1 : -1;
		return Vec2( 0, (Arena::ad2 - Arena::br ) *sign );
	}

	inline static Plane dan_to_sphere_inner( const Vec3 & InPoint, const Vec3 & InCenter, double Radius ) {
		return Plane( InCenter - InPoint, Radius - (InPoint - InCenter).Length() );
	}

	inline static Plane dan_to_sphere_outer( const Vec3 & InPoint, const Vec3 & InCenter, double Radius ) {
		return Plane( InPoint - InCenter, (InPoint - InCenter).Length() - Radius );
	}

	//inline static Plane dan_to_sphere_inner(Vec3 InPoint, Vec3 InCenter, double Radius) {
	//	return Plane(Vec3(InCenter - InPoint).Normalized(), Radius - (InPoint - InCenter).Length());
	//}

	//inline static Plane dan_to_sphere_outer(Vec3 InPoint, Vec3 InCenter, double Radius) {
	//	return Plane(Vec3(InPoint - InCenter).Normalized(), (InPoint - InCenter).Length() - Radius);
	//}

	inline static Plane dan_to_plane( const Vec3 & p, const Vec3 & pop, const Vec3 & norm ) {
		return Plane( norm, (p - pop).Dot( norm ) );
	}

	inline static const Plane & min( const Plane & a, const Plane & b ) {
		return (a.Dist < b.Dist) ? a : b;
	}


	static Plane dan_to_arena_quarter( const Vec3 & point, bool simple = false ) {
		Vec2 v, o, o2, n, corner_o;
		Plane dan;

		using namespace Arena;

		// Ground
		dan = dan_to_plane( point, Vec3( 0, 0, 0 ), Vec3( 0, 1, 0 ) );

		// Ceiling
		dan = min( dan, dan_to_plane( point, Vec3( 0, ah, 0 ), Vec3( 0, -1, 0 ) ) );

		// Side x
		dan = min( dan, dan_to_plane( point, Vec3( aw2, 0, 0 ), Vec3( -1, 0, 0 ) ) );

		// Side z (goal)
		dan = min( dan, dan_to_plane( point, Vec3( 0, 0, ad2 + gd ), Vec3( 0, 0, -1 ) ) );

		// Side z
		v = Vec2( point.x, point.y ) - Vec2( gw2 - gtr, gh - gtr );
		if (point.x >= gw2 + gsr || point.y >= gh + gsr || (v.x > 0 && v.y > 0 && v.Len2() >= Sqr( gtr + gsr ))) {
			dan = min( dan, dan_to_plane( point, Vec3( 0, 0, ad2 ), Vec3( 0, 0, -1 ) ) );
		}

		// Side x & ceiling (goal)
		if (point.z >= ad2 + gsr) {
			// x
			dan = min( dan, dan_to_plane( point, Vec3( gw2, 0, 0 ), Vec3( -1, 0, 0 ) ) );
			// y
			dan = min( dan, dan_to_plane( point, Vec3( 0, gh, 0 ), Vec3( 0, -1, 0 ) ) );
		}

		if (simple) return dan;

		// Goal back corners
		if (point.z > ad2 + gd - br) {
			dan = min( dan, dan_to_sphere_inner( point, Vec3( clamp( point.x, br - gw2, gw2 - br ), clamp( point.y, br, gh - gtr ), ad2 + gd - br ), br ) );
		}

		// Corner
		if (point.x > aw2 - cr && point.z > ad2 - cr) {
			dan = min( dan, dan_to_sphere_inner( point, Vec3( aw2 - cr, point.y, ad2 - cr ), cr ) );
		}

		// Goal outer corner
		if (point.z < ad2 + gsr) {
			// Side x
			if (point.x < gw2 + gsr) {
				dan = min( dan, dan_to_sphere_outer( point, Vec3( gw2 + gsr, point.y, ad2 + gsr ), gsr ) );
			}
			// Ceiling
			if (point.y < gh + gsr) {
				dan = min( dan, dan_to_sphere_outer( point, Vec3( point.x, gh + gsr, ad2 + gsr ), gsr ) );
			}
			// Top corner
			o = Vec2( gw2 - gtr, gh - gtr );
			v = Vec2( point.x, point.y ) - o;
			if (v.x > 0 && v.y > 0) {
				o = o + v.Normalized() * (gtr + gsr);
				dan = min( dan, dan_to_sphere_outer( point, Vec3( o.x, o.y, ad2 + gsr ), gsr ) );
			}
		}

		// Goal inside top corners
		if (point.z > ad2 + gsr && point.y > gh - gtr) {
			// Side x
			if (point.x > gw2 - gtr) {
				dan = min( dan, dan_to_sphere_inner( point, Vec3( gw2 - gtr, gh - gtr, point.z ), gtr ) );
			}
			// Side z
			if (point.z > ad2 + gd - gtr) {
				dan = min( dan, dan_to_sphere_inner( point, Vec3( point.x, gh - gtr, ad2 + gd - gtr ), gtr ) );
			}
		}

		// Bottom corners
		if (point.y < br) {
			// Side x
			if (point.x > aw2 - br) {
				dan = min( dan, dan_to_sphere_inner( point, Vec3( aw2 - br, br, point.z ), br ) );
			}
			// Side z
			if (point.z > ad2 - br && point.x >= gw2 + gsr) {
				dan = min( dan, dan_to_sphere_inner( point, Vec3( point.x, br, ad2 - br ), br ) );
			}
			// Side z (goal)
			if (point.z > ad2 + gd - br) {
				dan = min( dan, dan_to_sphere_inner( point, Vec3( point.x, br, ad2 + gd - br ), br ) );
			}
			// Goal outer corner
			o = Vec2( gw2 + gsr, ad2 + gsr );
			v = Vec2( point.x, point.z ) - o;
			if (v.x < 0 && v.y < 0 && v.Len2() < Sqr( gsr + br )) {
				o = o + v.Normalized() * (gsr + br);
				dan = min( dan, dan_to_sphere_inner( point, Vec3( o.x, br, o.y ), br ) );
			}
			// Side x (goal)
			if (point.z >= ad2 + gsr && point.x > gw2 - br) {
				dan = min( dan, dan_to_sphere_inner( point, Vec3( gw2 - br, br, point.z ), br ) );
			}
			// corner
			if (point.x > aw2 - cr && point.z > ad2 - cr) {
				corner_o = Vec2( aw2 - cr, ad2 - cr );
				n = Vec2( point.x, point.z ) - corner_o;
				double dist = n.Len2();
				if (dist > Sqr(cr - br)) {
					n = n / sqrt(dist);
					o2 = corner_o + n * (cr - br);
					dan = min( dan, dan_to_sphere_inner( point, Vec3( o2.x, br, o2.y ), br ) );
				}
			}
		}

		// Ceiling corners
		if (point.y > ah - tr) {
			// Side x
			if (point.x > aw2 - tr) {
				dan = min( dan, dan_to_sphere_inner( point, Vec3( aw2 - tr, ah - tr, point.z ), tr ) );
			}
			// Side z
			if (point.z > ad2 - tr) {
				dan = min( dan, dan_to_sphere_inner( point, Vec3( point.x, ah - tr, ad2 - tr ), tr ) );
			}

			Vec2 dv;
			// Corner
			if (point.x > aw2 - cr && point.z > ad2 - cr) {
				corner_o = Vec2( aw2 - cr, ad2 - cr );
				dv = Vec2( point.x, point.z ) - corner_o;
				if (dv.Len2() > Sqr( cr - tr )) {

					n = dv.Normalized();
					o2 = corner_o + n * (cr - tr);
					dan = min( dan, dan_to_sphere_inner( point, Vec3( o2.x, ah - tr, o2.y ), tr ) );
				}
			}
		}

		dan.Normal.Normalize();

		return dan;
	}


	inline static Plane dan_to_arena( Vec3 point, bool simple = false ) {
		double signX = (point.x < 0) ? -1 : 1;
		double signZ = (point.z < 0) ? -1 : 1;

		point.x *= signX;
		point.z *= signZ;
		Plane result = dan_to_arena_quarter( point, simple );
		result.Normal.x *= signX;
		result.Normal.z *= signZ;

		return result;
	}

	bool collide_entities( Entity & a, Entity & b ) {
		if (!UseEntityCollide) return false;
		if (!a.enabled || !b.enabled) return false;

		Vec3 delta_position = b.position - a.position;

		double distance = delta_position.Length();
		double penetration = a.radius + b.radius - distance;

		if (penetration > 0) {
			double d1 = (1 / a.mass);
			double d2 = (1 / b.mass);
			double d1p2inv = 1 / (d1 + d2);
			double k_a = d1 * d1p2inv;
			double k_b = d2 * d1p2inv;
			//double k_a = (1 / a.mass) / ((1 / a.mass) + (1 / b.mass));
			//double k_b = (1 / b.mass) / ((1 / a.mass) + (1 / b.mass));
			Vec3 normal = delta_position.Normalized();
			a.position -= normal * penetration * k_a;
			b.position += normal * penetration * k_b;
			double delta_velocity = normal.Dot( b.velocity - a.velocity ) + b.radius_change_speed - a.radius_change_speed;
			if (delta_velocity < 0) {
				const double hitE = (UseFixedHitE) ? FixedHitE : random( MIN_HIT_E, MAX_HIT_E );
				Vec3 impulse = normal * (1 + hitE) * delta_velocity;
				a.velocity += impulse * k_a;
				b.velocity -= impulse * k_b;
			}
			a.lastCollideRobot = CurrentTick;
			b.lastCollideRobot = CurrentTick;
			NonAxial |= true;
			return true;
		}
		return false;
	}

	bool collide_with_arena( Entity & e ) {
		if (!e.enabled) return false;

		Plane p = dan_to_arena( e.position, UseSimpleCollide );
		e.dist = p.Dist;
		e.touch = false;
		e.touch_normal = p.Normal;

		double penetration = e.radius - p.Dist;
		if (penetration > 0) {
			e.position += p.Normal * penetration;
			double velocity = e.velocity.Dot( p.Normal ) - e.radius_change_speed;
			if (velocity < 0) {
				e.touch = true;
				e.velocity -= p.Normal * (1 + e.arena_e) * velocity;
				e.lastCollideArena = CurrentTick;
				NonAxial |= !UseSimpleCollide && !p.Normal.IsAxis();
				return true;
			}
		}
		return false;
	}

	//inline void move( Entity & e, double delta_time ) const {
	//	if (!e.applyMove || !e.enabled) return;

	//	//e.velocity = e.velocity.Clamped(MAX_ENTITY_SPEED);
	//	assert( e.velocity.Length() <= MAX_ENTITY_SPEED );

	//	e.position += e.velocity * DeltaTime2;
	//	//double d = GRAVITY * delta_time;
	//	e.position.y -= gravityDelta * delta2half;
	//	e.velocity.y -= gravityDelta;
	//}


	static void move( Entity & e, double delta_time ) {
		if (!e.applyMove || !e.enabled) return;

		if (e.velocity.Length() > MAX_ENTITY_SPEED) {
			int b = 0;
		}

		//e.velocity = e.velocity.Clamped(MAX_ENTITY_SPEED);
		//assert( e.velocity.Length() <= MAX_ENTITY_SPEED );

		e.position += e.velocity * delta_time;
		double d = GRAVITY * delta_time;
		e.position.y -= d * delta_time * 0.5;
		e.velocity.y -= d;
	}

	static void ApplyRobotAction( Robot & robot, double delta_time ) {

		if (robot.touch) {
			if (robot.action.target_velocity.Length() > 30+FLT_EPSILON) {
				int b = 0;
			}

			//assert( robot.action.target_velocity.Length() <= ROBOT_MAX_GROUND_SPEED );

			//Vec3 target_velocity = robot.action.target_velocity.Clamped( ROBOT_MAX_GROUND_SPEED );
			Vec3 target_velocity = robot.action.target_velocity;
			target_velocity -= robot.touch_normal * robot.touch_normal.Dot( target_velocity );

			Vec3 target_velocity_change = target_velocity - robot.velocity;
			double d = target_velocity_change.Length();

			if (d > 0) {
				Vec3 n = target_velocity_change / d;
				double acceleration = ROBOT_ACCELERATION * std::max( 0.0, robot.touch_normal.y );
				//Vec3 v = target_velocity_change.Normalized() * acceleration * delta_time;
				Vec3 v = n * acceleration * delta_time;
				//assert( v.Length() <= d );
				robot.velocity += v.Clamped( d );
			}
		}

		if (robot.action.use_nitro) {

			Vec3 target_velocity_change = (robot.action.target_velocity - robot.velocity).Clamped( robot.nitro_amount * NITRO_POINT_VELOCITY_CHANGE );
			double d = target_velocity_change.Length();

			if (d > 0) {
				Vec3 acceleration = target_velocity_change / d * ROBOT_NITRO_ACCELERATION;
				Vec3 velocity_change = (acceleration * delta_time).Clamped( d );
				robot.velocity += velocity_change;
				robot.nitro_amount -= velocity_change.Length() / NITRO_POINT_VELOCITY_CHANGE;
			}
		}

		move( robot, delta_time );

		robot.radius = ROBOT_MIN_RADIUS + (ROBOT_MAX_RADIUS - ROBOT_MIN_RADIUS)	* robot.action.jump_speed / ROBOT_MAX_JUMP_SPEED;
		robot.radius_change_speed = robot.action.jump_speed;
	}

	bool update( double delta_time ) {

		bool hasCollisions = false;

		if (UseRobots) {
			for (int i = 0; i < numRobots; i++) {
				ApplyRobotAction( robots[i], delta_time );
			}
		}

		if (UseBall) {
			move( ball, delta_time );
		}

		if (UseRobots) {
			for (int i = 0; i < numRobots; i++) {
				for (int j = 0; j < i - 1; j++) {
					hasCollisions |= collide_entities( robots[i], robots[j] );
				}
			}
		}

		for (int i = 0; i < numRobots; i++) {
			if (UseBall && UseRobots) {
				if (collide_entities( robots[i], ball )) robots[i].lastHitBall = CurrentTick;
			}
			if (UseRobots) {
				hasCollisions |= collide_with_arena( robots[i] );
			}
		}

		if (UseBall) {
			hasCollisions |= collide_with_arena( ball );
			ball.arena_e = IsScored() ? 0 : BALL_ARENA_E;
		}

		//if (UseRobots) {
		//	for (Robot & robot : robots) {
		//		if (robot.nitro_amount == MAX_NITRO_AMOUNT) continue;
		//		for (NitroPack pack : nitro_packs) {
		//			if (!pack.alive) continue;
		//			if ((robot.position - pack.position).LengthSq() <= Sqr(robot.radius + pack.radius)) {
		//				robot.nitro_amount = MAX_NITRO_AMOUNT;
		//				pack.alive = false;
		//				pack.respawn_ticks = NITRO_PACK_RESPAWN_TICKS;
		//			}
		//		}
		//	}
		//}

		return hasCollisions;
	}


	inline void tickFixed() {
		for (int i = 0; i < MicroTicksPerTick; i++) {
			update( DeltaTime2 );
		}

		if (CurrentTick < StartTick) {
			int b = 0;
		}

		//for (NitroPack pack : nitro_packs) {
		//	if (pack.alive) continue;
		//	pack.respawn_ticks -= 1;
		//	if (pack.respawn_ticks == 0) {
		//		pack.alive = true;
		//	}
		//}
		CurrentTick++;
	}

	bool tickAuto() {
		static Robot robotsLocal[MAX_ROBOTS];
		static Ball ballLocal;

		NonAxial = false;

		if(UseRobots) for (int i = 0; i < numRobots; i++) robotsLocal[i] = robots[i];
		ballLocal = ball;

		//SetMicroTicks( 1 );
		tickFixed();

		if (MicroTicksPerTick == 100) {
			num100Ticks++;
		}
		else {
			num1Ticks++;
		}

		if (NonAxial) {
			CurrentTick--;
			if(UseRobots) for (int i = 0; i < numRobots; i++) robots[i] = robotsLocal[i];
			ball = ballLocal;
			SetMicroTicks( 100 );
			tick();
		}
		else {
			SetMicroTicks( 1 );
		}

		return NonAxial;
	}

	void tick() {
		if (AutoTicks) {
			tickAuto();
		}
		else {
			tickFixed();
		}
	}

	inline void ticks( int num ) {
		if (num == 0) return;
		for (int i = 0; i < num; i++) {
			tick();
		}
	}

};

#endif
