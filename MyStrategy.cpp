#undef _HAS_STD_BYTE

#include <tuple>
#include <algorithm>
#include <deque>

#include "MyStrategy.h"

#include "Helpers.h"
#include "Predict.h"
#ifdef DEBUG
#	include "DebugHelper.h"
#endif

#ifdef DBG
#	undef DBG
#endif
#ifdef DEBUG
#	define DBG(x) x;
#else
#	define DBG(x)
#endif

//#ifndef DEBUG
//#	undef assert(x)
//#	define assert(x)
//#endif

#undef min
#undef max

using namespace std;

DBG( DebugHelper debug );

Predict game;

int MAX_TICK_COUNT = 0;
int TEAM_SIZE = 0;

bool IsNewFrame = false;

int ResetTimeout = 0;
bool AfterReset = false;

bool BallChanged = false;

Timer TotalTime;

typedef vector<Ball> BallTrace;
//
//namespace BallPredict {
//	const int TicksForward = 150;
//
//	bool Changed = false;
//	int Score = 0;
//	int TicksToGoal = -1;
//	bool IsEnemyGoal = false;
//
//	deque<Ball> Trajectory;
//	vector<int> Scores;
//
//	inline const Ball & GetBall( int ticks = 0 ) {
//		assert( ticks < TicksForward );
//		return Trajectory[ticks];
//	}
//
//	inline bool IsChanged() {
//		return Changed;
//	}
//
//	int ScoreFunc( const Predict & pr, int maxTicks = TicksForward, int selfId = -1 ) {
//		int score = 0;
//
//		if (pr.IsScoredEnemy()) {
//			score -= 1000000;
//		}
//		if (pr.IsScoredSelf()) {
//			score += 1000000;
//		}
//
//		score += (int)(pr.ball.position.z / Arena::ad * 100);
//		score += (int)(pr.ball.position.y / Arena::ah * 1000);
//		score += Sqr( pr.ball.velocity.z / MAX_ENTITY_SPEED * 200) * Sign( pr.ball.velocity.z);
//
//		if (pr.ball.position.z > -Arena::ad2 + Arena::cr) score -= 1000;
//
//		double expf = exp( (pr.StartTick - pr.CurrentTick) / (double)maxTicks );
//
//		if (expf > 1) {
//			int b = 0;
//		}
//
//		assert( expf <= 1.0 );
//
//		score = score * expf;
//
//		return score;
//	}
//
//	void ComputeScore() {
//		Predict pr;
//
//		Score = 0;
//		TicksToGoal = -1;
//		
//		Scores.resize( TicksForward );
//
//		int score = 0;
//		int tick = 0;
//
//		for (tick = 0; tick < TicksForward; tick++) {
//			pr.ball = GetBall( tick );
//			pr.CurrentTick = pr.StartTick + tick;
//
//			score += ScoreFunc( pr );
//			Scores[tick] = score / (tick + 1);
//			Score = score;
//
//			if (pr.IsScored()) {
//				TicksToGoal = tick;
//				IsEnemyGoal = pr.IsScoredEnemy();
//				break;
//			}
//		}
//
//		Score /= (tick + 1);
//	}
//
//	void BuildTrajectory() {
//		if (!IsNewFrame) return;
//		if (game.IsScored()) return;
//
//		bool rebuildTrajectory = false;
//
//		if (Trajectory.empty()) {
//			rebuildTrajectory = true;
//		}
//		else {
//			const Ball & b = Trajectory.front();
//
//			DBG( debug.AddSphere( b.position, BALL_RADIUS, "Yellow", 0.5 ) );
//			//BallError += game.ball.position.Dist( b.position );
//
//			double deltaV = (game.ball.velocity - b.velocity).Length();
//			double deltaP = (game.ball.position - b.position).Length();
//			rebuildTrajectory = deltaV > 1 || deltaP > 1;
//		}
//
//		rebuildTrajectory &= (ResetTimeout == 0);
//
//		//DBG( debug.AddParameter( "Error", BallError ) );
//		DBG( debug.AddParameter( "Score", Score ) );
//		DBG( debug.AddParameter( "TicksToGoal", TicksToGoal ) );
//		//DBG( debug.AddParameter( "Changed", LastChangedTick ) );
//
//		Predict pr = game;
//
//		pr.UseRobots = false;
//		pr.SetMicroTicks( 100 );
//
//		if (rebuildTrajectory) {
//			//BallError = 0;
//			
//			Trajectory.clear();
//
//			for (int i = 0; i < TicksForward; i++) {
//				pr.tick();
//				Trajectory.emplace_back( pr.ball );
//			}
//		}
//		else {
//			pr.StartTick = game.CurrentTick + TicksForward;
//			pr.ball = Trajectory.back();
//			Trajectory.pop_front();
//			pr.tick();
//			Trajectory.emplace_back( pr.ball );
//		}
//
//		Changed = rebuildTrajectory;
//
//		ComputeScore();
//
//#ifdef DEBUG
//		for (int i = 0; i < Trajectory.size(); i++) {
//			if (i % 10 != 0) continue;
//			if (i > 80) break;
//			debug.AddSphere( Trajectory[i].position, BALL_RADIUS, "Magenta", 0.2 );
//		}
//#endif
//
//	}
//
//};


inline Vec2 ToVec2( const Vec3 & in ) {
	return Vec2( in.x, in.z );
}

inline Vec3 ToVec3( const Vec2 & in ) {
	return Vec3( in.x, 0, in.y );
}

enum Winding {
	CW = -1, FWD = 0, CCW = 1
};

struct Point {
	Vec2 point2;
	Vec2 dir = Vec2(0,0);
	double height = 0;
	double velocity = 0;

	inline Point() {};
	inline Point( const Entity & e ) {
		SetEntity( e );
	}
	inline Point( const Vec3 & p, const Vec2 & inDir = Vec2( 0, 0 ), double vel = 0 ) : point2( ToVec2( p ) ), height( p.y ), dir( inDir ), velocity( vel ) {};
	inline Point( const Vec2 & p, const Vec2 & inDir = Vec2( 0, 0 ), double vel = 0 ) : point2( p ), dir( inDir ), velocity( vel ) {};

	Point & operator=( const Vec3 & in ) {
		SetPoint( in );
		return *this;
	}

	Vec3 GetVec3() const {
		return Vec3( point2.x, height, point2.y );
	}

	void SetEntity( const Entity & e ) {
		point2 = ToVec2( e.position );
		height = e.position.y;
		if (!e.velocity.IsEpsilon()) {
			dir = ToVec2( e.velocity );
			velocity = dir.Normalize();
		}
		else {
			dir = Vec2( 0, 0 );
			velocity = 0;
		}
	}
	//void AddOffset( Vec3 offset ) {
	//	//point += offset;
	//	point2 += ToVec2( offset );
	//}
	void SetPoint( const Vec3 & in ) {
		point2 = ToVec2( in );
		height = in.y;
	}
	void SetPoint( const Vec2 & in ) {
		point2 = in;
		height = 0;
	}
};

struct Strike : public Point {
	//int id = -1;
	//int prio = -1;
	//bool useNitro = false;
	//int level = 0;

	//bool operator<( const Strike & in ) const {
	//	return prio < in.prio;
	//}
};

struct Path {
	Vec2 initPoint;
	Vec2 initDir;
	double initVel;
	Vec2 rotCenter1;
	double rotRadius1;

	Vec2 finalPoint;
	Vec2 finalDir;
	double finalVel;
	Vec2 rotCenter2;
	double rotRadius2;

	double offVel;
	double reachVel;

	Winding w1, w2;
	Vec2 lineDir;
	double lineVel;
	double lineMoveLen;

	Vec2 lp1, lp2;
	Vec2 accel1, accel2;
	//Vec2 accel3, accel4;
	Line2 lineMove;

	Line2 velLine;
	Line2 velLine2;

	bool useNitro = false;
	mutable double nitroExpense = 0;

	int ticks = -1;
	//double len = -1;
	bool valid = false;

	int reachDiff = 0;
};

struct Probe {

	Strike jump;
	Vec3 impactPos;
	Vec3 impactVel;
	Vec3 ballPos;
	Vec3 ballVel;
	int airTicks = -1;
	int reachTicks = -1;
	int ticks = -1;
	int score = 0;

	double factor = 1;

	bool useNitro = false;
	double nitroExpense = 0;
	int nitroTicks = 0;

	int startTick = -1;
	mutable int dbgGoaTick = -1;

	bool valid = false;

	Path path;

	inline bool operator<( const Probe & in ) const {
		return in.valid && score < in.score;
	}
};


const int STRIKE_LEVELS = 7;
vector<Strike> StrikePoses[STRIKE_LEVELS];

int DbgRebuildId = 0;


Action Quickstart( const Robot & me, const Predict & world, bool isEnemy = false ) {

	Action action;
	const Ball & ball = world.ball;
	Vec3 ball_v = ball.velocity;

	if (!me.touch) {
		action.target_velocity.x = 0.0;
		action.target_velocity.z = 0.0;
		action.target_velocity.y = -MAX_ENTITY_SPEED;
		action.jump_speed = 0.0;
		action.use_nitro = true;
		return action;
	}

	bool jump = (ball.position.Dist( me.position ) < (BALL_RADIUS + ROBOT_MAX_RADIUS) && me.position.y < ball.position.y);

	bool is_attacker = false;
	for (const Robot &robot : world.robots) {
		if (robot.id == -1) break;

		bool teammate = robot.is_teammate ^ isEnemy;
		if (teammate && robot.id != me.id) {
			if (robot.position.z < me.position.z) {
				is_attacker = true;
			}
		}
	}

	if (is_attacker) {

		for (int i = 0; i < 100; ++i) {
			double t = i * 0.1;
			Vec3 ball_pos = ball.position + ball.velocity * t;

			if (ball_pos.z > me.position.z && abs( ball.position.x ) < Arena::aw2 && abs( ball.position.z ) < Arena::ad2) {
				Vec2 delta_pos( ball_pos.x - me.position.x, ball_pos.z - me.position.z );
				double delta_pos_dist = delta_pos.Len();
				double need_speed = delta_pos_dist / t;

				if (0.5 * ROBOT_MAX_GROUND_SPEED < need_speed && need_speed < ROBOT_MAX_GROUND_SPEED) {

					Vec2 target_velocity( delta_pos.Normalized()*need_speed );

					action.target_velocity.x = target_velocity.x;
					action.target_velocity.z = target_velocity.y;
					action.target_velocity.y = 0.0;
					action.jump_speed = jump ? ROBOT_MAX_JUMP_SPEED : 0.0;
					action.use_nitro = false;
					return action;
				}
			}
		}
	}

	Vec2 target_pos( 0.0, -Arena::ad2 + Arena::br );

	if (ball.velocity.z < -FLT_EPSILON) {
		double t = (target_pos.y - ball.position.z) / ball_v.z;
		double x = ball.position.x + ball_v.x * t;
		if (abs( x ) < Arena::gw2) {
			target_pos.x = x;
		}
	}

	Vec2 target_velocity( target_pos.x - me.position.x, target_pos.y - me.position.z );
	//target_velocity = target_velocity * ROBOT_MAX_GROUND_SPEED;

	target_velocity = target_velocity.Rescale( ROBOT_MAX_GROUND_SPEED );

	action.target_velocity.x = target_velocity.x;
	action.target_velocity.z = target_velocity.y;
	action.target_velocity.y = 0.0;
	action.jump_speed = jump ? ROBOT_MAX_JUMP_SPEED : 0.0;
	action.use_nitro = false;

	return action;
}

class Mover;
Mover & GetMover( int id );

class Mover {
public:

	void EnemyAiSimple( Predict & pr ) const {

		for (int i = 0; i < pr.numRobots; i++) {
			Robot & r = pr.robots[i];

			//if (r.is_teammate) continue;

			r.action = Quickstart( r, pr, true );
		}
		pr.tickAuto();
	}

	enum Role {
		NONE,
		ATTACKER,
		GOALKEEPER
	};

	struct NitroOpts {
		double amount = 0;

		bool nitroRun = false;
		bool nitroJump = false;

		double jumpMinHeight = MAX_JUMP_HEIGHT;
		double jumpFactor = 1;

		NitroOpts( bool use = false ) { assert( use == false );  };
	};

	struct Score {
		int score = 0;
		int ticks = 0;
		int goalRes = 0;
		int goalTick = -1;
	};

	static Vec3 NormalOrZero( const Vec3 & in ) {
		double l = in.Length();
		if (l <= FLT_EPSILON) {
			return Vec3( 0, 0, 0 );
		}
		else {
			return in / l;
		}
	}

	static Vec2 NormalOrZero( const Vec2 & in ) {
		double l = in.Len();
		if (l <= FLT_EPSILON) {
			return Vec2( 0, 0 );
		}
		else {
			return in / l;
		}
	}

	static void InitStrikePoses( vector<Strike> & poses, int numH, int numV ) {
		//static const double velList[] = { 0, 8, 15, 20, 25, 30 };
		static const double velList[] = { 0, 8, 20, 30 };
		//static const double velList[] = { 30 };
		static vector<Vec3> dirs;

		dirs.clear();
		dirs.reserve( numH * numV );

		GenSphereVectors2( numH, numV, dirs );

		poses.reserve( dirs.size() * 5 );

		const bool useSideStrikes= false;
		const bool useBackStrikes = false;

		Strike sp;

		int id = 0;

		for (const Vec3 & d : dirs) {
			sp.SetPoint( -d * (ROBOT_RADIUS + BALL_RADIUS) * 1 );

			if (d.Dot( Vec3( 0, 1, 0 ) ) > 0.7) continue;

			for (const double len : velList) {

				sp.velocity = len;

				if (useSideStrikes) {
					Vec2 dir = ToVec2( d ).Normalized();
					Vec2 dirs[2];

					dirs[0] = dir.Lerped( 0.5, dir.Perp() );
					dirs[1] = dir.Lerped( 0.5, -dir.Perp() );

					for (int i = 0; i < 5; i++) {

						sp.dir = dirs[0].Lerped( i, dirs[1] ).Normalized();

						poses.emplace_back( sp );

						if (useBackStrikes) {
							sp.dir = -sp.dir;

							poses.emplace_back( sp );
						}
					}
				}
				else {
					sp.dir = NormalOrZero( ToVec2( d ) );

					poses.emplace_back( sp );

					if (useBackStrikes) {
						sp.dir = -sp.dir;

						poses.emplace_back( sp );
					}

					//AddStrikePose( sp );
				}

			}
		}
	}

	static void InitMovers() {
		InitStrikePoses( StrikePoses[0], 8, 4 );
		InitStrikePoses( StrikePoses[1], 12, 10 );
		InitStrikePoses( StrikePoses[2], 16, 8 );
		InitStrikePoses( StrikePoses[3], 22, 10 );
		InitStrikePoses( StrikePoses[4], 30, 12 );
		InitStrikePoses( StrikePoses[5], 36, 18 );
		InitStrikePoses( StrikePoses[6], 36, 20 );
	}

	Robot Self;
	Robot Ghost;
	Probe Best;
	Role GameRole = NONE;
	deque<Action> Actions;

	inline bool IsGoalkeeper() const {
		return GameRole == GOALKEEPER;
	}

	inline bool IsEnemy() const {
		return !Self.is_teammate;
	}

	inline int GetRank( bool useEnemy = false, bool useOwn = true ) const {
		int rank = 0;
		for (int i = 0; i < game.GetNumRobots(); i++) {
			const Robot & r = game.GetRobot( i );
			if (!useEnemy && !r.is_teammate) continue;
			if (!useOwn && r.is_teammate) continue;
			if (r.id == Self.id) continue;
			if (r.position.z >= Self.position.z) rank++;
		}
		return rank;
	}

	inline static double TicksToTime( int ticks ) {
		return ticks * TICK_DURATION;
	}

	inline static int TicksToTravel( const Vec2 & from, const Vec2 & to, double speed = ROBOT_MAX_GROUND_SPEED ) {
		return from.Dist( to ) / speed * TICKS_PER_SECOND;
	}

	inline static double ROBOT_ACCEL( bool useNitro = false ) {
		return (useNitro) ? ROBOT_ACCELERATION + ROBOT_NITRO_ACCELERATION : ROBOT_ACCELERATION;
	}

	static double TimeToReachHeight( double jumpVel, double height, bool second = false ) {
		double d = Sqr( jumpVel ) - (2 * GRAVITY * height);
		if (d < 0) return -1;
		const double sign = (!second) ? -1 : 1;
		double t = (jumpVel + sign * sqrt( d )) / (GRAVITY);
		return t;
	}

	static int TicksToReachHeight( double jumpVel, double height, bool second = false ) {
		//double d = Sqr( jumpVel ) - (2 * GRAVITY * height);
		//if (d < 0) return -1;
		//const double sign = (!second) ? -1 : 1;
		//double t = (jumpVel + sign * sqrt( d )) / (GRAVITY);
		//return (int)(t * TICKS_PER_SECOND);
		return (int)(TimeToReachHeight(jumpVel,height,second) * TICKS_PER_SECOND);
	}

	enum {
		TG_CROSS_LEFT = 0,
		TG_CROSS_RIGHT = 1,
		TG_SIDE_LEFT = 2,
		TG_SIDE_RIGHT = 3
	};

	// 0 - cross left 1 - cross right 2 - side left 3 - side right
	static Line2 GetTangentLine( const Vec2 & p1, double r1, const Vec2 & p2, double r2, int type ) {

		int k = 1;

		if (type == 0) r1 = -r1;
		if (type == 1) r2 = -r2;
		if (type == 2) k = 1;
		if (type == 3) k = -1;

		Vec2 delta = p2 - p1;
		double len = delta.Len();
		double dr = r2 - r1;

		double X = delta.x / len;
		double Y = delta.y / len;
		double R = dr / len;

		//a = RX − kY√( 1 − R2 )
		//b = RY + kX√( 1 − R2 )
		//c = r1 − (ax1 + by1)

		double a, b, c;

		double root = sqrt( 1 - Sqr( R ) );

		if (isnan( root )) return Line2( 0, 0, 0 );

		a = R * X - k * Y * root;
		b = R * Y + k * X * root;
		c = r1 - (a * p1.x + b * p1.y);

		return Line2( a, b, c );
	}

	inline static double GetRotateRadius( double velocity, bool useNitro = false ) {
		return Sqr( velocity ) / ROBOT_ACCEL(useNitro);
	}

	inline static double GetAccelLength( double vel1, double vel2, bool useNitro = false ) {
		double a = abs( vel2 - vel1 ) / ROBOT_ACCEL(useNitro);
		return ( abs(vel2) + abs(vel1) ) * 0.5 * a;
	}

	inline static int GetAccelTicks( double vel1, double vel2, bool useNitro = false ) {
		return round( abs( vel1 - vel2 ) / ROBOT_ACCEL(useNitro) * TICKS_PER_SECOND );
	}

	inline static double GetNitroExpense( double time ) {
		return (time * ROBOT_NITRO_ACCELERATION) / NITRO_POINT_VELOCITY_CHANGE;
	}

	inline static double GetNitroExpense( double vel1, double vel2 ) {
		double time = abs(vel2 - vel1) / (ROBOT_ACCELERATION + ROBOT_NITRO_ACCELERATION);
		return (time * ROBOT_NITRO_ACCELERATION) / NITRO_POINT_VELOCITY_CHANGE;
	}

	inline static double GetNitroExpenseV( double height ) {
		double time = height / ROBOT_MAX_JUMP_SPEED;
		return (time * ROBOT_NITRO_ACCELERATION) / NITRO_POINT_VELOCITY_CHANGE;
	}

	inline static double GetMaxNitroJump( double nitroAmount ) {
		double time = (nitroAmount * ROBOT_NITRO_ACCELERATION) / NITRO_POINT_VELOCITY_CHANGE;
		return time * ROBOT_MAX_JUMP_SPEED;
	}

	inline static double GetNitroExpenseArc( const Vec2 & from, const Vec2 & to, Winding w ) {

	}

	inline void AddAction( const Action & act, int num = 1 ) {
		while (num > 0) {
			Actions.emplace_back( act );
			num--;
		}
	}

	inline void ClearActions() {
		Actions.clear();
	}

	inline Action PopAction() {
		if (Actions.empty()) return Action();
		Action a = Actions.front();
		Actions.pop_front();
		return a;
	}

	inline bool IsActionsEmpty() const {
		return Actions.empty();
	}

	inline void CropActions( int ticks ) {
		Actions.resize( min( ticks, (int)Actions.size() ) );
	}

	int AddRotationQ( double velocity, const Vec2 & from, const Vec2 & to, bool useNitro, bool estimate ) {
		const double rotStep = ROBOT_ACCEL(useNitro) * TICK_DURATION;
		double ang = abs( from.Angle( to ) );
		double stepAng = 2 * asin( rotStep / (2 * velocity) );
		double slerpStep = stepAng / ang;
		int rotTicks = round( ang / stepAng );

		if (isnan( stepAng )) {
			return 0;
		}

		if (!estimate) {
			if (rotTicks == 0) return 0;

			Vec2 v;

			for (int i = 1; i <= rotTicks; i++) {
				v = from.Slerped( clamp( i * slerpStep, 0.0, 1.0 ), to ) * velocity;

				AddAction( Action( ToVec3( v ), 0, useNitro ) );
			}

		}

		return rotTicks;
	}

	int AddRotation( double velocity, const Vec2 & from, const Vec2 & to, bool useNitro, Winding w, bool estimate = false ) {
		double d = from.Dot( to );

		//if (abs( d ) <= FLT_EPSILON) return 0;
		if (abs( d - 1 ) <= FLT_EPSILON) return 0;
		if (w == FWD) return 0;

		Line2 ln;
		ln.FromOrigin( from );

		int mod = (int)w;

		int side = ln.GetSign( to );
		side *= mod;

		Vec2 p1 = from.Perp() * mod;
		Vec2 p2 = p1.Perp() * mod;
		Vec2 p3 = p2.Perp() * mod;

		int ticks = 0;

		if (side == 1) {
			if (d > 0) {
				ticks += AddRotationQ( velocity, from, to, useNitro, estimate );
			}
			else {
				ticks += AddRotationQ( velocity, from, p1, useNitro, estimate );
				ticks += AddRotationQ( velocity, p1, to, useNitro, estimate );
			}
		}
		else {
			if (d < 0) {
				ticks += AddRotationQ( velocity, from, p1, useNitro, estimate );
				ticks += AddRotationQ( velocity, p1, p2, useNitro, estimate );
				ticks += AddRotationQ( velocity, p2, to, useNitro, estimate );
			}
			else {
				ticks += AddRotationQ( velocity, from, p1, useNitro, estimate );
				ticks += AddRotationQ( velocity, p1, p2, useNitro, estimate );
				ticks += AddRotationQ( velocity, p2, p3, useNitro, estimate );
				ticks += AddRotationQ( velocity, p3, to, useNitro, estimate );
			}
		}

		return ticks;
	}

	int AddAccel( double initVelocity, double velocity, bool useNitro, const Vec2 & dir, bool estimate = false ) {
		int accelTicks = round( abs(velocity-initVelocity) / ROBOT_ACCEL(useNitro) * TICKS_PER_SECOND );
		if (!estimate) {
			AddAction( Action( Vec3( dir.x, 0, dir.y ) * velocity, 0, useNitro ), accelTicks );
		}
		return accelTicks;
	}

	int AddLineMove( double velocity, double len, const Vec2 & dir, bool estimate = false ) {
		//if (velocity <= FLT_EPSILON) return 0;
		assert( velocity > 0 );
		int moveTicks = round(len / velocity * TICKS_PER_SECOND);
		if (!estimate) {
			AddAction( Action( Vec3( dir.x, 0, dir.y )*velocity ), moveTicks );
		}
		return moveTicks;
	}

	int AddJump( double velocity, const Vec2 & dir, double power, int ticks, bool useNitro, bool estimate = false ) {
		if (!estimate) {
			AddAction( Action( ToVec3( dir * velocity ).SetY( (useNitro) ? GRAVITY : 0 ), power, useNitro ), ticks );
		}
		return ticks;
	}

	int AddWaitTicks( int num, bool estimate = false ) {
		if (!estimate) {
			AddAction( Action(), num );
		}
		return num;
	}

	int BuildPath( const Path & path, bool estimate = false ) {
		int ticks = 0;

		int rt1, rt2, lm, at1, at2, at3, at4;

		assert( path.valid || estimate );

		ticks += at1 = AddAccel( path.initVel, path.offVel, path.useNitro, path.initDir, estimate );
		ticks += rt1 = AddRotation( path.offVel, path.initDir, path.lineDir, path.useNitro, path.w1, estimate );
		ticks += at2 = AddAccel( path.offVel, path.lineVel, path.useNitro, path.lineDir, estimate );
		ticks += lm = AddLineMove( path.lineVel, path.lineMoveLen, path.lineDir, estimate );
		ticks += at3 = AddAccel( path.lineVel, path.reachVel, path.useNitro, path.lineDir, estimate );
		ticks += rt2 = AddRotation( path.reachVel, path.lineDir, path.finalDir, path.useNitro, path.w2, estimate );
		ticks += at4 = AddAccel( path.reachVel, path.finalVel, path.useNitro, path.finalDir, estimate );

		if (path.useNitro) {
			path.nitroExpense = 0;
			path.nitroExpense += GetNitroExpense( path.initVel, path.offVel );
			path.nitroExpense += GetNitroExpense( path.reachVel, path.finalVel );
			path.nitroExpense += GetNitroExpense( path.offVel, path.lineVel );
			path.nitroExpense += GetNitroExpense( path.lineVel, path.reachVel );
			path.nitroExpense += GetNitroExpense( rt1 * TICK_DURATION );
			path.nitroExpense += GetNitroExpense( rt2 * TICK_DURATION );
		}

		return ticks;
	}

	int GenPathTo( const Point & initp, double offVel, double reachVel, double lineVel, Winding w1, Winding w2, bool useNitro, const Point & finalp, Path & path ) {

		path.valid = false;
		path.ticks = -1;

		path.useNitro = useNitro;
		path.nitroExpense = 0;

		path.initPoint = initp.point2;
		path.initDir = initp.dir;
		path.initVel = initp.velocity;
		path.offVel = offVel;
		path.reachVel = reachVel;
		path.lineVel = lineVel;
		path.finalPoint = finalp.point2;
		path.finalDir = finalp.dir;
		path.finalVel = finalp.velocity;

		bool initVelZero = IsEpsilon( path.initVel );
		bool finalVelZero = IsEpsilon( path.finalVel );

		if (initVelZero) path.initDir = Vec2( 0, 0 );
		if (finalVelZero) path.finalDir = Vec2( 0, 0 );

		path.velLine.FromRay( path.initPoint, path.initDir );
		path.velLine2.FromRay( path.finalPoint, path.finalDir );

		double accelLen1 = GetAccelLength( path.initVel, path.offVel, useNitro );
		double accelLen2 = GetAccelLength( path.reachVel, path.finalVel, useNitro );

		//if (useNitro) {
		//	path.nitroExpense += GetNitroExpense( path.initVel, path.offVel );
		//	path.nitroExpense += GetNitroExpense( path.reachVel, path.finalVel );
		//}

		path.accel1 = path.initPoint + path.initDir * accelLen1;
		path.accel2 = path.finalPoint - path.finalDir * accelLen2;

		path.rotRadius1 = GetRotateRadius( offVel, useNitro );
		path.rotRadius2 = GetRotateRadius( reachVel, useNitro );

		if (initVelZero) path.rotRadius1 = 0;
		if (finalVelZero) path.rotRadius2 = 0;

		path.w1 = w1;
		path.w2 = w2;

		path.rotCenter1 = path.accel1 + path.initDir.Perp() * path.rotRadius1 * (int)path.w1;
		path.rotCenter2 = path.accel2 + path.finalDir.Perp() * path.rotRadius2 * (int)path.w2;

		if (initVelZero) path.rotCenter1 = path.accel1;
		if (finalVelZero) path.rotCenter2 = path.accel2;

		int type = -1;
		if (path.w1 != path.w2) {
			type = (path.w2 == CCW) ? 0 : 1;
		}
		else {
			type = (path.w2 == CCW) ? 2 : 3;
		}

		path.lineMove = GetTangentLine( path.rotCenter1, path.rotRadius1, path.rotCenter2, path.rotRadius2, type );

		if (path.lineMove.IsEpsilon()) return -1;

		path.lp1 = path.lineMove.Project( path.rotCenter1 );
		path.lp2 = path.lineMove.Project( path.rotCenter2 );
		path.lineMoveLen = path.lp1.Dist( path.lp2 );
		path.lineDir = (path.lp2 - path.lp1).Normalized();

		if (path.initDir.IsEpsilon()) {
			path.initDir = path.lineDir;
			path.lineMoveLen -= accelLen1;
		}
		if (path.finalDir.IsEpsilon()) path.finalDir = path.lineDir;

		path.lineMoveLen -= GetAccelLength( path.offVel, path.lineVel, useNitro );
		path.lineMoveLen -= GetAccelLength( path.lineVel, path.reachVel, useNitro );

		//if (useNitro) {
		//	path.nitroExpense += GetNitroExpense( path.offVel, path.lineVel );
		//	path.nitroExpense += GetNitroExpense( path.lineVel, path.reachVel );
		//}

		if (path.lineMoveLen < 0) return -1;

		path.valid = true;
		path.ticks = BuildPath( path, true );

		if (path.ticks < 0) {
			int b = 0;
		}

		return path.ticks;
	}

	int GenPathTo( const Point & initp, double offVel, double reachVel, double lineVel, bool useNitro, const Point & finalp, Path & path ) {
		Path path2;
		GenPathTo( initp, offVel, reachVel, lineVel, CCW, CCW, useNitro, finalp, path );
		GenPathTo( initp, offVel, reachVel, lineVel, CCW, CW, useNitro, finalp, path2 );
		if (path.ticks > path2.ticks) path = path2;
		GenPathTo( initp, offVel, reachVel, lineVel, CW, CCW, useNitro, finalp, path2 );
		if (path.ticks > path2.ticks) path = path2;
		GenPathTo( initp, offVel, reachVel, lineVel, CW, CW, useNitro, finalp, path2 );
		if (path.ticks > path2.ticks) path = path2;
		return path.ticks;
	}

	inline static double DistToBorder( Vec2 point ) {
		double dist = 1000;

		if (point.x < 0) point.x = -point.x;
		if (point.y < 0) point.y = -point.y;


		if (point.y > Arena::ad2) {
			dist = min( dist, Arena::gw2 - point.x );
			dist = min( dist, Arena::ad2 + Arena::gd - point.y );
		}
		else {
			if (point.y > Arena::ad2 - Arena::cr && point.x < Arena::gw2) {
				Vec2 corner2( Arena::gw2 + Arena::gsr, Arena::ad2 + Arena::gsr );
				dist = min( dist, point.Dist( corner2 ) - Arena::gsr );
			}
			else {
				if (point.y > Arena::ad2 - Arena::cr && point.x > Arena::aw2 - Arena::cr) {
					Vec2 corner( Arena::ad2 - Arena::cr, Arena::aw2 - Arena::cr );
					dist = min( dist, Arena::cr - point.Dist( corner ) );
				}
				else {
					dist = min( dist, Arena::ad2 - point.y);
					dist = min( dist, Arena::aw2 - point.x);
				}
			}
		}

		return dist-ROBOT_RADIUS-Arena::br;
	}

	inline static double GetVelocityForRadius( double radius, bool useNitro = false ) {
		return sqrt( radius * ROBOT_ACCEL(useNitro) );
	}

	int GenOptimalPathTo( const Point & initp, const Point & finalp, int reachTicks, const NitroOpts & nitroOpts, Path & path ) {

		double offVel = clamp( initp.velocity, 0.0, ROBOT_MAX_GROUND_SPEED );
		double reachVel = clamp( finalp.velocity, 0.0, ROBOT_MAX_GROUND_SPEED );
		double lineVel = ROBOT_MAX_GROUND_SPEED;

		path.reachDiff = 0;

		bool useNitro = nitroOpts.nitroRun && nitroOpts.amount > 0;

		GenPathTo( initp, offVel, reachVel, lineVel, useNitro, finalp, path );

		Winding w1 = path.w1;
		Winding w2 = path.w2;

		double dist1 = DistToBorder( path.rotCenter1 );
		double dist2 = DistToBorder( path.rotCenter2 );

		if (dist1 <= 0 || dist2 <= 0) {
			return -1;
		}

		offVel = min( offVel, GetVelocityForRadius( dist1 ) );
		reachVel = min( reachVel, GetVelocityForRadius( dist2 ) );

		double dot = path.lineDir.Dot( path.finalDir );
		if (dot < 0 && path.finalVel > 0) reachVel = Lerp( abs( dot ), reachVel, max(reachVel - 15,8.0) );	

		dot = path.lineDir.Dot( path.initDir );
		if (dot < 0 && path.initVel > 0) offVel = Lerp( abs( dot ), offVel, max(offVel - 15,8.0) );

		GenPathTo( initp, offVel, reachVel, lineVel, path.w1, path.w2, useNitro, finalp, path );
		//GenPathTo( initp, offVel, reachVel, lineVel, useNitro, finalp, path );

		double lineStep = 10.0;
		int diff = 1000;
		int maxIters = 12;

		while (DistToBorder( path.rotCenter1 ) < path.rotRadius1) {
			offVel *= 0.8;
			GenPathTo( initp, offVel, reachVel, lineVel, path.w1, path.w2, useNitro, finalp, path );
			//GenPathTo( initp, offVel, reachVel, lineVel, useNitro, finalp, path );
			maxIters--;
			if (maxIters <= 0) break;
		}

		while (DistToBorder( path.rotCenter2 ) < path.rotRadius2) {
			reachVel *= 0.8;
			GenPathTo( initp, offVel, reachVel, lineVel, path.w1, path.w2, useNitro, finalp, path );
			//GenPathTo( initp, offVel, reachVel, lineVel, useNitro, finalp, path );
			maxIters--;
			if (maxIters <= 0) break;
		}
		
		while (!path.valid && (path.lineMoveLen < 0 || path.lineMove.IsEpsilon()) ) {
			offVel *= 0.8;
			reachVel *= 0.8;
			lineVel *= 0.8;
			GenPathTo( initp, offVel, reachVel, lineVel, path.w1, path.w2, useNitro, finalp, path );
			//GenPathTo( initp, offVel, reachVel, lineVel, useNitro, finalp, path );
			maxIters--;
			if (maxIters <= 0) break;
		}

		if (useNitro && path.nitroExpense > nitroOpts.amount) {
			GenPathTo( initp, offVel, reachVel, lineVel, false, finalp, path );
		}

		dist1 = DistToBorder( path.rotCenter1 );
		dist2 = DistToBorder( path.rotCenter2 );

		if (dist1 <= 0 || dist2 <= 0) {
			return -1;
		}

		if (path.lineMove.IsEpsilon()) {
			int b = 0;
		}

		//double d1 = Predict::dan_to_arena( ToVec3( path.rotCenter1 ) ).Dist;
		//double d2 = Predict::dan_to_arena( ToVec3( path.rotCenter2 ) ).Dist;
		//if (d1 < -3 || d2 < -3) {
		//	int b = 0;
		//}

		int minTicks = path.ticks;

		if (reachTicks == -1) {
			return path.ticks;
		}

		maxIters = max( maxIters, 12 );

		while (abs(diff) > 0) {
			GenPathTo( initp, offVel, reachVel, lineVel, path.w1, path.w2, useNitro, finalp, path );
			//GenPathTo( initp, offVel, reachVel, lineVel, useNitro, finalp, path );
			diff = reachTicks - path.ticks;
			if (diff < 0) {
				lineStep *= 0.5;
				lineVel += lineStep;
			}
			else {
				lineVel -= lineStep;
			}
			maxIters--;
			if (maxIters <= 0) break;
			if (lineVel <= 0 || lineVel > ROBOT_MAX_GROUND_SPEED) break;
		}

		path.reachDiff = path.ticks - minTicks;

		return path.ticks;
	}

	bool GenProbe( const Ball & b, const Strike & strike, const NitroOpts & nitroOpts, Probe & p ) {
		Vec3 point = strike.GetVec3() + b.position;

		if (point.y < ROBOT_RADIUS && point.y > ROBOT_RADIUS*0.5) {
			point.y = ROBOT_RADIUS;
		}

		if (point.y < ROBOT_RADIUS-FLT_EPSILON) return false;

		double nitroExpense = 0;
		double nitroHeight = 0;

		double jumpTime = -1;

		bool useNitro = nitroOpts.nitroJump && nitroOpts.amount > 0;

		if (useNitro) {
			nitroHeight = point.y - ROBOT_RADIUS;
			nitroHeight *= nitroOpts.jumpFactor;
			nitroExpense = GetNitroExpenseV( nitroHeight );

			if (nitroExpense > nitroOpts.amount) {
				nitroHeight *= nitroOpts.amount / nitroExpense;
				nitroExpense = nitroOpts.amount;
			}
			jumpTime = TimeToReachHeight( ROBOT_MAX_JUMP_SPEED, point.y - nitroHeight - ROBOT_RADIUS );
		}
		else {
			jumpTime = TimeToReachHeight( ROBOT_MAX_JUMP_SPEED, point.y - ROBOT_RADIUS );
		}

		if (jumpTime < 0) return false;

		int jumpTicks = jumpTime * TICKS_PER_SECOND;

		double nitroTime = nitroHeight / ROBOT_MAX_JUMP_SPEED;
		int nitroTicks = nitroTime * TICKS_PER_SECOND;

		int airTicks = jumpTicks + nitroTicks;
		double airTime = jumpTime + nitroTime;

		double jumpLen = strike.velocity * airTime;

		Vec2 jumpOffs = ToVec2( point ) - strike.dir * jumpLen;

		Vec3 impactVel = ToVec3( strike.dir * strike.velocity );
		impactVel.y = ROBOT_MAX_JUMP_SPEED - GRAVITY * jumpTime;

		p.ticks = airTicks;
		p.airTicks = airTicks;
		p.jump.point2 = jumpOffs;
		p.jump.dir = strike.dir;
		p.jump.velocity = strike.velocity;
		p.impactPos = point;
		p.impactVel = impactVel;
		p.ballPos = b.position;
		p.ballVel = b.velocity;

		p.useNitro = nitroOpts.nitroJump;
		p.nitroExpense = nitroExpense;
		p.nitroTicks = nitroTicks;

		return true;
	}

	enum {
		HIT_E_RANDOM,
		HIT_E_AVERAGE,
		HIT_E_MIN,
		HIT_E_MAX
	};

	int TestProbe( const Probe & probe, int maxSimTicks, bool simple = false, int hitEType = HIT_E_AVERAGE) {

		static BallTrace trace;
		trace.clear();

		Predict pr;
		Robot & r = pr.AddRobot();

		pr.GetBall().SetPosition( probe.ballPos, probe.ballVel );

		int simTicks = 0;

		pr.SetMicroTicks( 100 );

		int score = 0;

		pr.StartTick = game.CurrentTick;
		pr.CurrentTick = pr.StartTick + probe.reachTicks;

		pr.UseFixedHitE = true;
		if (hitEType == HIT_E_RANDOM) pr.UseFixedHitE = false;
		if (hitEType == HIT_E_AVERAGE) pr.FixedHitE = (MIN_HIT_E + MAX_HIT_E) * 0.5;
		if (hitEType == HIT_E_MIN) pr.FixedHitE = MIN_HIT_E;
		if (hitEType == HIT_E_MAX) pr.FixedHitE = MAX_HIT_E;

		assert( pr.UseFixedHitE == false || pr.FixedHitE > 0 );

		pr.UseSimpleCollide = simple;

		bool useRobots = false;

		if (useRobots) {
			for (int i = 0; i < game.GetNumRobots(); i++) {
				const Robot & r = game.GetRobot( i );
				if (r.id == Self.id) continue;
				pr.AddRobot( r ).enabled = false;
			}
		}

		simTicks = maxSimTicks;
		int maxTicks = simTicks;

		r.position = probe.impactPos;
		r.velocity = probe.impactVel;
		pr.CurrentTick = pr.StartTick + probe.ticks;

		pr.SetMicroTicks( 100 );

		r.action.target_velocity = ToVec3( probe.jump.dir * probe.jump.velocity );
		r.action.jump_speed = ROBOT_MAX_JUMP_SPEED;

		while (r.lastHitBall == -1 && simTicks > maxTicks - 3) {
			pr.tick();
			simTicks--;
		}

		//double d = r.position.Dist( pr.GetBall().position );
		//double v = r.velocity.Length();
		//double v2 = probe.impactVel.Length();
		//double v3 = probe.ballVel.Length();
		//double dot = r.velocity.DotNorm( pr.ball.velocity );
		//double ballVel = pr.ball.velocity.Length();

		//if (r.lastHitBall == -1 && (v2 >= v3) && (probe.impactVel.x > 3 || probe.impactVel.z > 3) ) {
		//	int b = 0;
		//}
		if (r.lastHitBall == -1) {
			return -10000000;
		}

		r.enabled = false;
		pr.UseRobots = false;

		pr.SetMicroTicks( 100 );

		pr.StartTick = pr.CurrentTick;

		//if(UseBallTrace) BallTrace.clear();

		while (simTicks > 0) {
			//score += BallPredict::ScoreFunc( pr, maxTicks );
			//score += ScoreFunc( pr.ball, maxTicks - simTicks, maxTicks );

			trace.emplace_back( pr.ball );

			if (pr.IsScored()) {
				break;
			}

			//if(UseBallTrace) BallTrace.emplace_back( pr.ball );

			pr.tickAuto();

			simTicks--;
		}
		DBG( probe.dbgGoaTick = maxTicks - simTicks );

		score = ComputeScore( trace, !simple, probe.ticks ).score;

		//score /= max( 1, maxTicks - simTicks );

		//UseBallTrace = false;

		return score;
	}

	inline bool TryAddProbe( const Ball & b, const Point & robot, const Strike & strike, int tick, NitroOpts nitroOpts, Probe & p ) {

		if (!GenProbe( b, strike, nitroOpts, p )) return false;

		if (TicksToTravel( robot.point2, p.jump.point2 ) > tick) return false;
		if (DistToBorder( p.jump.point2 ) < 0) return false;

		int reachTicks = tick - p.airTicks - 1;
		if (reachTicks < 1) return false;

		nitroOpts.amount -= p.nitroExpense;

		int ticks = GenOptimalPathTo( robot, p.jump, -1, nitroOpts, p.path );
		 
		if (ticks == -1) return false;

		int diff = abs( ticks - reachTicks );

		if (diff > 2) return false;

		if (p.path.reachDiff > 10) {
			return false;
		}

		p.reachTicks = ticks;
		p.ticks = p.airTicks + p.reachTicks;

		p.valid = true;
		p.startTick = game.CurrentTick + p.path.ticks;

		return true;
	}

	int AddProbesForTick( const Ball & b, int tick, vector<Probe> & probes, NitroOpts nitroOpts, int strikeId = 0 ) {

		//const Ball & b = BallPredict::GetBall( tick );

		int numProbes = 0;

		bool remove = false;

		if (TicksToTravel( ToVec2( Self.position ), ToVec2( b.position ) ) > tick) {
			return 0;
		}

		if (nitroOpts.nitroJump && b.position.y - BALL_RADIUS < nitroOpts.jumpMinHeight) {
			nitroOpts.nitroJump = false;
		}

		if( !nitroOpts.nitroJump && b.position.y - BALL_RADIUS > MAX_JUMP_HEIGHT) {
			return 0;
		}

		assert( strikeId < STRIKE_LEVELS );

		for (const Strike & strike : StrikePoses[strikeId]) {

			if (!remove) probes.emplace_back();
			Probe & p = probes.back();

			remove = !TryAddProbe( b, Self, strike, tick, nitroOpts, p );

			if (!remove) numProbes++;
		}

		if (remove && !probes.empty()) probes.pop_back();

		return numProbes;
	}

	int FillProbes( const BallTrace & trace, vector<Probe> & probes, int step, int tickLimit, const NitroOpts & nitroOpts, int strikeId = 0 ) {
		int numProbes = 0;
		int tick = 0;

		tickLimit = min( tickLimit, (int)trace.size()-1 );

		if (tickLimit == 0) return 0;

		while (tick < tickLimit) {
			//const Ball & b = BallPredict::GetBall( tick );
			const Ball & b = trace[tick];

			int num = AddProbesForTick( b, tick, probes, nitroOpts, strikeId );
			numProbes += num;

			tick += step;
		}

		return numProbes;
	}

	void BuildProbe( const Probe & probe ) {
		BuildPath( probe.path );
		AddJump( probe.jump.velocity, probe.jump.dir, ROBOT_MAX_JUMP_SPEED, probe.nitroTicks, true );
		AddJump( probe.jump.velocity, probe.jump.dir, ROBOT_MAX_JUMP_SPEED, probe.airTicks-probe.nitroTicks+5, false );
	}

	bool IsSafe() const {
		bool safe = true;
		const Ball & b = game.ball;
		safe &= b.position.z > 0;
		safe &= b.velocity.z > 0;
		//safe &= BallPredict::TicksToGoal == -1 || !BallPredict::IsEnemyGoal;
		return safe;
	}

	bool IsDanger() const {
		return GetBallScore().goalTick != -1 && GetBallScore().goalTick < 80 && GetBallScore().score < 0;
	}

	inline static Vec2 GateCenter( bool isEnemy = false ) {
		int sign = (isEnemy) ? 1 : -1;
		return Vec2( 0, (Arena::ad2 - Arena::br) *sign );
	}

	static Vec2 GetGateDir( const Vec2 & pos, bool isEnemy = false ) {
		return (GateCenter( !isEnemy ) - pos).Normalized();
	}

	bool GoalKeeperMove() {

		if (GameRole != GOALKEEPER) return false;

		Point dest;
		dest.point2 = GetGoalkeeperPos();
		dest.velocity = 0;

		Path path;

		GenOptimalPathTo( Self, dest, -1, false, path );

		if (!path.valid) return false;

		BuildPath( path );

		Best.path = path;

		return true;
	}

	bool PickupNitro() {
		//if (Self.nitro_amount > 75) return false;

		double minDist = 1000;
		NitroPack bestPack;

		if (game.GetNumNitroPacks() == 0) return false;

		for (int i = 0; i < game.GetNumNitroPacks(); i++) {
			const NitroPack & n = game.GetNitroPack( i );

			//if (!n.alive) continue;
			if (!n.alive && n.respawn_ticks > TicksToTravel( ToVec2( Self.position ), ToVec2( n.position ) )) continue;

			double d = Self.position.Dist( n.position );

			if (d < minDist) {
				minDist = d;
				bestPack = n;
			}
		}

		if (GameRole == ATTACKER) {
			if (minDist > Lerp( Self.nitro_amount / MAX_NITRO_AMOUNT, 12, 0 )) return false;
		}
		
		if (GameRole == GOALKEEPER) {
			if (Self.nitro_amount > 80 || !IsSafe()) {
				return false;
			}
		}

		Path path;

		GenOptimalPathTo( Self, bestPack, -1, false, path );

		if (!path.valid) return false;

		Best.path = path;

		BuildPath( Best.path );

		return true;
	}

	void SelectRole() {
		GameRole = NONE;

		int rank = GetRank();

		if (rank < TEAM_SIZE-1 || TEAM_SIZE == 1) {
			GameRole = ATTACKER;
		}
		else {
			GameRole = GOALKEEPER;
		}

	}

	void DoNothing( int numTicks ) {
		ClearActions();
		AddWaitTicks( numTicks );

		Best.valid = false;
		Best.path.valid = false;
	}

	bool InterceptBall() {
		if (GameRole == GOALKEEPER) return false;

		Path minPath;
		Path path;

		minPath.ticks = 10000;

		int minTickDiff = 10000;

		bool found = false;
		Point dest;

		double ballMinZ = Arena::ad;

		for (int tick = 0; tick < GetTicksForward(); tick += 4) {
			const Ball & b = GetBallTrace()[tick];

			if (tick > 50) break;

			if (GetBallScore().goalRes != 0 && tick > GetBallScore().goalTick) break;

			ballMinZ = min( ballMinZ, b.position.z );

			//if (b.position.z > Self.position.z) continue;
			if (b.position.y - BALL_RADIUS > MAX_JUMP_HEIGHT) continue;

			if (DistToBorder( ToVec2( b.position ) ) < BALL_RADIUS) continue;

			dest.point2 = ToVec2( b.position );
			//dest.dir = Vec2( 0, 1 );
			dest.dir = GetGateDir( dest.point2 );
			dest.velocity = ROBOT_MAX_GROUND_SPEED;

			//dest.point2 -= dest.dir * (ROBOT_RADIUS + BALL_RADIUS);
			//dest.point2 -= dest.dir * ROBOT_MAX_GROUND_SPEED * TimeToReachHeight( ROBOT_MAX_JUMP_SPEED, b.position.y - ROBOT_RADIUS );

			GenOptimalPathTo( Self, dest, -1, false, path );

			found = path.valid;

			int tickDiff = abs( path.ticks - tick );

			if (path.valid && minTickDiff > tickDiff) {
				minPath = path;
				minTickDiff = tickDiff;
			}
		}

		if (!found) {
			dest.point2.y = ballMinZ;
			dest.point2.x = Self.position.x;
			dest.dir = Vec2( 0, -1 );
			dest.velocity = ROBOT_MAX_GROUND_SPEED;

			GenOptimalPathTo( Self, dest, -1, false, minPath );
		}

		if (!minPath.valid) return false;

		Best.path = minPath;

		BuildPath( Best.path );

		return true;
	}

	static int EstimateReach( const Point & from, const Point & to, bool useNitro = false, double velocity = ROBOT_MAX_GROUND_SPEED ) {
		Vec2 accel1 = from.point2 + from.dir * GetAccelLength( from.velocity, velocity, useNitro );
		Vec2 accel2 = to.point2 - to.dir * GetAccelLength( velocity, to.velocity, useNitro );
		return GetAccelTicks( from.velocity, velocity, useNitro ) + TicksToTravel( accel1, accel2, velocity ) + GetAccelTicks( velocity, to.velocity, useNitro );
	}

	int GetTickLimit( int maxLimit ) const {
		int tickLimit = maxLimit;

		//tickLimit = min( tickLimit, BallPredict::TicksForward - 1 );

		if (GetBallScore().goalTick != -1) tickLimit = min( tickLimit, GetBallScore().goalTick );
		//if (!IsDefault && Actions.size() > 0) tickLimit = min( tickLimit, (int)Actions.size() );

		//if (IsGoalkeeper()) tickLimit = 60;

		if (AfterReset) {
			for (int i = 0; i < game.numRobots; i++) {
				const Robot & r = game.robots[i];

				if (r.id == Self.id) continue;

				Point dest;
				dest.SetEntity( game.ball );
				dest.dir = ToVec2( r.position ).DirTo( dest.point2 );
				dest.velocity = 30;

				dest.point2 -= dest.dir * (ROBOT_RADIUS + BALL_RADIUS);


				int minTicks = EstimateReach( r, dest, (Self.nitro_amount > 0) );

				tickLimit = min( tickLimit, minTicks - 5 );
			}
		}

		//int ttg = BallPredict::TicksToGoal;

		int ttg = GetBallScore().goalTick;

		if (IsGoalkeeper() && ttg != -1 && ttg < 40 && GetBallScore().goalRes == -1) {
			tickLimit = max( tickLimit, ttg );
		}

		return tickLimit;
	}

	void SortProbes( vector<Probe> & probes ) {

		Vec2 selfPos = ToVec2( Self.position );

		for (Probe & p : probes) {

			Vec2 dirToProbe = (p.jump.point2 - selfPos).Normalized();

			p.factor = 1.0;
			p.factor *= 1 + p.jump.dir.Dot( dirToProbe );
			p.factor *= 1 + p.jump.dir.Dot( GetGateDir(p.jump.point2) );
		}

		std::sort( probes.begin(), probes.end(), []( const Probe & left, const Probe & right ) { return left.factor > right.factor;  } );
	}

	NitroOpts GetNitroOpts() const {
		NitroOpts nitroOpts;

		bool isGoalkeeper = (GameRole == GOALKEEPER);

		nitroOpts.amount = Self.nitro_amount;
		nitroOpts.nitroJump = true;

		if (isGoalkeeper) {
			nitroOpts.nitroJump = true;
		}

		if (AfterReset && !isGoalkeeper) {
			nitroOpts.nitroRun = true;
			nitroOpts.nitroJump = false;
		}

		//if (!isGoalkeeper && BallPredict::TicksToGoal < 50 && BallPredict::IsEnemyGoal) {
		//	nitroOpts.nitroRun = true;
		//}

		//nitroOpts.nitroRun = true;

		return nitroOpts;
	}

	const BallTrace & GetBallTrace() const {
		static BallTrace trace;
		static int lastUpdated = -1;

		if (lastUpdated < game.CurrentTick) {
			trace.clear();
			GetBallTrace( GetTicksForward(), trace );
			lastUpdated = game.CurrentTick;
		}

		return trace;
	}

	Score GetBallScore() const {
		static Score score;
		static int lastUpdated = -1;

		if (lastUpdated < game.CurrentTick) {
			score = Score();
			score = ComputeScore( GetBallTrace(), true );
			lastUpdated = game.CurrentTick;
		}

		return score;
	}

	static int GetTicksForward() {
		return 150;
	}

	static void GetBallTrace( int ticks, BallTrace & trace ) {

		Predict pr;

		pr.UseRobots = false;

		pr.ball = game.ball;

		trace.reserve( ticks );

		while (ticks > 0) {

			trace.emplace_back( pr.ball );

			if (GetBallStatus( pr.ball ) != 0) break;

			pr.tickFixed();

			ticks--;
		}

	}

	void GetBallTrace( const Probe & probe, int maxTicks, bool simple, BallTrace & trace ) const {

		Predict pr;

		pr.ball.position = probe.ballPos;
		pr.ball.velocity = probe.ballVel;

		Robot & r = pr.AddRobot();

		r.position = probe.impactPos;
		r.velocity = probe.impactVel;

		int collideTimeout = 3;

		pr.UseFixedHitE = true;
		pr.FixedHitE = (MIN_HIT_E + MAX_HIT_E)*0.5;

		pr.UseSimpleCollide = simple;

		while (pr.ball.lastCollideRobot == -1 && collideTimeout > 0) {
			pr.tickFixed();
			collideTimeout--;
		}

		if (pr.ball.lastCollideRobot == -1) {
			return;
		}

		pr.UseRobots = false;

		int ticks = 0;

		while (ticks < maxTicks) {

			trace.emplace_back( pr.ball );

			if (GetBallStatus( pr.ball ) != 0) break;

			pr.tickAuto();
			ticks++;
		}

	}

	bool BuildTrajectory( const BallTrace & trace, int scoreThreshold, int tickLimit ) {
		static vector<Probe> probes;
		probes.clear();

		if (tickLimit == 0) return false;

		bool isDanger = IsDanger();

		int step = 2;
		if (isDanger || IsGoalkeeper()) step = 1;

		int strikeId = 3;

		if (AfterReset) strikeId = 5;

		NitroOpts nitroOpts = GetNitroOpts();

		//int num = FillProbes( GetBallTrace(), probes, step, tickLimit, nitroOpts, strikeId );
		int num = FillProbes( trace, probes, step, tickLimit, nitroOpts, strikeId );

		if (probes.empty()) return false;

		SortProbes( probes );

		int numRoughTests = 2000;
		int numPreciseTests = 300;

		if (AfterReset) numPreciseTests = 300;

		if(probes.size() > numRoughTests ) probes.resize( numRoughTests );

		assert( !probes.empty() );

		int simTicks = 100;
		if (IsGoalkeeper()) simTicks = 150;

		for (Probe & probe : probes) {
			probe.score = 0;
			probe.score += TestProbe( probe, simTicks, true, HIT_E_AVERAGE );
		}

		std::sort( probes.begin(), probes.end() );
		std::reverse( probes.begin(), probes.end() );
		if (probes.size() > numPreciseTests) probes.resize( numPreciseTests );

		//for (Probe & probe : probes) {
		//	probe.score = 0;
		//	probe.score += TestProbe( probe, simTicks, false, HIT_E_MIN );
		//	probe.score += TestProbe( probe, simTicks, false, HIT_E_MAX );
		//	probe.score *= 0.5;
		//}
		for (Probe & probe : probes) {
			probe.score = 0;
			int score = 10000000000;
			score = min( score, TestProbe( probe, simTicks, false, HIT_E_MIN ) );
			score = min( score, TestProbe( probe, simTicks, false, HIT_E_MAX ) );
			probe.score = score;
		}
		std::sort( probes.begin(), probes.end() );

		const Probe & best = probes.back();

		if (!best.valid) {
			return false;
		}

		if (best.score < scoreThreshold) {
			return false;
		}

		ClearActions();
		BuildProbe( best );
		Best = best;

		//std::sort( probes.begin(), probes.end(), []( const Probe & left, const Probe & right ) { return left.ticks < right.ticks;  } );

		//double d1 = Predict::dan_to_arena( ToVec3( best.path.rotCenter1 ) ).Dist;
		//double d2 = Predict::dan_to_arena( ToVec3( best.path.rotCenter2 ) ).Dist;
		//if (d1 < -3 || d2 < -3) {
		//	int b = 0;
		//}

		return true;
	}

	static int GetBallStatus( const Ball & b ) {
		if (b.position.z > Arena::ad2 + BALL_RADIUS) return 1;
		if (b.position.z < -Arena::ad2 - BALL_RADIUS) return -1;
		return 0;
	}

	double ScoreFunc( const Predict & predict, int tick, int maxTicks, bool isEnemy = false ) const {
		int score = 0;

		const Ball & b = predict.ball;

		int status = GetBallStatus( predict.ball );

		score += 1000000 * status;

		//score += predict.ball.velocity.z / ROBOT_MAX_GROUND_SPEED * 10000;
		//score += b.position.y / Arena::ah * 10000;

		score += (int)(predict.ball.position.z / Arena::ad * 200);
		score += (int)(predict.ball.position.y / Arena::ah * 1000);
		score += Sqr( predict.ball.velocity.z / MAX_ENTITY_SPEED * 200 ) * Sign( predict.ball.velocity.z );

		//score += (b.position.z / Arena::ad) * (b.position.y / (Arena::gh - BALL_RADIUS)) * 1000;

		//score += ToVec2( predict.ball.velocity ).Dot( GetGateDir(ToVec2(b.position)) ) / MAX_ENTITY_SPEED * 1000;
		//score -= ToVec2( predict.ball.velocity ).Dot( GetGateDir(ToVec2(b.position), true) ) * 100;

		//if (predict.ball.position.z > -Arena::ad2 + Arena::cr) score -= 1000;

		//if (b.position.y > MAX_JUMP_HEIGHT + BALL_RADIUS) score += 500; else score -= 500;

		//if (b.dist < BALL_RADIUS + 1 /*&& b.touch_normal != Vec3(0,1,0)*/ ) {
			//score -= 500;
			score -= (1-b.dist / 10.0) * 2000;
		//}

		//if (b.position.z > Arena::ad2 - Arena::cr && b.position.y > MAX_JUMP_HEIGHT) score += 10000;

		for (int i = 0; i < predict.numRobots; i++) {
			const Robot & enemy = predict.robots[i];

			if (enemy.is_teammate) continue;

			if (enemy.position.z < b.position.z) continue;

			//if (abs( enemy.position.y - predict.ball.position.y ) > MAX_JUMP_HEIGHT) score += 300;

			double enemyDist = ToVec2( enemy.position ).Dist( ToVec2( predict.ball.position ) );
			if (enemyDist < 5 && b.position.y < MAX_JUMP_HEIGHT+BALL_RADIUS) {
				score -= Sqr( 1.0 / enemyDist * 100 );
			}

			//score += (b.position.z - enemy.position.z) / 100 * 500;

			//score += Sqr( enemy.position.Dist( predict.ball.position ) / 100 * 100 );
		}

		score *= exp( -tick / (double)maxTicks );

		return score;
	}

	Score ComputeScore( const BallTrace & trace, bool useRobots = false, int startTick = 0 ) const {

		Score s;

		int tick = startTick;
		int maxTicks = trace.size();

		s.score = 0;
		s.goalTick = -1;
		s.ticks = 0;
		s.goalRes = 0;

		Predict predict;

		predict.UseEntityCollide = false;
		predict.ball.enabled = false;

		//useRobots = false;

		if (useRobots) {
			for (int i = 0; i < game.numRobots; i++) {
				const Robot & r = game.robots[i];

				if (r.is_teammate) continue;

				predict.AddRobot( r );
			}

			predict.ball = trace[0];
			predict.ball.enabled = false;

			for (int i = 0; i < startTick; i++) {
				if (i > 30) break;
				//EnemyAiSimple( predict );
			}
		}

		for (const Ball & b : trace) {

			if (useRobots && tick < 30) {
				//EnemyAiSimple( predict );
			}

			predict.ball = b;
			predict.ball.enabled = false;
			s.score += ScoreFunc( predict , tick, maxTicks );
			s.goalRes = GetBallStatus( b );

			if (s.goalRes!= 0) break;

			tick++;
		}

		s.score *= 1.0 / (maxTicks+1);

		//s.score *= 1.1;
		//s.score += 100;

		//s.score -= exp( startTick / 150.0 ) * 200;
		s.score += exp( 1 - startTick / 150.0  ) * 200;

		//int enemyReach = EnemyReachTick( startTick, trace, game );

		//if (enemyReach != -1) s.score -= 500 * enemyReach / 150.0;

		s.ticks = trace.size();
		s.goalTick = tick;

		return s;
	}

	int EnemyReachTick( int startTick, const BallTrace & trace, const Predict & enemy ) const {

		int minReachTick = 100000;

		int tick = startTick;

		bool reachable = false;
		
		for (const Ball & b : trace) {
			if (b.position.y - ROBOT_RADIUS> MAX_JUMP_HEIGHT) continue;

			for (int i = 0; i < enemy.numRobots; i++) {
				const Robot & r = enemy.robots[i];

				if (r.is_teammate) continue;

				Point impact;

				impact.dir = GetGateDir( ToVec2( r.position ), true );
				impact.point2 = ToVec2( b.position ) - impact.dir * (ROBOT_RADIUS + BALL_RADIUS);
				impact.velocity = ROBOT_MAX_GROUND_SPEED;

				int reachTick = EstimateReach( r, impact );

				int diff = abs( reachTick - tick );

				if (diff > 3) continue;

				minReachTick = min( minReachTick, reachTick );
				reachable = true;
			}

			tick++;
		}

		return reachable?minReachTick:-1;
	}

	void TestProbe( Probe & probe ) {
		BallTrace trace;

		GetBallTrace( probe, 100, false, trace );
		Score score = ComputeScore( trace, probe.ticks );

		int enemyReach = -1;

		enemyReach = EnemyReachTick( 0, GetBallTrace(), game );

		if (enemyReach <= probe.ticks) {
			score.score -= (IsGoalkeeper()) ? 10000 : 1000;
		}
		else {
			score.score += 1000;
		}

		if (IsGoalkeeper()) score.score -= probe.ticks / 100.0 * 1000;

		if (GetBallScore().goalRes == -1 && GetBallScore().goalTick != -1 && GetBallScore().goalTick < probe.ticks+5) {
			score.score -= 100000;
		}

		probe.score = score.score;
	}

	bool TryBuildTrajectory() {

		vector<Probe> probes;

		FillProbes( GetBallTrace(), probes, 1, IsGoalkeeper()?60:60, GetNitroOpts(), IsGoalkeeper()?5:4 );

		if (probes.empty()) return false;

		SortProbes( probes );

		//if (probes.size() > 2000) probes.resize( 2000 );

		for (Probe & probe : probes) {
			TestProbe( probe );
		}

		std::sort( probes.begin(), probes.end() );

		Probe best = probes.back();

		//int score = ComputeScore( GetBallTrace() ).score;

		if (best.score > -10000) {
			Best = best;

			ClearActions();
			BuildProbe( Best );

			return true;
		}

		return false;
	}


	int NextThink = -1;
	int RebuildTimeout = 0;
	double NitroPrevFrame = 0;
	bool IsDefault = true;

	Vec2 GetGoalkeeperPos() const {
		
		double minZ = 100000000;
		for (int i = 0; i < game.numRobots; i++) {
			const Robot & r = game.robots[i];

			if (r.id == Self.id) continue;

			minZ = min( minZ, r.position.z );
		}

		const Ball & b = GetBallTrace()[20];

		minZ = min( minZ, GetBallTrace()[20].position.z );
		//minZ = min( minZ, GetBallTrace()[0].position.z );

		minZ -= 15;

		minZ = min( minZ, -Arena::ad2 + 15 );

		minZ = max( minZ, -Arena::ad2 );

		return Vec2( clamp( b.position.x, -Arena::gw2+5, Arena::gw2-5), minZ );

	}

	double GetGoalkeeperRadius() const {
		return 15;

		//double radius;

		//radius = Predict::GateCenter().Dist( GetGoalkeeperPos() );
		//radius = max( radius, 15.0 );
		//return radius;
	}

	void Rebuild() {
		ClearActions();

		Ghost = Self;

		SelectRole();

		if (GameRole == NONE) return;

		NextThink = 20;
		IsDefault = true;
		RebuildTimeout = 5;

		if (IsGoalkeeper()) RebuildTimeout = 3;

		double distToGate = ToVec2( Self.position ).Dist( ToVec2( GetBallTrace()[30].position) );
		//double distToGate = Predict::GateCenter().Dist( ToVec2( GetBallTrace()[30].position) );

		if (GameRole == GOALKEEPER && distToGate > GetGoalkeeperRadius()) {
			if (PickupNitro()) return;
			GoalKeeperMove();
			return;
		}


		int thresh = GetBallScore().score;
		int tickLimit = GetTickLimit( 80 );

		//thresh = -2000;

		//if (!IsDefault) {
		//	thresh = Best.score;
		//	tickLimit = GetTickLimit( 40 );
		//}

		bool built = BuildTrajectory( GetBallTrace(), thresh, tickLimit );
		//bool built = TryBuildTrajectory();

		//int sc1 = ComputeScore( GetBallTrace(), false ).score;
		//int sc2 = ComputeScore( GetBallTrace(), true ).score;

		//for (int i = 0; i < MAX_ROBOTS; i++) {
		//	const Mover & mover = GetMover( i );
		//	if (mover.Self.id == Self.id) continue;
		//	if (mover.Self.id == -1) continue;
		//	if (mover.IsDefault) continue;

		//	if (mover.Best.ticks < Best.ticks && mover.Best.score > Best.score) {
		//		BallTrace trace;

		//		mover.EvalActions( 100, trace );

		//		if (BuildTrajectory( trace, -1000, 100 )) {
		//			IsDefault = true;
		//			RebuildTimeout = 15;
		//			return;
		//		}
		//	}
		//}

		if (built) {
			IsDefault = false;
			RebuildTimeout = 10;

			//UseBallTrace = true;
			//TestProbe( Best, 150, false, HIT_E_AVERAGE );

			DbgRebuildId = Self.id;

			//if (GameRole == GOALKEEPER) {
			//	if (AfterReset || ToVec2( Best.ballPos ).Dist( GetGoalkeeperPos() ) > GetGoalkeeperRadius()) {
			//		ClearActions();
			//		PickupNitro();
			//		GoalKeeperMove();
			//	}
			//}

			//}

			for (int i = 1; i < MAX_ROBOTS; i++) {
				const Mover & mover = GetMover( i );
				if (mover.Self.id == Self.id) continue;
				if (mover.Self.id == -1) continue;
				if (mover.IsDefault) continue;

				if (mover.Best.ticks < Best.ticks && mover.Best.score > Best.score) {
					BallTrace trace;

					mover.EvalActions( 100, trace );

					if (BuildTrajectory( trace, mover.Best.score, 80 )) {
						IsDefault = true;
						RebuildTimeout = 15;
						return;
					}
				}
			}

			return;
		}

		IsDefault = true;

		if (PickupNitro()) return;
		if (InterceptBall()) return;
		if (GoalKeeperMove()) return;

		DoNothing( 5 );
	}

	void TestMove() {
		//AddAccel( 0, 15, false, Vec2( 0, -1 ) );
		////AddRotation( 15, Vec2( 0, -1 ), Vec2( 0, -1 ).Slerped(0.2,Vec2(-1,0)), CCW );
		////AddRotation( 15, Vec2( 0, -1 ), Vec2( 0, 1 ) , CW );
		//AddRotation( 15, Vec2( 0, -1 ), Vec2( 1, 0 ), CCW );

		//double r = GetRotateRadius( 30 );
		////AddRotationQ( 30, Vec2( 0, -1 ), Vec2( 1, 0 ) );
	}

	void TestMove2() {

		double r = GetRotateRadius( 30 );

		double spd = ROBOT_MAX_GROUND_SPEED;
		spd = 15;
		Point robotPos;
		robotPos.SetEntity( Self );
		Path path;
		Point finalp;
		finalp.point2 = Vec2( 0, 10 );
		finalp.point2 = Vec2( 10, 0 );
		finalp.dir = Vec2( 0, -1 );
		//finalp.dir = Vec2( 1, 0 );
		//finalp.dir = Vec2( 0, 1 );
		finalp.point2 = robotPos.point2 + Vec2( 0, -10 );
		finalp.dir = (finalp.point2 - robotPos.point2).Normalized().Right(0.2);
		finalp.velocity = spd;
		finalp.velocity = 5;
		GenPathTo( robotPos, spd, spd, spd, false, finalp, path );
		BuildPath( path );
		Best.path = path;
	}

	void TestMove3() {
		double spd = 30;
		//spd = 15;

		Point robotPos;
		robotPos.SetEntity( Self );
		Path path;
		Point finalp;

		Vec2 accelDir( 0, -1 );
		double accelVel = 30;
		accelDir = accelDir.Left( 0.3 );

		//robotPos.point2 += accelDir * GetAccelLength( 0, accelVel );
		//robotPos.dir = accelDir;
		//robotPos.velocity = accelVel;

		finalp.SetPoint( Vec2( 0, 40 ) );
		finalp.SetPoint( Vec2( 15, 0 ) );
		//finalp.point2 = robotPos.point2 + Vec2( 0, 2 );
		//finalp.dir = Vec2( 0, 1 );
		finalp.dir = Vec2( 1, 0 );
		finalp.velocity = 30;

		int ticks1, ticks2,ticks3;

		NitroOpts opts;

		opts.nitroRun = true;
		opts.amount = Self.nitro_amount;

		//AddWaitTicks( 100 );
		//AddAccel( 0, 30, false, accelDir );
		//ticks1 = GenPathTo( robotPos, 30, 30, 30, finalp, path );
		//ticks2 = GenPathTo( robotPos, 30, 30, 1, finalp, path );
		//ticks3 = GenPathTo( robotPos, 15, 25, 30, false, finalp, path );
		GenOptimalPathTo( robotPos, finalp, -1, opts, path );
		//GenPathTo( robotPos, 3, 3, 3, false, finalp, path );

		double al = GetAccelLength( path.reachVel, path.finalVel );
		double d1 = path.finalPoint.Dist( path.accel2 );

		if (!path.valid) {
			int breakp = 0;
		}

		int s = sizeof( Path );

		BuildPath( path );
		Best.path = path;

		Best.valid = true;
		Best.path.valid = true;

		int ticks = EstimateReach( robotPos, finalp );

		//for (int i = 0; i < 2; i++) {
		//	FixPathMovement();
		//}
	}

#ifdef DEBUG

	void DebugDrawTrace( const BallTrace & trace, const string & color, int step = 10 ) {
		int tick = 0;
		for (const Ball & b : trace) {
			if (tick % step != 0) continue;
			debug.AddSphere( b.position, BALL_RADIUS, color, 0.2 );
			tick++;
		}
	}

	void DebugDrawPath( const Path & path ) {
		if (!path.valid) return;
		debug.AddSphere( ToVec3(path.rotCenter1), path.rotRadius1, "Green", 0.2 );
		debug.AddSphere( ToVec3(path.rotCenter2), path.rotRadius2, "Green", 0.2 );
		debug.AddSphere( ToVec3( path.initPoint ), 0.5, "Purple", 0.5 );
		debug.AddSphere( ToVec3( path.finalPoint ), 0.5, "Cyan", 0.5 );
		debug.AddSphere( ToVec3( path.lp1 ), 0.5, "Red", 0.5 );
		debug.AddSphere( ToVec3( path.lp2 ), 0.5, "Red", 0.5 );
		debug.AddSphere( ToVec3( path.accel1 ), 0.5, "Green", 0.5 );
		debug.AddSphere( ToVec3( path.accel2 ), 0.5, "Green", 0.5 );
		//debug.AddParameter( "DistVel", path.velLine2.DistToPoint( ToVec2( Self.position ) ) );
		//debug.AddParameter( "DistFinal", path.finalPoint.Dist( ToVec2( Self.position ) ) );
		//debug.AddParameter( "Dist2", path.accel2.Dist( ToVec2( Self.position ) ) );
		//debug.AddParameter( "DistLn", path.lineMove.DistToPoint( ToVec2( Self.position ) ) );
		//debug.AddParameter( "DistArc1", path.rotCenter1.Dist( ToVec2( Self.position ) ) );
		//debug.AddParameter( "DistArc2", path.rotCenter2.Dist( ToVec2( Self.position ) ) );
	}

	void DebugDrawProbe( const Probe & probe ) {
		DebugDrawPath( probe.path );

		if (probe.ticks < 0) return;
		if (!probe.valid) return;

		debug.AddSphere( probe.ballPos, BALL_RADIUS, "White", 0.2 );
		debug.AddSphere( probe.impactPos, ROBOT_RADIUS, "White", 0.2 );
		debug.AddSphere( ToVec3( probe.jump.point2 ), ROBOT_RADIUS, "Olive", 0.2 );
		debug.AddParameter( "BestScore", probe.score );
		debug.AddParameter( "ProbeGoal", probe.dbgGoaTick );
	}

	void DebugDraw() {
		double distToGate = Predict::GateCenter( 0 ).Dist( ToVec2( game.ball.position ) );

		//if(GameRole == GOALKEEPER) debug.AddParameter( "distToGate", distToGate );

		if (DbgRebuildId != Self.id) return;

		DBG( debug.AddParameter( "Speed", Self.velocity.Length() ) );
		DBG( debug.AddParameter( "IsDefault", IsDefault ) );
		DBG( debug.AddParameter( "Actions", (int)Actions.size() ) );
		DBG( debug.AddParameter( "DistToBorder", DistToBorder( ToVec2( Self.position ) ) ) );
		DBG( debug.AddParameter( "NitroAmount", Self.nitro_amount ) );
		DBG( DebugDrawProbe( Best ) );

		DBG( debug.AddSphere( Ghost.position, ROBOT_RADIUS, "Yellow" ) );

		//int tick = 0;
		//for (const Ball & b : BallTrace) {
		//	tick++;
		//	if (tick % 10 != 0) continue;
		//	debug.AddSphere( b.position, BALL_RADIUS, "Gray", 0.2 );
		//}
	}
#endif

	void DebugFuncs() {

		if (game.CurrentTick == 0) TestMove3();

		//DebugDrawPath( Best.path );
		//DebugDraw();
	}

	void EvalActions( int maxFrames, BallTrace & trace ) const {

		int tick = 0;

		Predict pr;

		pr.ball = game.ball;

		Robot & r = pr.AddRobot( Self );

		for (const Action & a : Actions) {

			if (tick > maxFrames) break;

			r.action = a;

			trace.emplace_back( pr.ball );

			pr.tickAuto();
			tick++;
		}

		pr.UseRobots = false;

		while (tick < maxFrames) {

			trace.emplace_back( pr.ball );

			pr.tickAuto();

			tick++;
		}

	}

	void NitroDown() {
		if (Self.nitro_amount <= FLT_EPSILON) return;
		if (!Actions.empty()) return;

		AddAction( Action( Vec3( 0, -MAX_ENTITY_SPEED, 0 ), 0, true ) );		
	}

	void MoveGhost() {
		Predict pr;
		Robot & r = pr.AddRobot( Ghost );

		pr.UseSimpleCollide = true;
		pr.SetMicroTicks( 1 );

		if (!Actions.empty()) r.action = Actions.front(); else r.action = Action();

		pr.tickFixed();

		Ghost = r;
	}

	void MoveOffSlope() {
		if (Actions.size() > 0 /*&& Self.velocity.Length() < 8*/) return;

		Vec3 dir;

		dir = (Vec3( 0, 0, 0 ) - Self.position).Rescaled( ROBOT_MAX_GROUND_SPEED );

		AddAction( Action( dir ) );
	}

	void TryRebuild() {

	}

	Action GetAction() {
		if (ResetTimeout > 0) {
			return Action();
		}

		if (AfterReset) {
			ClearActions();
		}

		const Plane dan = Predict::dan_to_arena( Self.position );
		const bool inAir = dan.Dist > Self.radius + FLT_EPSILON || Self.position.y > Self.radius + Arena::br;
		const bool isSlope = !inAir && dan.Normal.Dot( Vec3( 0, 1, 0 ) ) < 0.95;

		const bool nitroExhausted = Self.nitro_amount < 0.1 && NitroPrevFrame > 0.1;
		NitroPrevFrame = Self.nitro_amount;

		double robotDistErr = Self.position.Dist( Ghost.position );

		//IsBallChanged |= BallPredict::Changed;

		//DebugFuncs();

		//return PopAction();

		if (BallChanged) {
			ClearActions();
		}

		NextThink--;
		RebuildTimeout--;
		
		bool rebuild = false;
		rebuild |= IsActionsEmpty();
		//rebuild |= BallChanged;
		rebuild |= IsDefault;
		//rebuild |= nitroExhausted;
		rebuild |= robotDistErr > 1;
		//rebuild |= NextThink <= 0;
		rebuild &= !inAir;
		rebuild &= (RebuildTimeout <= 0);
		rebuild &= !isSlope;

		if (rebuild) {
			Rebuild();
		}

		if (isSlope) {
			MoveOffSlope();
		}

		if (inAir) {
			NitroDown();
		}

		MoveGhost();

		DBG( DebugDraw() );

		return PopAction();
	}

	static void InitFrame() {

	}
};


Mover & GetMover( int id ) {
	static Mover movers[MAX_ROBOTS];
	assert( id < MAX_ROBOTS + 1 );
	return movers[id - 1];
}

Mover & GetMover( const Robot & r ) {
	Mover & m = GetMover( r.id );
	m.Self = r;
	return m;
}

Mover & GetDummy() {
	static Mover dummy;
	return dummy;
}

void InitGame( const model::Rules & rules ) {
	static bool isInit = false;

	if (isInit) {
		return;
	}

	isInit = true;

	Mover::InitMovers();

	MAX_TICK_COUNT = rules.max_tick_count;
	TEAM_SIZE = rules.team_size;

	AfterReset = true;
}

void EvalBall() {
	static Predict ballPred;

	if (ResetTimeout > 0) return;

	if (game.CurrentTick == 0) {
		BallChanged = true;
		ballPred.UseRobots = false;
	}

	double deltaP = (ballPred.ball.position - game.ball.position).Length();
	double deltaV = (ballPred.ball.velocity - game.ball.velocity).Length();

	BallChanged = (deltaV > 1);
	//BallChanged = (deltaP > 1.5);

	if (BallChanged) {
		ballPred.ball = game.ball;
	}

	ballPred.tickFixed();
}

void InitFrame( const Predict & _game ) {
	static int LastUpdated = -1;

	game = _game;
	IsNewFrame = (LastUpdated < game.CurrentTick);

	if (!IsNewFrame) {
		return;
	}

	LastUpdated = game.CurrentTick;

	if (AfterReset && ResetTimeout == 0) AfterReset = false;

	if (game.CurrentTick == 0) AfterReset = true;

	if (game.IsScored() && ResetTimeout == 0) {
		ResetTimeout = RESET_TICKS;
		AfterReset = true;
	}

	if (ResetTimeout > 0) ResetTimeout--;

	EvalBall();

	Mover::InitFrame();

	//BallPredict::BuildTrajectory();

	DBG( debug.AddGameOjbects( game ) );
	DBG( debug.FinishFrame() );
}

MyStrategy::MyStrategy() {}
void MyStrategy::act(const model::Robot& me, const model::Rules& rules, const model::Game& _game, model::Action& action) {
	TotalTime.Begin();
		
		//DBG( Sleep( 5 ) );

		InitGame( rules );
		InitFrame( _game );

		//action = Quickstart( me, _game, false );

		action = GetMover( me ).GetAction();

	TotalTime.End();
}

#ifdef BENCHMARK

int main( int argc, char* argv[] ) {

	return 0;
}

#endif
