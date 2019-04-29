#include <iostream>
#include <fstream>
#include <algorithm>  
#include <thread>
#include <mutex>

#pragma warning (disable : 4996)
using namespace std;

std::mutex mute;
condition_variable ready;
bool data_ready = true, complete=false;

void separation(char* name, uint64_t kol)
{
	unique_lock<std::mutex> lock(mute);
	uint64_t tmp, k=1;
	ifstream f;
	ofstream f1, f2;
	while (k < kol) {
		while(!data_ready)
			ready.wait(lock);
		if (complete)
			break;
		f.open(name);
		f1.open("smsort_1");
		f2.open("smsort_2");
		if (!f.eof()) f >> tmp;
		while (!f.eof()) {
			for (uint64_t i = 0; i < k && !f.eof(); ++i) {
				f1 << tmp << ' ';
				f >> tmp;
			}
			for (uint64_t j = 0; j < k && !f.eof(); ++j) {
				f2 << tmp << ' ';
				f >> tmp;
			}
		}
		f.close();
		f1.close();
		f2.close();
		data_ready = false;
		k *= 2;
		ready.notify_one();
	}
	f.close();
	f1.close();
	f2.close();
}


void merge(char* name, uint64_t kol) 
{
	unique_lock<std::mutex> lock(mute);
	uint64_t a1, a2, k = 1;
	ifstream f1, f2;
	ofstream f;
	while (k < kol) {
		while (data_ready)
			ready.wait(lock);
		f.open(name);
		f1.open("smsort_1");
		f2.open("smsort_2");
		if (!f1.eof()) f1 >> a1; else break;
		if (!f2.eof()) f2 >> a2; else break;
		while (!f1.eof() && !f2.eof()) {
			uint64_t i = 0;
			uint64_t j = 0;
			while (i < k && j < k && !f1.eof() && !f2.eof()) {
				if (a1 < a2) {
					f << a1 << ' ';
					f1 >> a1;
					i++;
				}
				else {
					f << a2 << ' ';
					f2 >> a2;
					j++;
				}
			}
			while (i < k && !f1.eof()) {
				f << a1 << ' ';
				f1 >> a1;
				i++;
			}
			while (j < k && !f2.eof()) {
				f << a2 << ' ';
				f2 >> a2;
				j++;
			}
		}
		while (!f1.eof()) {
			f << a1 << ' ';
			f1 >> a1;
		}
		while (!f2.eof()) {
			f << a2 << ' ';
			f2 >> a2;
		}
		f2.close();
		f1.close();
		f.close();
		k *= 2;
		data_ready = true;
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

bool Simple_Merging_Sort(char* name)
{
	FILE* f1 = fopen(name, "rb");
	if (feof(f1))
		return false;
	uint64_t  N = 100, K = 0;
	ofstream f2("data.txt");
	uint64_t* buf = new uint64_t[N];
	while (!feof(f1)) {
		uint64_t end = fread(reinterpret_cast<char*>(buf), sizeof(uint64_t), N, f1);
		K += end;
		sort(buf, buf + end);
		for (auto in = buf; in != buf + end; in++)
			f2 << *in << ' ';
	}
	delete[] buf;
	fclose(f1);
	f2.close();
	std::thread t1(separation,(char*)"data.txt", K);
	std::thread t2(merge,(char*)"data.txt", K);
	t1.join();
	t2.join();
	return true;
}

int main()
{
	cout << Simple_Merging_Sort((char*)"data.dat") << endl;
	return 0;
}

