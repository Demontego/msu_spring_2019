#include<iostream>
#include<vector>
#include<utility>
#include "numbers.dat"
#include <cmath>

void Eratosfen(std::vector<bool>& a)
{
	long i, j, d, bound = sqrt((double)a.size());
	a[0] = a[1] = 0; a[2] = 1;
	for (i = 4; i < a.size(); i += 2)
		a[i] = 0;
	for (i = 3; i < a.size(); i += 2)
		a[i] = 1;
	i = 3;
	while (i <= bound)
	{
		d = i << 1;
		for (j = i * i; j < a.size(); j += d)
			a[j] = 0;
		i += 2;
		while (i <= bound && !a[i])
			i += 2;
	}
}

std::pair<int,int> find(const int& start, const int& end)
{
	int i = 0;
	while (i < Size) {
		if (Data[i] == start) {
			int j = i;
			while (j < Size) {
				if (Data[j] == end)
					return std::pair<int, int>(i, j);
				++j;
			}
		}
		++i;
	}
	return std::pair<int, int>(-1,-1);

}

int main(int argc, char** argv)
{
	std::vector<bool> v(100000);
	Eratosfen(v);
	if (argc < 3 || argc % 2 == 0) return -1;
	for (int i = 1; i < argc; i += 2)
	{
		int count = 0;
		std::pair<int,int> ind=find(std::atoi(argv[i]), std::atoi(argv[i + 1]));
		for (int i = ind.first; i <= ind.second; ++i)
			count += v[Data[i]];
		std::cout << count << std::endl;
	}
	//system("pause");
	return 0;
}
