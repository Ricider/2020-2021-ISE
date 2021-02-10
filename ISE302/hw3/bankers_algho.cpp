/*********************************************************************************
Developement environment: Ubuntu 18.04 on WSL (works on itu ssh servers)
To compile: g++ bankers_algho.cpp -o bankers_algho
To run: ./bankers_algho input.txt
*********************************************************************************/

#include <iostream> 
#include <fstream>
#include <vector>

using namespace std;

typedef struct { int max_request, has; } process;

bool bankers_alghoritm(vector<process>& resource_vector, int total, vector<int>& execution_order)
{
	int i;
	int free = total;
	vector<bool> may_not_finish(resource_vector.size(), true);
	vector<int> remaining_request(resource_vector.size(), 0);

	execution_order.clear();

	for (i = 0; i < resource_vector.size(); i++)
	{
		free -= resource_vector[i].has;
		remaining_request[i] = resource_vector[i].max_request - resource_vector[i].has;
	}

	bool flag = true;

	while (flag)
	{
		flag = false;
		for (i = 0; i < resource_vector.size(); i++)
		{
			if (may_not_finish[i] && remaining_request[i] <= free)
			{
				execution_order.push_back(i);

				may_not_finish[i] = false;
				free += resource_vector[i].has;
				flag = true;
			}
		}
	}
	if (free == total) //is safe
	{
		return true;
	}
	else //is not safe
	{
		return false;
	}
}

int main(int argc, char* argv[])
{
	ifstream input_file(argv[1]);
	int total, i, proc_num;
	vector<process> resource_vector;
	input_file >> total;
	string input;

	int max_request, has;
	while (input_file >> max_request && input_file >> has)
		resource_vector.push_back({ max_request,has });

	vector<int> execution_order, backup_execution_order;
	if (!bankers_alghoritm(resource_vector, total, execution_order))
	{
		cout << "Input state is UNSAFE, Terminating program." << endl;
	}

	else
	{
		cout << "Input state is SAFE,";
		for (;;)
		{
			cout << " Enter E to execute, R to request." << endl;
			cin >> input;

			if (input == "E")
			{
				cout << "A correct process execution order is ";
				for (i = 0; i < execution_order.size(); i++)
				{
					cout << execution_order[i];
					if (i == execution_order.size() - 1) break;
					cout << "->";
				}
			}

			else if (input == "R")
			{
				cout << "Which process requests the resource?" << endl;
				cin >> proc_num;
				cout << "How many resources does process " << proc_num << " request?" << endl;
				cin >> i;

				resource_vector[proc_num].has += i;

				if (bankers_alghoritm(resource_vector, total, execution_order))
				{
					cout << "Request CAN be granted.";
				}
				else
				{
					cout << "Request CANNOT be granted.";
				}

				resource_vector[proc_num].has -= i;
			}
		}
	}
}
