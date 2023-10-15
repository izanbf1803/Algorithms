#include <bits/stdc++.h>
using namespace std;


template<typename T>
struct Point {
	T x, y;
	Point() {}
	Point(T x_, T y_) : x(x_), y(y_) {}
	void read() {
		cin >> x >> y;
	}
};


using ll = long long;
using ld = long double;
using pt = Point<ld>;

const ld eps = 1e-9;


struct CustomHash {
	size_t operator()(const pair<ll,ll>& p) const {
		static const uint64_t C = chrono::steady_clock::now().time_since_epoch().count();
		return C ^ ((p.first << 32) ^ p.second);
	}
};


ld dist(pt a, pt b) {
	ld dx = a.x - b.x;
	ld dy = a.y - b.y;
	return sqrt(dx*dx + dy*dy);
}


// O(n^2)
pair<pt,pt> closest_pair_of_points_naive(vector<pt> P) {
	int n = int(P.size());
 	assert(n >= 2);

	pair<pt,pt> closest = {P[0], P[1]};
	ld d = dist(P[0], P[1]);

	auto candidate_closest = [&](const pt& a, const pt& b) -> void {
		ld ab = dist(a, b);
		if (ab + eps < d) {
			d = ab;
			closest = {a, b};
		}
	};

	for (int i = 0; i < n; ++i) {
		for (int j = i+1; j < n; ++j) {
			candidate_closest(P[i], P[j]);
		}
	}

	return closest;
}


// O(n)
pair<pt,pt> closest_pair_of_points_rand_reals(vector<pt> P) {
	int n = int(P.size());
	assert(n >= 2);
	unordered_map<pair<ll,ll>,vector<pt>,CustomHash> grid;
	grid.reserve(n);

	mt19937 rd(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<int> dis(0, n-1);

	ld d = dist(P[0], P[1]);
	pair<pt,pt> closest = {P[0], P[1]};

	auto candidate_closest = [&](const pt& a, const pt& b) -> void {
		ld ab = dist(a, b);
		if (ab + eps < d) {
			d = ab;
			closest = {a, b};
		}
	};

	for (int i = 0; i < n; ++i) {
		int j = dis(rd);
		int k = dis(rd);
		while (j == k) k = dis(rd);
		candidate_closest(P[j], P[k]);
	}

	for (const pt& p : P) {
		grid[{ll(p.x/d), ll(p.y/d)}].push_back(p);
	}

	// same block
	for (const auto& it : grid) {
		int k = int(it.second.size());
		for (int i = 0; i < k; ++i) {
			for (int j = i+1; j < k; ++j) {
				candidate_closest(it.second[i], it.second[j]);
			}
		}
	}
 
	// adjacent blocks
	for (const auto& it : grid) {
		auto coord = it.first;
		for (int dx = 0; dx <= 1; ++dx) {
			for (int dy = -1; dy <= 1; ++dy) {
				if (dx == 0 and dy == 0) continue;
				pair<ll,ll> neighbour = {
					coord.first  + dx, 
					coord.second + dy
				};
				for (const pt& p : it.second) {
					if (not grid.count(neighbour)) continue;
					for (const pt& q : grid.at(neighbour)) {
						candidate_closest(p, q);
					}
				}
			}
		}
	}

	return closest;
}


void test_rand_reals(int size) {
	auto pair_dist = [&](pair<pt,pt> pp) -> ld {
		return dist(pp.first, pp.second);
	};
	auto close_enough = [&](ld x, ld y) -> bool {
		return abs(x - y) < eps;
	};
	// random tests:
	assert(size >= 10);
	mt19937 rd(3636);
	uniform_real_distribution<ld> dis_unit(0.0, 1.0);
	uniform_real_distribution<ld> dis_small(-100*eps, 100*eps);
	uniform_real_distribution<ld> dis_big(-1e18, 1e18);
	int n = 100;
	for (int it = 0; it < size; ++it) {
		vector<pt> P(n);
		for (int i = 0; i < n; ++i) {
			if (it % 3 == 0)
				P[i] = pt(dis_unit(rd), dis_unit(rd));
			else if (it % 3 == 1)
				P[i] = pt(dis_small(rd), dis_small(rd));
			else
				P[i] = pt(dis_big(rd), dis_big(rd));
		}
		assert(
			close_enough(
				pair_dist(closest_pair_of_points_naive(P)),
				pair_dist(closest_pair_of_points_rand_reals(P))
			)
		);
	}
	cerr << "rand1_reals passed " << size << " random tests of size " << n << endl;
}


void tests(int size=10000) {
	test_rand_reals(size);
}


// AC - https://open.kattis.com/problems/closestpair2
void kattis_submission() {
	cout.setf(ios::fixed);
	cout.precision(2);
	int n;
	while ((cin >> n) and n != 0) {
		vector<pt> P(n);
		for (int i = 0; i < n; ++i) P[i].read();
		auto pp = closest_pair_of_points_rand_reals(P);
		cout << pp.first.x << ' ' << pp.first.y << ' '
			<< pp.second.x << ' ' << pp.second.y << '\n';
	}
}


int main() {
	ios::sync_with_stdio(false);
	cin.tie(0);

	tests();
	// kattis_submission();
}
