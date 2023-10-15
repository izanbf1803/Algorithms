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
using pt = Point<ll>;


struct CustomHash {
	size_t operator()(const pair<ll,ll>& p) const {
		static const uint64_t C = chrono::steady_clock::now().time_since_epoch().count();
		return C ^ ((p.first << 32) ^ p.second);
	}
};


ll dist2(pt a, pt b) {
	ll dx = a.x - b.x;
	ll dy = a.y - b.y;
	return dx*dx + dy*dy;
}


// O(n^2)
pair<pt,pt> closest_pair_of_points_naive(vector<pt> P) {
	int n = int(P.size());
 	assert(n >= 2);

	pair<pt,pt> closest = {P[0], P[1]};
	ll d2 = dist2(P[0], P[1]);

	auto candidate_closest = [&](const pt& a, const pt& b) -> void {
		ll ab2 = dist2(a, b);
		if (ab2 < d2) {
			d2 = ab2;
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
pair<pt,pt> closest_pair_of_points_rand_ints(vector<pt> P) {
	int n = int(P.size());
	assert(n >= 2);
	unordered_map<pair<ll,ll>,vector<pt>,CustomHash> grid;
	grid.reserve(n);

	mt19937 rd(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<int> dis(0, n-1);

	ll d2 = dist2(P[0], P[1]);
	pair<pt,pt> closest = {P[0], P[1]};

	auto candidate_closest = [&](const pt& a, const pt& b) -> void {
		ll ab2 = dist2(a, b);
		if (ab2 < d2) {
			d2 = ab2;
			closest = {a, b};
		}
	};

	for (int i = 0; i < n; ++i) {
		int j = dis(rd);
		int k = dis(rd);
		while (j == k) k = dis(rd);
		candidate_closest(P[j], P[k]);
	}

	ll d = ll( sqrt(ld(d2)) + 1 );

	for (const pt& p : P) {
		grid[{p.x/d, p.y/d}].push_back(p);
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


void test_rand_ints(int size) {
	auto pair_dist = [&](pair<pt,pt> pp) -> ll {
		return dist2(pp.first, pp.second);
	};
	// random tests:
	assert(size >= 10);
	mt19937 rd(53426);
	uniform_int_distribution<> dis(-10, 10);
	int n = 100;
	for (int it = 0; it < size; ++it) {
		vector<pt> P(n);
		for (int i = 0; i < n; ++i) {
			P[i] = pt(dis(rd), dis(rd));
		}
		assert(
			pair_dist(closest_pair_of_points_naive(P))
			== pair_dist(closest_pair_of_points_rand_ints(P))
		);
	}
	cerr << "rand1_ints passed " << size << " random tests of size " << n << endl;
}


void tests(int size=10000) {
	test_rand_ints(size);
}


// AC - https://cses.fi/problemset/task/2194/
void cses_submission() {
	int n;
	cin >> n;
	vector<pt> P(n);
	for (int i = 0; i < n; ++i) P[i].read();
	auto pp = closest_pair_of_points_rand_ints(P);
	cout << dist2(pp.first, pp.second) << '\n';
}


int main() {
	ios::sync_with_stdio(false);
	cin.tie(0);

	tests();
	// cses_submission();
}
