#include <iostream>
#include <fstream>
#include <algorithm>  
#include <thread>
#include <mutex>
#include <chrono>

#pragma warning (disable : 4996)

uint64_t  N = 1000000;
using namespace std;

class Timer
{
	using clock_t = std::chrono::high_resolution_clock;
	using microseconds = std::chrono::microseconds;
public:
	Timer()
		: start_(clock_t::now())
	{
	}

	~Timer()
	{
		const auto finish = clock_t::now();
		const auto us =
			std::chrono::duration_cast<microseconds>
			(finish - start_).count();
		std::cout << us << " us" << std::endl;
	}

private:
	const clock_t::time_point start_;
};

std::mutex mute;
condition_variable ready;
bool dataReady = true, complete = false;

void separation(const char* name, uint64_t kol)
{
	unique_lock<std::mutex> lock(mute);
	uint64_t tmp, k = N;
	ifstream f;
	ofstream f1, f2;
	while (k < kol) {
		while (!dataReady)
			ready.wait(lock);
		if (complete)
			break;
		f.open(name, ios::binary);
		f1.open("smsort1", ios::binary);
		f2.open("smsort2", ios::binary);
		if (!f.eof())  f.read(reinterpret_cast<char*>(&tmp), sizeof(uint64_t));
		while (!f.eof()) {
			for (uint64_t i = 0; i < k && !f.eof(); ++i) {
				f1.write(reinterpret_cast<char*>(&tmp), sizeof(uint64_t));
				f.read(reinterpret_cast<char*>(&tmp), sizeof(uint64_t));
			}
			for (uint64_t j = 0; j < k && !f.eof(); ++j) {
				f2.write(reinterpret_cast<char*>(&tmp), sizeof(uint64_t));
				f.read(reinterpret_cast<char*>(&tmp), sizeof(uint64_t));
			}
		}
		f.close();
		f1.close();
		f2.close();
		dataReady = false;
		k *= 2;
		ready.notify_one();
	}
	f.close();
	f1.close();
	f2.close();
}


void merge(const char* name, uint64_t kol)
{
	unique_lock<std::mutex> lock(mute);
	uint64_t a1, a2, k = N;
	ifstream f1, f2;
	ofstream f;
	while (k < kol) {
		while (dataReady)
			ready.wait(lock);
		f.open(name, ios::binary);
		f1.open("smsort_1", ios::binary);
		f2.open("smsort_2", ios::binary);
		if (!f1.eof()) f1.read(reinterpret_cast<char*>(&a1), sizeof(uint64_t)); else break;
		if (!f2.eof()) f2.read(reinterpret_cast<char*>(&a2), sizeof(uint64_t)); else break;
		while (!f1.eof() && !f2.eof()) {
			uint64_t i = 0;
			uint64_t j = 0;
			while (i < k && j < k && !f1.eof() && !f2.eof()) {
				if (a1 < a2) {
					f.write(reinterpret_cast<char*>(&a1), sizeof(uint64_t));
					f1.read(reinterpret_cast<char*>(&a1), sizeof(uint64_t));
					i++;
				}
				else {
					f.write(reinterpret_cast<char*>(&a2), sizeof(uint64_t));
					f2.read(reinterpret_cast<char*>(&a2), sizeof(uint64_t));
					j++;
				}
			}
			while (i < k && !f1.eof()) {
				f.write(reinterpret_cast<char*>(&a1), sizeof(uint64_t));
				f1.read(reinterpret_cast<char*>(&a1), sizeof(uint64_t));
				i++;
			}
			while (j < k && !f2.eof()) {
				f.write(reinterpret_cast<char*>(&a2), sizeof(uint64_t));
				f2.read(reinterpret_cast<char*>(&a2), sizeof(uint64_t));
				j++;
			}
		}
		while (!f1.eof()) {
			f.write(reinterpret_cast<char*>(&a1), sizeof(uint64_t));
			f1.read(reinterpret_cast<char*>(&a1), sizeof(uint64_t));
		}
		while (!f2.eof()) {
			f.write(reinterpret_cast<char*>(&a2), sizeof(uint64_t));
			f2.read(reinterpret_cast<char*>(&a2), sizeof(uint64_t));
		}
		f2.close();
		f1.close();
		f.close();
		k *= 2;
		dataReady = true;
		ready.notify_one();
	}
	f2.close();
	f1.close();
	f.close();
	complete = true;
	remove("smsort_1");
	remove("smsort_2");
	ready.notify_one();
}

bool simpleMergeSort(const char* name)
{
	ifstream f1(name, ios::binary);
	if (f1.eof())
		return false;
	uint64_t  K = 0;
	ofstream f2("data_s2.dat", ios::binary);
	uint64_t* buf = new uint64_t[N];
	while (!f1.eof()) {
		f1.read(reinterpret_cast<char*>(buf), sizeof(uint64_t)*N);
		K += f1.gcount()/ sizeof(uint64_t);
		sort(buf, buf + f1.gcount()/ sizeof(uint64_t));
		f2.write(reinterpret_cast<char*>(buf), f1.gcount());
	}
	delete[] buf;
	f1.close();
	f2.close();
	std::thread t1(separation, "data_s2.dat", K);
	std::thread t2(merge, "data_s2.dat", K);
	t1.join();
	t2.join();
	return true;
}

int main()
{
	{
		Timer t;
		cout << simpleMergeSort("my_data.dat") << endl;
	}
	return 0;
}
