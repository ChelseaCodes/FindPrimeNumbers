/******************************************************************************
* Author:			Chelsea Christison
* Date Created:		2019.05.03
* Modified:			2019.07.03 - refined comments.
*
* Assignment:	Find Prime Numbers 
*
* Overview:
*	This program executes three threads:
*	Main Program - reads a list of numbers from a STL collection, halves list 
*					and checks prime on two async tasks for each half.  
*					each async task will check and return either the number, 
*					or -1 if not prime. Finally, prgram writes all found primes
*					to primenumbers.txt. 
*	async1 - check primes of first half of list, one num at a time
*	async2 - check primes of second half of list, one num at a time. 
*	
* Input:
*	The input consists of a text file, numbers.txt located under Resource Files
*	Format of input can be new line per num, comma separated or space separated.
*
* Output:
*	A text file called primenumbers.txt located under ResourceFiles 
*
* Dan's Note: 
* Please write a C++ program that reads a list of numbers from a STL collection
* and for each number determine if the number is prime. 
*
* The prime check should occur in a secondary thread and put the results
* in a STL data structure. 
*
* Use two worker threads to perform the calculations(one prime number per thread). 
* Please provide a Xcode or Visual Studio project along with source files.
*
* Chelsea's Notes: 
* One number per thread is limiting. why not send each worker thread a vector
* and have both threads add to a protected shared vector. 
******************************************************************************/
#include "pch.h"

#include <iostream>
using std::cout; 
using std::endl; 
using std::ios; 

#include <vector>
using std::vector;

#include <string> 
using std::string;

#include<fstream>
using std::ifstream;
using std::ofstream; 

#include<future>
using std::future; 

/******************************************
* checkPrime is called by async 
* it returns n if n is prime, and -1 
* if not. 
******************************************/
int checkPrime(int n) {
	if (n <= 1) return -1; 
	if (n <= 3) return n; 

	//is it divisible by 2 or 3
	if (n % 2 == 0 || n % 3 == 0) return -1;

	//check if n is divisible by all nums 
	//of form 6k +- 1
	for (int i(5); i*i <= n; i += 6) {
		if (n % i == 0 || n % (i + 2) == 0)
			return -1; 
	}

	return n; 
}

/*****************************************
* ReadFile reads in numbers.txt located under
* Resource Files. 
* The file contains 10K random numbers separated
* by a newline. 
******************************************/
bool ReadFile(vector<int> & allNumbers) {
	//read in a file of primes 
	ifstream dataIn("numbers.txt");

	//safety check
	if (!dataIn) {
		cout << "Cannot open file" << endl;
		return false;
	}

	if (dataIn.is_open()) {
		string num_str;
		while (dataIn.good()) {
			//reads in one chunk at a time
			//will delineate by spaces and commas too
			dataIn >> num_str; 

			if (num_str.size() > 0) {
				//convert string to number
				allNumbers.push_back(stoi(num_str));
			}
		}
	}
	dataIn.close();
	return true; 
}

/*****************************************
* WriteFile writes to primenumbers.txt
* the prime numbers stored in primeNumbers
* it will overwrite data in the file. 
******************************************/
bool WriteFile(vector<int> primeNumbers) {
	ofstream dataOut("primenumbers.txt", ios::trunc);

	if (!dataOut) {
		cout << "Cannot open file" << endl; 
		return false; 
	}

	if (dataOut.is_open()) {
		dataOut << "Chelsea Found These Primes: " << endl; 
		for (auto i : primeNumbers) {
			dataOut << i << endl; 
		}
	}
	dataOut.close(); 
}

int main()
{
	vector<int> allNumbers;
	vector<int> primeNumbers;

	std::vector<std::future<int>> futures;

	if (ReadFile(allNumbers)) {

		auto NUM_COUNT = allNumbers.size();
		vector<int> one_half;

		//take half of the numbers read and place in another vector
		//allNumbers will have the other half of numbers. 
		for (auto j(0); j < NUM_COUNT / 2; j++) {
			one_half.emplace_back(allNumbers[j]);
			allNumbers.erase(allNumbers.begin() + j);
		}

		//odd number of numbers, one vector will be +1 larger, so loop +1
		if (NUM_COUNT % 2 == 1) {
			NUM_COUNT = allNumbers.size() + 1;
		}

		auto i = 0;
		auto one_flag = false;
		auto two_flag = false;

		while (i < NUM_COUNT) {
			if (i < one_half.size())
				//begin one async task to check one number
				futures.push_back(std::async(checkPrime, one_half[i]));
			else
				one_flag = true;

			if (i < allNumbers.size())
				//begin second async task to check another number
				futures.push_back(std::async(checkPrime, allNumbers[i]));
			else
				two_flag = true;

			//before doing next round, grab results 
			//if not done, this will block main thread
			//until futures has a result.
			if (!one_flag || !two_flag) {
				for (auto &e : futures) {
					auto num = e.get();
					if (num != -1)
						primeNumbers.emplace_back(num);
				}
				futures.clear(); //finished w futures so clear them. 
			}
			//break out of loop when both flags are met
			if (one_flag && two_flag) break;
			i++;
		}

		if (WriteFile(primeNumbers)) {
			cout << "Please view primenumbers.txt located in the Resource Files" << endl; 
			return 0;
		}
		else return -1;
	}
	else
		cout << "ERROR, couldn't open numbers.txt" << endl; 

	return -1; 
}
