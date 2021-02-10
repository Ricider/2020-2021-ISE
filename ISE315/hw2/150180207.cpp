#include <iostream>
#include <fstream>
#include <string>
#include <math.h>  
#include <stdlib.h> 
#include <vector>
#include <math.h>
#include <string.h>
#include <ctime>

using namespace std;
float target_lat, target_lon;

float rad(float degrees)
{
	return 2 * M_PI * (degrees/360);
}

class Position
{
public:
	string name;
	float latitude, longtitude, distance;

	Position() : name(""), latitude(0), longtitude(0), distance(0) {}

	Position(string _name, float _latitude, float _longtitude)
		: name(_name), latitude(_latitude), longtitude(_longtitude)
	{
		distance = this->distance_from_target();
	}

	float distance_from_target()
	{
		float dlon, dlat, a, c, d, lon1, lon2, lat1, lat2, R = 6373;
		lon2 = rad(longtitude);
		lon1 = rad(target_lon);
		lat2 = rad(latitude);
		lat1 = rad(target_lat);
		//this part is from the website mentioned in the pdf
		dlon = lon2 - lon1;
		dlat = lat2 - lat1;
		a = pow(sin(dlat / 2), 2) + cos(lat1) *cos(lat2) * pow(sin(dlon / 2), 2);
		c = 2 * atan2(sqrt(a), sqrt(1 - a));
		d = R * c;

		return d;
	}

	bool operator< (const Position& other)
	{
		return this->distance < other.distance;
	}
};

void swap(vector<Position>& arr, int first,int second)
{
	Position temp = arr[first];
	arr[first] = arr[second];
	arr[second] = temp;
}

int heap_left_child(vector<Position>& arr, int index)
{
	if (arr.size() <= (unsigned int) 2 * index + 1) return -1;
	return  2 * index + 1;
}

int heap_right_child(vector<Position>& arr, int index)
{
	if (arr.size() <= (unsigned int) 2 * index + 2) return -1;
	return  2 * index + 2;
}

void heap_heapify(vector<Position>& arr, int index)
{
	int l_index = heap_left_child(arr, index);
	int r_index = heap_right_child(arr, index);

	int smallest_index = arr[index] < arr[l_index] ? index : l_index;
	smallest_index = arr[smallest_index] < arr[r_index] ? smallest_index : r_index;

	if (smallest_index != index && smallest_index != -1) //the -1 is for checking if a parent exists
	{
		swap(arr, index, smallest_index);
		heap_heapify(arr, smallest_index);
	}
}

Position heap_pop(vector<Position>& arr)
{
	Position return_value = arr[0];
	arr[0] = arr[arr.size()-1];
	arr.pop_back();
	heap_heapify(arr, 0);
	return return_value;
}

void heap_build(vector<Position>& arr)
{
	int i;
	for (i = arr.size() / 2; i >= 0; i--)
	{
		heap_heapify(arr, i);
	}
}

void print_closest(vector<Position>& arr, int K, ostream& stream)
{
	heap_build(arr);
	int i;
	Position current;

	for (i = 0; i < K; i++)
	{
		current = heap_pop(arr);
		stream << current.name << " " << current.latitude << " " << current.longtitude << endl;;
	}
}

int main(int argc, char* argv[]) {
	string line;
	ifstream myfile("location.txt");
	ofstream outfile("sorted.txt");
	int N = atoi(argv[1]);
	int K = atoi(argv[2]);
	target_lat = atof(argv[3]);
	target_lon = atof(argv[4]);
	vector<Position> cities(N);
	if (myfile.is_open())
	{
		int i = 0;
		while (getline(myfile, line) && i < N)
		{
			unsigned pos = line.find("\t");
			string name = line.substr(0, pos);
			line = line.substr(pos + 1);
			pos = line.find("\t");
			float latitude = atof(line.substr(0, pos).c_str());
			line = line.substr(pos + 1);
			float longitude = atof(line.substr(0, pos).c_str());

			cities[i++] = Position(name, latitude, longitude);
		}
		if (argc == 6) //to generate excel file
		{
			clock_t start;
			int K_values[4] = {1,2,10,-1};
			ofstream null("/dev/null");
			ofstream result_file("results.csv");
			vector<Position> copy_list(cities);

			result_file << ",1,2,10,n/2" << endl;
			for (int N=10000; N<=1000000; N+=10000)
			//for (int N = 10; N <= 1000000; N *= 10)
			{
				result_file << N << ",";
				for (int K_index=0; K_index < 4; K_index++)
				{
					cities = vector<Position>(&copy_list[0],&copy_list[N]);
					if (K_values[K_index] == -1) K = N / 2;
					else K = K_values[K_index];
					start = clock();
					print_closest(cities, K, null);
					result_file << ((float)clock() - start) / CLOCKS_PER_SEC << ",";
				}
				result_file << endl;
			}
			exit(0);
		}
		print_closest(cities, K, outfile);
	}
	else
		cout << "File could not be opened." << endl;
	return 0;
}