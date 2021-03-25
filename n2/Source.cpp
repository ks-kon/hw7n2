#include <algorithm>
#include <chrono>
#include <execution>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>


class Timer
{
public:
	using clock_t = std::chrono::steady_clock;
	using time_point_t = clock_t::time_point;
	Timer() : m_begin(clock_t::now()) {};
	~Timer()
	{
		auto end = clock_t::now();
		std::cout << "micro" << std::chrono::duration_cast <std::chrono::microseconds> (end - m_begin).count() << std::endl;
	}
private:
	time_point_t m_begin;
};


template < typename Iterator, typename T >
T parallel_accumulate(Iterator first, Iterator last, T init, int N)
{
	if (N == 1)
	{
		return std::accumulate(first, last, init);
	}

	else
	{
		const std::size_t length = std::distance(first, last);

		Iterator middle = first;
		std::advance(middle, length / N);

		std::vector <T> v(N);

		for (auto i = 0; i < v.size()-1; ++i)
		{
			std::future < T > result = std::async(std::launch::async, parallel_accumulate < Iterator, T >, first, middle, init, 1);

			std::advance(first, length / N);
			std::advance(middle, length / N);

			v[i] = result.get();
		}

		v[N-1] = std::accumulate(first, last, init);
		
		return std::accumulate(std::begin(v), std::end(v), 0);
	}
}

int main(int argc, char** argv)
{
	std::vector < int > v(100);

	std::iota(v.begin(), v.end(), 1);

	int N = 10000;

	for (auto i = 1; i < 10; ++i)
	{
		{
			Timer t;
			parallel_accumulate(v.begin(), v.end(), 0, i);
		}
		
	}

	for (auto i = 10; i < N/100; ++i)
	{
		{
			Timer t;
			parallel_accumulate(v.begin(), v.end(), 0, i * 100);
		}
	}
	


	return EXIT_SUCCESS;
}