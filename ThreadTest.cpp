
#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <future>

volatile int imaginaryResource;
std::mutex imaginaryResourceLock;
int jobs = 100;
const int difficulty = 1000;

int DoSomeWork(int iterations) {
	for(int i = 0; i < iterations; ++i) {
		std::vector<int> v(difficulty, 1);
		imaginaryResource = std::accumulate(v.begin(), v.end(), 0u); // make sure it's a side effect
	}
	return imaginaryResource;
}

int DoSomeWorkWithLock(int iterations) {
	std::lock_guard<std::mutex> guard(imaginaryResourceLock);
	return DoSomeWork(iterations);
}

int main() {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> diff;


	{
		start = std::chrono::system_clock::now();
		DoSomeWork(jobs);
		end = std::chrono::system_clock::now();
		diff = end - start;
		std::cout << "Time to do jobs:" << jobs << " of difficulty:" << difficulty << " is : " << diff.count() * 1000. << " ms\n";
	}
	{
		start = std::chrono::system_clock::now();
		auto asyncWork = std::async(std::launch::async, DoSomeWork, jobs);
		asyncWork.wait();
		end = std::chrono::system_clock::now();
		diff = end - start;
		std::cout << "Time to do async jobs:" << jobs << " of difficulty:" << difficulty << " is : " << diff.count() * 1000. << " ms\n";
	}
	{
		start = std::chrono::system_clock::now();
		auto asyncWork = std::async(std::launch::async, DoSomeWorkWithLock, jobs);
		asyncWork.wait();
		end = std::chrono::system_clock::now();
		diff = end - start;
		std::cout << "Time to do async work with a lock jobs:" << jobs << " of difficulty:" << difficulty << " is : " << diff.count() * 1000. << " ms\n";
	}
	{
		start = std::chrono::system_clock::now();
		std::vector<std::future<int>> allTheWork{};
		int workIterator = jobs;
		do {
			allTheWork.push_back(std::async(std::launch::async, DoSomeWork, 1));
		} while(workIterator-- > 0);
		for(int i = 0; i < jobs; ++i) {
			allTheWork[i].wait();
		}
		end = std::chrono::system_clock::now();
		diff = end - start;
		std::cout << "Time to do async jobs with thread for each job with the busted stompy method:" << jobs << " of difficulty:" << difficulty << " is : " << diff.count() * 1000. << " ms\n";
	}

	{
		start = std::chrono::system_clock::now();
		std::vector<std::future<int>> allTheWork{};
		int workIterator = jobs;
		do {
			allTheWork.push_back(std::async(std::launch::async, DoSomeWorkWithLock, 1));
		} while(workIterator-- > 0);
		for(int i = 0; i < jobs; ++i) {
			allTheWork[i].wait();
		}
		end = std::chrono::system_clock::now();
		diff = end - start;
		std::cout << "Time to do async work with thread for each job with a resource lock:" << jobs << " of difficulty:" << difficulty << " is : " << diff.count() * 1000. << " ms\n";
	}
}




