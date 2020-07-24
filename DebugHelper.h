#pragma once

#include <tuple>
#include <algorithm>
#include <deque>

#include "Helpers.h"
#include "Predict.h"

#include "JsonNetwork.h"

class DebugHelper {
public:

	JsonNetwork JN;

	DebugHelper() {

	}

	void AddGameOjbects(const Predict & game) {
		JsonNetwork::Object obj, obj2;
		obj["Type"] = "Shape";
		obj["Name"] = "Ball";
		obj["Pri"] = "Sphere";

		obj["Radius"] = to_string(game.ball.radius);
		obj["Color"] = "Green";
		obj["Alpha"] = "0.7";
		obj["Pos"] = PackVec3(game.ball.position);

		JN.AddObject(obj);

		obj2["Type"] = "Line";
		obj2["Color"] = "White";
		obj2["Width"] = "0.2";
		obj2["Points"] = PackLine(game.ball.position, game.ball.position + game.ball.velocity);

		JN.AddObject(obj2);

		Plane d = game.dan_to_arena(game.ball.position);

		obj2["Color"] = "Red";
		obj2["Width"] = "0.2";
		obj2["Points"] = PackLine(game.ball.position, game.ball.position -d.Normal * d.Dist);

		JN.AddObject(obj2);

		for (int i = 0; i < game.GetNumRobots(); i++) {
			const Robot & r = game.GetRobot( i );

			obj["Name"] = "Robot";
			obj["Id"] = to_string(r.id);
			obj["Radius"] = to_string(r.radius);
			if (r.is_teammate) {
				obj["Color"] = "Blue";
			}
			else {
				obj["Color"] = "Red";
			}
			obj["Pos"] = PackVec3(r.position);
			JN.AddObject(obj);

			obj2["Color"] = "White";
			obj2["Name"] = "Velocity";
			obj2["Points"] = PackLine( r.position, r.position + r.velocity );
			JN.AddObject( obj2 );

			//obj2["Color"] = "Green";
			//obj2["Points"] = PackLine( r.position, r.position + r.action.target_velocity );
			//obj2["Id"] = to_string( r.id );
			//obj2["Name"] = "TargetVelocity";
			//JN.AddObject( obj2 );
		}

		for (int i = 0; i < game.GetNumNitroPacks(); i++ ) {
			const NitroPack & n = game.GetNitroPack( i );

			if (!n.alive) continue;

			obj["Name"] = "Nitro";
			obj["Id"] = to_string(n.id);
			obj["Radius"] = to_string(n.radius);
			obj["Color"] = "Yellow";
			obj["Pos"] = PackVec3(n.position);
			JN.AddObject(obj);
		}

		JN.AddCommand("SetScore", to_string(game.ScorePlayer1) + " " + to_string(game.ScorePlayer2));

		JN.AddCommand("Write", "Ball speed: " + to_string(game.ball.velocity.Length()));
		//JN.AddCommand("Write", "Time: " + to_string(msec));
		//if (pr.ScoredEnemy) {
		//	JN.AddCommand("Write", "Enemy scored");
		//}
		//if (pr.ScoredSelf) {
		//	JN.AddCommand("Write", "Self scored");
		//}

		//JN.SendFrameData();
	}

	void AddPredict(const Predict & game) {
		JsonNetwork::Object obj, obj2;

		Predict pr(game);
		//pr.SetArenaDefault();
		pr.UseRobots = false;

		obj["Type"] = "Shape";
		obj["Pri"] = "Sphere";
		obj["Name"] = "Ghost";
		obj["Radius"] = to_string(pr.ball.radius);
		obj["Color"] = "Magenta";
		obj["Alpha"] = "0.3";

		int msec = 0;

		pr.SetMicroTicks( 30 );

		if (pr.ball.velocity.Length() > 0.0) {
			for (int i = 0; i < 20; i++) {

				HrcTimePoint t1 = std::chrono::high_resolution_clock::now();
				pr.ticks(10);
				HrcTimePoint t2 = std::chrono::high_resolution_clock::now();
				msec += (int)std::chrono::duration_cast<Msec>(t2 - t1).count();

				obj["Id"] = to_string(i);
				obj["Pos"] = PackVec3(pr.ball.position.x, pr.ball.position.y, pr.ball.position.z);
				JN.AddObject(obj);

				Plane d = pr.dan_to_arena(pr.ball.position);

				obj2["Type"] = "Line";
				obj2["Color"] = "Red";
				obj2["Width"] = "0.2";
				obj2["Points"] = PackLine(pr.ball.position, pr.ball.position - d.Normal * d.Dist);
				JN.AddObject(obj2);
			}
		}
	}

	void AddSphere(Vec3 pos, double radius, string c, double a = 1.0) {
		JsonNetwork::Object obj;
		obj["Type"] = "Shape";
		obj["Name"] = "Ball";
		obj["Pri"] = "Sphere";
		obj["Radius"] = to_string(radius);
		obj["Alpha"] = to_string(a);
		obj["Color"] = c;
		obj["Pos"] = PackVec3(pos);
		JN.AddObject(obj);
	}

	void AddLine(Vec3 a, Vec3 b, string c) {
		JsonNetwork::Object obj;
		obj["Type"] = "Line";
		obj["Color"] = c;
		obj["Width"] = "0.2";
		obj["Points"] = PackLine(a, b);
		JN.AddObject(obj);
	}

	//void AddRobot(const Robot & r) {

	//}
	//void AddBall(const Ball & b) {

	//}

	void AddParameter(const string & n, const string & v) {
		JN.AddCommand("Write", n + " : " + v);
	}

	void AddParameter(const string & n, double d) {
		AddParameter(n, to_string(d));
	}

	void AddParameter(const string & n, int d) {
		AddParameter(n, to_string(d));
	}

	void WriteLine() {

	}

	void FinishFrame() {
		JN.Submit();
	}

};
