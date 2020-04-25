#include <iostream>
#include <vector>
#include <iomanip> //Formatted output
#include <future> // This includes, <thread> and <time.h> as well.
using namespace std;

const double PI = 3.141592653589793;

double taskForPlusMinusEvenInterval(double starting, double end)
{
	double ret = 0;
	for (double i = starting; i < -end; i += 4)
	{
		ret += 1 / i;
		ret -= 1 / (i + 2);
		//cout << endl << "  +" << i << "   -" << i + 2;
	}
	return ret;
}

double taskForMinusPlusEvenInterval(double starting, double end)
{
	double ret = 0;
	for (double i = -starting; i < end; i += 4)
	{
		ret -= 1 / i;
		ret += 1 / (i + 2);
		//cout << endl << "  -" << i << "   +" << i + 2;
	}
	return ret;
}

double taskForMinusMinusOddInterval(double starting, double end)
{
	double ret = 0;
	double i = -starting;
	for (; i < -end - 4; i += 4)
	{
		ret -= 1 / i;
		ret += 1 / (i + 2);
		//cout << endl << "  -" << i << "  +" << i + 2;
	}
	ret -= 1 / i;
	//cout << endl << "  -" << i;
	return ret;
}

double taskForPlusPlusOddInterval(double starting, double end)
{
	double ret = 0;
	double i = starting;
	for (; i < end - 4 ; i += 4)
	{
		ret += 1 / i;
		ret -= 1 / (i+2);
		//cout << endl << "  +" << i << "  -" << i + 2;
	}
	ret += 1 / i;
	//cout << endl << "  +" << i ;
	return ret;
}

double makeCalculation(long long int numberOfThreads, long long int numberOfOperations)
{
	vector<future<double>> valuesWillBeReturnedByThreads;
	long long int intervalForEachThread = numberOfOperations / numberOfThreads * 2;		//Going to be used for calculation the denominator
	long long int totalIntervalToBeDistrubutedToThreads;
	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();	//For measuring the calculation time
	if (numberOfThreads == 1)	// No new thread will be created. All calculations will be done on the main thread
	{
		totalIntervalToBeDistrubutedToThreads = 0;
	}
	else						// New threads will be created at the amount of user requested besides the main thread. So: Total thread number = user request + 1 (main thread)
	{
		totalIntervalToBeDistrubutedToThreads = intervalForEachThread * numberOfThreads;
	}

	//Distributing calculations to threads
	if (intervalForEachThread / 2 % 2 == 0 && intervalForEachThread >= 2)		//When number of calculations per thread is an EVEN number
	{
		// Since starting index is one there is only one possibility in terms of inputs' mark in sequences: "+,-"
		for (long long int startIndexForAThread = 1; startIndexForAThread < totalIntervalToBeDistrubutedToThreads + 1; startIndexForAThread += intervalForEachThread)
		{
			valuesWillBeReturnedByThreads.push_back(async(&taskForPlusMinusEvenInterval, startIndexForAThread, -(startIndexForAThread + intervalForEachThread)));
		}
	}
	else																	//When number of calculations per thread is an ODD number
	{
		bool changeMark = true;
		for (long long int startIndexForAThread = 1; startIndexForAThread < totalIntervalToBeDistrubutedToThreads + 1; startIndexForAThread += intervalForEachThread)
		{
			// 2 possibility in terms of inputs' mark in sequences: "-,+,-" or "+,-,+"
			if (changeMark)
			{
				valuesWillBeReturnedByThreads.push_back(async(&taskForPlusPlusOddInterval, startIndexForAThread, startIndexForAThread + intervalForEachThread));
				changeMark = false;
			}
			else
			{
				valuesWillBeReturnedByThreads.push_back(async(&taskForMinusMinusOddInterval, -startIndexForAThread, -(startIndexForAThread + intervalForEachThread)));
				changeMark = true;
			}
		}
	}

	//Calculation of remaining part which wasnt distrubuted to threads
	vector<double> valuesCalculatedByMainThread;
	long long int endOfWholeInterval = numberOfOperations * 2;
	long long int numberOfOperationDoneByThreads = totalIntervalToBeDistrubutedToThreads / 2;		// Needed for determining the mark of first element in the following calculations.
	if (numberOfOperations % numberOfThreads != 0 || numberOfThreads == 1) // If there are remaining operations which wasnt distrubuted to threads or If user requested only 1 thread
	{
		if (numberOfOperationDoneByThreads % 2 == 0 && (numberOfOperations - numberOfOperationDoneByThreads) % 2 == 1)
		{
			valuesCalculatedByMainThread.push_back(taskForPlusPlusOddInterval(+(totalIntervalToBeDistrubutedToThreads + 1), endOfWholeInterval));
		}
		else if (numberOfOperationDoneByThreads % 2 == 1 && (numberOfOperations - numberOfOperationDoneByThreads) % 2 == 1)
		{
			valuesCalculatedByMainThread.push_back(taskForMinusMinusOddInterval(-(totalIntervalToBeDistrubutedToThreads + 1), -endOfWholeInterval));
		}
		else if (numberOfOperationDoneByThreads % 2 == 0 && (numberOfOperations - numberOfOperationDoneByThreads) % 2 == 0)
		{
			valuesCalculatedByMainThread.push_back(taskForPlusMinusEvenInterval(+(totalIntervalToBeDistrubutedToThreads + 1), -endOfWholeInterval));
		}
		else if (numberOfOperationDoneByThreads % 2 == 1 && (numberOfOperations - numberOfOperationDoneByThreads) % 2 == 0)
		{
			valuesCalculatedByMainThread.push_back(taskForMinusPlusEvenInterval(-(totalIntervalToBeDistrubutedToThreads + 1), endOfWholeInterval));
		}
	}

	//Calculation of Final Result
	double result = 0;
	for (long long int i = 0; i < valuesCalculatedByMainThread.size(); i++)
	{
		result += valuesCalculatedByMainThread[i];
	}
	for (long long int i = 0; i < valuesWillBeReturnedByThreads.size(); i++)
	{
		result += valuesWillBeReturnedByThreads[i].get();
	}
	result *= 4.00;

	chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
	chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
	cout.precision(15);
	cout << "Duration of the computation: " << left << setw(10) << time_span.count() << " seconds for " << setw(2) << numberOfThreads << " threads. Result: " << result << endl;

	return result;
}

int main()
{	
	cout << "Going to calculate PI number with an approximation:" << endl;
	cout << "By using Tylor Series Method (PI/4 = 1/1 - 1/3 + 1/5 - 1/7 + 1/9 ...)" << endl;
	cout << "With implementation of multi-threaded programming." << endl << endl;
	
	cout << "Type \"1\" for using program with your own inputs." << endl;
	cout << "Type \"2\" for executing auto test: Running 1 to 64 threads for 1m operations." << endl;

	char input;
	cin >> input;
	
	long long int numberOfThreads;
	long long int numberOfOperations;
	double result;

	if (input == '1')
	{
		cout << "Enter number of operations: ";
		cin >> numberOfOperations;

		cout << "Enter number of threads: ";
		cin >> numberOfThreads;
		while (numberOfThreads > numberOfOperations)
		{
			cout << endl << "Thread number can not be bigger than number of operations. Enter number of threads: ";
			cin >> numberOfThreads;
		}

		result = makeCalculation(numberOfThreads, numberOfOperations);

		cout.precision(15);
		cout << endl << "Result: " << result;
		cout << endl << "PI:     " << PI << endl;

		cout << endl << "Approximation Error: Result - PI = " << result - PI;
		cout << endl << "Approximation Error: PI - Result = " << PI - result << endl;
	}
	else if (input == '2')
	{
		int threadNo = 0;
		cout.precision(15);
		cout << "------------------------------------------------------------------- PI: " << PI << endl;
		while (threadNo<64)
		{
			threadNo++;
			makeCalculation(threadNo, 1000000);
		}
	}
	
	char end;
	cin >> end;
	return 0;
}
