#pragma once

class Plane;
class Vec3;
class Vec2;

inline Plane dan_to_sphere_inner(Vec3 InPoint, Vec3 InCenter, double Radius) {
	return Plane(Vec3(InCenter - InPoint).Normalized(), Radius - (InPoint - InCenter).Length());
}

inline Plane dan_to_sphere_outer(Vec3 InPoint, Vec3 InCenter, double Radius) {
	return Plane(Vec3(InPoint - InCenter).Normalized(), (InPoint - InCenter).Length() - Radius);
}

inline Plane dan_to_plane(Vec3 p, Vec3 pop, Vec3 norm) {

}

Plane dan_to_arena_quarter(Vec3 point) {
	double aw, ah, ad, br, tr, cr, gtr, gw, gh, gd, gsr;

	// Ground
	Plane dan = dan_to_plane(point, Vec3(0, 0, 0), Vec3(0, 1, 0));

	// Ceiling
	dan = min(dan, dan_to_plane(point, Vec3(0, ah, 0), Vec3(0, -1, 0)));

	// Side x
	dan = min(dan, dan_to_plane(point, Vec3(aw / 2, 0, 0), Vec3(-1, 0, 0)));

	// Side z (goal)
	dan = min(dan, dan_to_plane(point, Vec3(0, 0, (ad / 2) + gd), Vec3(0, 0, -1)));

	// Side z
	Vec2 v = Vec2(point.x, point.y) - Vec2((gw / 2) - gtr, gh - gtr);
	if (point.x >= (gw / 2) + gsr || point.y >= gh + gsr || (v.x > 0 && v.y > 0 && v.Len() >= gtr + gsr)) {
		dan = min(dan, dan_to_plane(point, Vec3(0, 0, ad / 2), Vec3(0, 0, -1)));
		// Side x & ceiling (goal)
		if (point.z >= (ad / 2) + gsr) {
			// x
			dan = min(dan, dan_to_plane(point, Vec3(gw / 2, 0, 0), Vec3(-1, 0, 0)));
			// y
			dan = min(dan, dan_to_plane(point, Vec3(0, gh, 0), Vec3(0, -1, 0)));
		}
	}

	// Goal back corners
	if (point.z > (ad / 2) + gd - br) {
		dan = min(dan, dan_to_sphere_inner(point, Vec3(clamp(point.x, br - (gw / 2), (gw / 2) - br, ), clamp(point.y, br, gh - gtr, ), (ad / 2) + gd - br), br));
	}

	// Corner
	if (point.x > (aw / 2) - cr && point.z > (ad / 2) - cr) {
		dan = min(dan, dan_to_sphere_inner(point, ((aw / 2) - cr, point.y, (ad / 2) - cr), cr))
	}


	// Goal outer corner
	if (point.z < (ad / 2) + gsr) {
		// Side x
		if (point.x < (gw / 2) + gsr) {
			dan = min(dan, dan_to_sphere_outer(point, ((gw / 2) + gsr, point.y, (ad / 2) + gsr), gsr));
			// Ceiling
			if (point.y < gh + gsr) {
				dan = min(dan, dan_to_sphere_outer(point, (point.x, gh + gsr, (ad / 2) + gsr), gsr));
			}
			// Top corner
			Vec2 o = Vec2((gw / 2) - gtr, gh - gtr)
				Vec2 v = Vec2(point.x, point.y) - o;
			if (v.x > 0 && v.y > 0) {
				Vec2 o = o + normalize(v) * (gtr + gsr);
				dan = min(dan, dan_to_sphere_outer(point, (o.x, o.y, (ad / 2) + gsr), gsr));
			}
		}
	}


	// Goal inside top corners
	if (point.z > (ad / 2) + gsr && point.y > gh - gtr) {
		// Side x
		if (point.x > (gw / 2) - gtr) {
			dan = min(dan, dan_to_sphere_inner(point, ((gw / 2) - gtr, gh - gtr, point.z), gtr))
				// Side z
				if (point.z > (ad / 2) + gd - gtr) {
					dan = min(dan, dan_to_sphere_inner(point, (point.x, gh - gtr, (ad / 2) + gd - gtr), gtr));
				}
		}
	}

	// Bottom corners
	if (point.y < br) {
		// Side x
		if (point.x > (aw / 2) - br) {
			dan = min(dan, dan_to_sphere_inner(point, ((aw / 2) - br, br, point.z), br));
			// Side z
			if (point.z > (ad / 2) - br && point.x >= (gw / 2) + gsr) {
				dan = min(dan, dan_to_sphere_inner(point, (point.x, br, (ad / 2) - br), br));
			}
			// Side z (goal)
			if (point.z > (ad / 2) + gd - br) {
				dan = min(dan, dan_to_sphere_inner(point, (point.x, br, (ad / 2) + gd - br), br));
				// Goal outer corner
				Vec2 o = ((gw / 2) + gsr, (ad / 2) + gsr)	let v = (point.x, point.z) - o;
				if (v.x < 0 && v.y < 0 && length(v) < gsr + br) {
					Vec2 o = o + normalize(v) * (gsr + br);
					dan = min(dan, dan_to_sphere_inner(point, (o.x, br, o.y), br));
				}
				// Side x (goal)
				if (point.z >= (ad / 2) + gsr && point.x > (gw / 2) - br) {
					dan = min(dan, dan_to_sphere_inner(point, ((gw / 2) - br, br, point.z), br));
				}
				// corner
				if (point.x > (aw / 2) - cr && point.z > (ad / 2) - cr) {
					Vec2 corner_o = Vec2((aw / 2) - cr, (ad / 2) - cr);
					Vec2 n = Vec2(point.x, point.z) - corner_o;
					double dist = n.len();
					if (dist > cr - br) {
						Vec2 n = n / dist;
						Vec2 o2 = corner_o + n * (cr - br);
						dan = min(dan, dan_to_sphere_inner(point, (o2.x, br, o2.y), br));
					}
				}
			}
		}
	}


	// Ceiling corners
	if (point.y > ah - tr) {
		// Side x
		if (point.x > (aw / 2) - tr) {
			dan = min(dan, dan_to_sphere_inner(point, ((aw / 2) - tr, ah - tr, point.z, ), tr));
		}
		// Side z
		if (point.z > (ad / 2) - tr) {
			dan = min(dan, dan_to_sphere_inner(point, (point.x, ah - tr, (ad / 2) - tr, )	tr));
		}

		// Corner
		if (point.x > (aw / 2) - cr && point.z > (ad / 2) - cr) {
			Vec2 corner_o = ((aw / 2) - cr, (ad / 2) - cr);
			Vec2 dv = (point.x, point.z) - corner_o;
			if (length(dv) > cr - tr) {

				Vec2 n = normalize(dv);
				Vec2 o2 = corner_o + n * (cr - tr);
				dan = min(dan, dan_to_sphere_inner(point, (o2.x, ah - tr, o2.y), tr));
			}
		}
	}
	return dan;
}