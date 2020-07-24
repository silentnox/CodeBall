#include "csimplesocket/ActiveSocket.h"
//
#define RAPIDJSON_HAS_STDSTRING 1

#include "Helpers.h"
#include "Predict.h"
#include "JsonNetwork.h"
//#include "GameAI.h"

#include <chrono>
#include <iostream>

#include "DebugHelper.h"

using namespace std;
using namespace rapidjson;
using namespace chrono;

JsonNetwork JN;

int TestVis() {
	//Document doc;
	//doc.SetObject();
	//doc.AddMember("Hello", "Wtf", doc.GetAllocator());
	////doc["dqfe"] = "wqddw";
	//string str;
	//str = doc.GetString();
	//return 0;



	JsonNetwork::Object o,o2;

	float offs = 10.0;

	o["Type"] = "Shape";
	o["Pri"] = "Cube";
	o["Scale"] = PackVec3(6.0,6.0,6.0);
	o["Radius"] = std::to_string(4.0);
	o["Color"] = "Green";
	o["Alpha"] = "0.5";
	o["Pos"] = PackVec3(0.0, offs, 0.0);

	o2["Type"] = "Line";
	o2["Width"] = "0.2";
	o2["Color"] = "White";
	o2["Points"] = PackVec3(0.0, offs, 0.0) + " " + PackVec3(0.0, offs+5.0, 0.0);

	JN.AddObject(o);
	JN.AddObject(o2);
	JN.Submit();



	Sleep(2000);

	for (int i = 0; i < 1000; i++) {
		o["Pos"] = PackVec3(0.0, offs, 0.0);
		o2["Points"] = PackVec3(0.0, offs, 0.0) + " " + PackVec3(0.0, offs + 5.0, 0.0);
		offs += 0.1f;
		JN.AddObject(o);
		JN.AddObject(o2);
		JN.Submit();
		Sleep(10);
	}

	return 0;
}

typedef std::chrono::milliseconds ms;
typedef high_resolution_clock::time_point hrtp;

void BenchCol() {
	hrtp t1 = high_resolution_clock::now();

	//Predict pr;
	for (int i = 0; i < 100000; i++) {
		Predict::dan_to_arena(Vec3(random(-30,30), random(0,20), random(-50,50)));
	}

	hrtp t2 = high_resolution_clock::now();

	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	ms msec = duration_cast<ms>(time_span);

	cout << msec.count() << endl;
	//cin.get();
	return;
}

void BenchCol2() {

	Predict pr;
	pr.ball.position = Vec3(0, 10, 0);
	pr.ball.velocity = Vec3(0, 0, 0);

	//pr.UseBall = true;
	//pr.UseRobots = false;

	//hrtp t1 = high_resolution_clock::now();
	//pr.ticks(2000);
	//hrtp t2 = high_resolution_clock::now();

	pr.UseBall = true;
	pr.UseRobots = false;

	//pr.robots.emplace_back(Robot());

	pr.AddRobot();

	//Robot & r = pr.robots[0];
	Robot & r = pr.GetRobot( 0 );

	r.nitro_amount = 100;

	r.position = Vec3(20, 1, 0);
	//r.velocity.z = ROBOT_MAX_GROUND_SPEED * 0.5;
	r.velocity.x = ROBOT_MAX_GROUND_SPEED;
	r.touch = false;
	//r.action.jump_speed = ROBOT_MAX_JUMP_SPEED;
	r.action.target_velocity.x = ROBOT_MAX_GROUND_SPEED;
	r.action.target_velocity.x = MAX_ENTITY_SPEED;
	r.action.use_nitro = true;


	Timer t,t2;
	int msec = 0,msec2 = 0;
	t2.Begin();
	//for (int i = 0; i < 20000; i++) {
	//	//t.Begin();
	//	pr.ticks(1);
	//	//msec += t.GetMsec().count();

	//	//for (int j = 0; j < 6000; j++) {
	//		//for (int k = 0; k < 100; k++) msec++;
	//	//}
	//	//for(int )
	//}
	pr.ticks(200000);
	msec2 =t2.GetMsec().count();

	cout << msec2 << endl;
	cin.get();
	return;
}

//width: 60
//	height : 20
//	depth : 80
//	bottom_radius : 3
//	top_radius : 7
//	corner_radius : 13
//	goal_top_radius : 3
//	goal_width : 30
//	goal_depth : 10
//	goal_height : 10
//	goal_side_radius : 1

void VisPrediction() {
	Predict pr;
	Predict pr2;
	Predict pr3;

	pr.ball.position = Vec3(0, 10, 0);
	//pr.ball.velocity = Vec3( random( 10, 30 ), random(5,10), random(10,30));
	pr.ball.velocity = Vec3( 0, 0, 10 );

	pr2.ball = pr.ball;
	pr3.ball = pr.ball;

	Ball bl;

	//Predict::Ball b(bl);

	//b.radius;

	//pr.SetArenaDefault();
	//pr2.SetArenaDefault();

	//pr.TicksPerSec = 60;
	pr.SetMicroTicks( 100 );

	//pr2.TicksPerSec = 60;
	pr2.SetMicroTicks( 1 );

	pr3.SetMicroTicks( 1 );

	JsonNetwork::Object o,o2,o3,o4;

	o["Type"] = "Shape";
	o["Pri"] = "Sphere";
	o["Radius"] = std::to_string(pr.ball.radius);
	o["Color"] = "Green";
	o["Alpha"] = "0.5";
	o["Pos"] = PackVec3(0.0, 0.0, 0.0);

	o3["Type"] = "Shape";
	o3["Pri"] = "Sphere";
	o3["Radius"] = std::to_string(pr.ball.radius);
	o3["Color"] = "Yellow";
	o3["Alpha"] = "0.5";
	o3["Pos"] = PackVec3(0.0, 0.0, 0.0);

	o4 = o3;
	o4["Color"] = "Red";

	o2["Type"] = "Line";
	o2["Width"] = "0.15";
	o2["Color"] = "Red";

	int msec = 0;

	JN.AddObject(o);
	JN.Submit();

	Ball prev;

	int num100ticks = 0;
	int num1ticks = 0;

	pr.UseRobots = false;
	//pr2.UseRobots = false;

	pr.UseSimpleCollide = true;
	pr2.UseSimpleCollide = true;
	pr3.UseSimpleCollide = true;

	//for (int i = 0; i < 1000; i++) {
	while(true) {
		Sleep(10);
		Plane dan = pr.dan_to_arena(pr.ball.position);

		//hrtp t1 = high_resolution_clock::now();
		pr.ticks(1);

		prev = pr2.ball;
		//pr2.SetMicroTicks( 1 );
		pr2.ticks(1);

		pr3.tickAuto();

		if (pr2.GetMicroTicks() == 100) num100ticks++; else num1ticks++;

		if (pr2.ball.lastCollideArena == pr2.CurrentTick-1 && pr2.ball.touch_normal.Dot( Vec3(0,1,0) ) < 1.0-FLT_EPSILON) {
			pr2.CurrentTick--;
			pr2.ball = prev;
			pr2.SetMicroTicks( 100 );
			pr2.tick();
			num100ticks++;
		}
		else {
			pr2.SetMicroTicks( 1 );
			//num1ticks++;
		}

		if (pr.CurrentTick == 300) {
			int breakp = 0;
		}

		//hrtp t2 = high_resolution_clock::now();
		//msec += duration_cast<ms>(t2 - t1).count();

		if (pr.ball.velocity.Length() < 3.0) {
		//	pr.ball.velocity += Vec3(random(3, 8), random(3,6), random(3, 8));
		}

		//pr2.ball.velocity = pr.ball.velocity;

		o["Pos"] = PackVec3(pr.ball.position);
		o["Name"] = "Ball";
		//o["Id"] = to_string(pr.CurrentTick);
		JN.AddObject(o);

		o3["Pos"] = PackVec3(pr2.ball.position);
		JN.AddObject(o3);

		o4["Pos"] = PackVec3( pr3.ball.position );
		JN.AddObject( o4 );

		o2["Points"] = PackLine(pr.ball.position, pr.ball.position + -dan.Normal * dan.Dist);
		JN.AddObject(o2);
		//JN.AddCommand("Clear", "");
		JN.AddCommand("Write", "Time: " + to_string(msec));
		JN.AddCommand("Write", "Normal: " + PackVec3(dan.Normal));
		JN.AddCommand("Write", "Dist: " + to_string(dan.Dist));

		JN.Submit();
	}

	//int count = msec.count();

	//JN.Submit();
}


void TestChrono() {
	hrtp t1 = high_resolution_clock::now();
	Sleep(1000);
	hrtp t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	ms msec = duration_cast<ms>(time_span);
	return;
}

void TestSpeed() {

	hrtp t1 = high_resolution_clock::now();
	double res = 0;
	double value = 3.0;
	for (int i = 0; i < 10000000; i++) {
		res += value / 2.0;
	}
	hrtp t2 = high_resolution_clock::now();
	ms time_span = duration_cast<ms>(t2 - t1);

	res = 0;

	t1 = high_resolution_clock::now();
	double res2 = value / 2.0;
	for (int i = 0; i < 10000000; i++) {
		res += res2;
	}
	t2 = high_resolution_clock::now();
	ms time_span2 = duration_cast<ms>(t2 - t1);

	Vec3 vec(1, 15, 12);

	res = 0;

	t1 = high_resolution_clock::now();
	for (int i = 0; i < 1000000; i++) {
		res += vec.Length();
	}
	t2 = high_resolution_clock::now();
	ms time_span3 = duration_cast<ms>(t2 - t1);

	res = 0;

	t1 = high_resolution_clock::now();
	for (int i = 0; i < 1000000; i++) {
		res += vec.LengthSq();
	}
	t2 = high_resolution_clock::now();
	ms time_span4 = duration_cast<ms>(t2 - t1);

}

int TicksToReachHeight( double jumpVel, double height, bool second = false ) {
	double d = Sqr( jumpVel ) - (2 * GRAVITY * height);
	if (d < 0) return -1;
	double t;
	if (!second) {
		t = (jumpVel - sqrt( d )) / (GRAVITY);
	}
	else {
		t = (jumpVel + sqrt( d )) / (GRAVITY);
	}
	return (int)(t * TICKS_PER_SECOND);
}

void TestRobot() {

	JsonNetwork::Object o;

	o["Type"] = "Shape";
	o["Pri"] = "Sphere";
	o["Radius"] = std::to_string(1);
	o["Color"] = "Green";
	o["Alpha"] = "0.5";
	o["Pos"] = PackVec3(0.0, 0.0, 0.0);

	Predict pr;

	pr.UseBall = false;

	//pr.robots.emplace_back(Robot());

	//Robot & r = pr.robots[0];

	Robot & r = pr.AddRobot();

	r.position = Vec3(0, 1, 0);
	//r.velocity.z = 25;
	//r.velocity.x = ROBOT_MAX_GROUND_SPEED;
	//r.touch = false;
	r.action.jump_speed = ROBOT_MAX_JUMP_SPEED;
	//r.action.target_velocity.z = -ROBOT_MAX_GROUND_SPEED * 0.5;
	//r.action.target_velocity.x = ROBOT_MAX_GROUND_SPEED;
	//r.action.target_velocity.y = ROBOT_RADIUS;
	//r.action.target_velocity.z = 15;

	//r.action.jump_speed = ROBOT_MAX_JUMP_SPEED;
	//r.action.target_velocity.y = ROBOT_MAX_GROUND_SPEED;
	//r.nitro_amount = 100;
	//r.action.use_nitro = true;

	//pr.MicroTicksPerTick = 1;

	//r.action.target_velocity.z = 100;
	//r.action.use_nitro = true;
	//r.nitro_amount = 100;

	double maxY = 0;
	double maxZ = 0;
	double maxX = 0;

	double maxVelZ = 0;
	double minVelZ = 100;

	int i = 0;

	int ticks = 0;
	double vel;

	for (i = 0; i < 100; i++) {

		maxY = max(r.position.y, maxY);
		maxZ = max(r.position.z, maxZ);
		maxX = max(r.position.x, maxX);
		maxVelZ = max(r.velocity.z, maxVelZ);
		minVelZ = min(r.velocity.z, minVelZ);

		int t = TicksToReachHeight( 15, r.position.y - ROBOT_RADIUS );

		vel = r.velocity.Length();
		//if (vel <= 15) break;

		pr.tick();

		o["Pos"] = PackVec3(r.position);

		JN.AddObject(o);
		JN.Submit();



		//r.action.target_velocity.x = 0;

		//if (i > 10 && r.position.y < 1.1) break;
		//if (r.velocity.Length() <= FLT_EPSILON) break;
		//if (r.velocity.z <= FLT_EPSILON*2) break;
		//if (r.velocity.x >= ROBOT_MAX_GROUND_SPEED - FLT_EPSILON*2) break;
		//if (maxZ > AccelLengthMax) break;

		//double f = r.touch_normal.Dot(Vec3(0, 1, 0));
		//if (r.dist < 1.1 && abs(r.touch_normal.Dot(Vec3(1, 0, 0))) > 0) {
		//	r.action.target_velocity = Vec3(0, ROBOT_MAX_GROUND_SPEED, 0);
		//}
		//if (r.dist < 1.1 && f < 1 && f > 0 )  {
		//	r.action.jump_speed = ROBOT_MAX_JUMP_SPEED;
		//}
		//else if (r.position.y > 5 && r.dist < 1.1) {
		//	r.action.jump_speed = ROBOT_MAX_JUMP_SPEED;
		//}
		//else {
		//	r.action.jump_speed = 0;
		//}

		ticks++;

		Sleep(20);
	}

	//Sleep(200);
	//TestRobot();
}

void TestBallStrike() {
	Predict pr;
	DebugHelper debug;

	Robot & r = pr.AddRobot();

	pr.ball.position = Vec3( 0, 10, 0 );
	r.position = Vec3( 0, 10, 0 );
	r.position += Vec3( 0, 0, 3 );
	r.velocity = Vec3( 0, 30, 0 );

	while (true) {

	}
}

void test2( int & v ) {
	v -= 5;
}

int test1(int n ) {
	test2( n );
	test2( n );
	return n;
}

#include <cmath>
#include <immintrin.h>
#include <ctime>
#include <iostream>
#include <random>

inline float sqrtFast( float x ) {
	__m128 xvec = _mm_set_ss( x );
	xvec = _mm_sqrt_ss( xvec );
	return _mm_cvtss_f32( xvec );
}

inline double sqrtFast( double x ) {
	__m128d xvec = _mm_set_sd( x );
	xvec = _mm_sqrt_sd( xvec, xvec );
	return _mm_cvtsd_f64( xvec );
}

inline float revSqrtFast( float x ) {
	__m128 xvec = _mm_set_ss( x );
	xvec = _mm_rsqrt_ss( xvec );
	return _mm_cvtss_f32( xvec );
}

inline float sqrtSlow( float x ) {
	return 1.0f / std::sqrt( x );
}

const int N = 1 << 14;
float a[N];
float b[N];

void testFast() {
	double begin = clock();
	float sum = 0;
	for (int i = 0; i < N; ++i) {
		float x = a[i];
		for (int j = 0; j < N; ++j) {
			sum += revSqrtFast( x + b[j] );
		}
	}
	double end = clock();
	cout << (end - begin) / CLOCKS_PER_SEC << endl;
	cout << sum << endl;
}

void testSlow() {
	double begin = clock();
	float sum = 0;
	for (int i = 0; i < N; ++i) {
		float x = a[i];
		for (int j = 0; j < N; ++j) {
			sum += sqrtSlow( x + b[j] );
		}
	}
	double end = clock();
	cout << (end - begin) / CLOCKS_PER_SEC << endl;
	cout << sum << endl;
}

void testSqrt() {
	mt19937_64 gen( 42 );
	for (int i = 0; i < N; ++i) {
		a[i] = std::uniform_real_distribution<float>( 0.0f, 1e10f )(gen);
		b[i] = std::uniform_real_distribution<float>( 0.0f, 1e10f )(gen);
	}
	testSlow();
	testFast();
}

int main() {
	//TestVis();
	//BenchCol();
	//BenchCol2();
	//TestChrono();
	//VisPrediction();
	//TestSpeed();
	//TestRobot();

	//int n = test1( 30 );

	//double p2 = pow( -3.0, 3 );

	testSqrt();

	//double f = sqrtFast( -1.0f );
	//double f2 = sqrt( -1.0f );
	//double f = 0;

	//cout << f << " " << f2 << endl;

	return 0;


	//double a = ToDegree( Vec2( 0, 1 ).Angle( Vec2( 1,0 )) - M_PI );
	//double a2 = ToDegree( Vec2( 0, 1 ).Angle( Vec2( -1,0 ) ) - M_PI);

	//vector<int> v = { 0,1,2,3,4,5,6,7,8, 1 , 1 , 1  };

	////FilterArray( v, []( int a ) { return a > 3;  } );

	////v.erase( std::remove_if( v.begin(), v.end(), []( int a ) { return a > 3;  } ), v.end() );

	////Filter( v, []( int a ) { return a % 2 == 0;  } );
	//Unique( v );

	//Timer timer,timer2;

	//double f;

	//for (int i = 0; i < 1e6; i++) {

	//}

	return 0;
}