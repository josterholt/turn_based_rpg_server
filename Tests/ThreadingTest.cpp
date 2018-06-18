#include "stdafx.h"
#include "CppUnitTest.h"
#include <omp.h>
// using omp_get_max_threads

#include <cmath>
// using std::fabsf, HUGE_VALF
#include <chrono>
// using std::chrono::*
#include <random>
// using std::default_random_engine, std::seedq, std::uniform_*_distribution
#include <iostream>
// using std::cout

#include <boost/thread/mutex.hpp>
// using boost::mutex
#include <boost/thread/shared_mutex.hpp>
// using boost::shared_mutex
#include <boost/thread/lock_guard.hpp>
// using boost::lock_guard
#include <boost/thread/shared_lock_guard.hpp>
// using boost::shared_lock_guard

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace {

#ifdef USE_RWLOCK
	using mutex_type = boost::shared_mutex;
	using read_lock_type = boost::shared_lock_guard<mutex_type>;
#else
	using mutex_type = boost::mutex;
	using read_lock_type = boost::lock_guard<mutex_type>;
#endif
	using write_lock_type = boost::lock_guard<mutex_type>;

	enum class Direction { up, down, left, right };

	struct Vector2f
	{
		float x, y;
		constexpr Vector2f(float x = 0.f, float y = 0.f) noexcept
			: x(x), y(y)
		{}
	};

	struct UnitState
	{
		Vector2f position, velocity;
		Direction facing;

		constexpr UnitState() noexcept
			: position(), velocity(), facing(Direction::up)
		{}
	};

	class GameUnit
	{
		mutable mutex_type mutex;
		UnitState state;
		float maxSpeed;

	public:
		GameUnit() noexcept
			: mutex(), state(), maxSpeed(HUGE_VALF)
		{}
		UnitState getFullState() const
		{
			read_lock_type lock(mutex);
			return state;
		}
		void updatePosition(float x, float y, Direction facing)
		{
			write_lock_type lock(mutex);
			if (std::fabs(state.position.x - x) > maxSpeed
				|| std::fabs(state.position.y - y) > maxSpeed)
				return;
			state.position.x = x;
			state.position.y = y;
			state.facing = facing;
		}
	};

}

namespace Tests
{
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(TestMethod1)
		{
			using clock_t = std::chrono::steady_clock;
			constexpr int maxReadsToWrites = 1000;
			constexpr std::chrono::seconds seconds(10);
			const clock_t::duration clock_seconds =
				std::chrono::duration_cast<clock_t::duration>(seconds);
			const int maxThreads = omp_get_max_threads();
			std::seed_seq seed;
			std::uniform_real_distribution<float> floatDistr;
			for (int readsToWrites = 1; readsToWrites <= maxReadsToWrites; readsToWrites *= 10) {
				std::uniform_int_distribution<int> intDistr(0, readsToWrites);
				for (int threads = 1; threads <= maxThreads; threads *= 2) {
					GameUnit unit;
					double opsPerSec = 0.;
					const clock_t::time_point startTime = clock_t::now();
#     pragma omp parallel num_threads(threads)
					{
						std::default_random_engine rnd;
#       pragma omp critical
						rnd = std::default_random_engine(seed);
						long long localOps = 0;
						clock_t::duration dt;
						Vector2f pos;
						for (localOps = 0; (dt = clock_t::now() - startTime) < clock_seconds;
							++localOps) {
							if (intDistr(rnd))
								pos = unit.getFullState().position;
							else {
								pos.x += floatDistr(rnd);
								pos.y += floatDistr(rnd);
								unit.updatePosition(pos.x, pos.y, Direction::up);
							}
						}
						const double localOpsPerSec = localOps /
							(std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count()
								* 1e-9);
#       pragma omp critical
						opsPerSec += localOpsPerSec;
					}
					std::string message = std::to_string(readsToWrites) + " reads per write "
						+ std::to_string(threads) + " threads "
						+ std::to_string(opsPerSec) + " operations per second\n";
					Logger::WriteMessage(message.c_str());
				}
			}
		}
	};
}