#pragma once
#include <vector>
#include <map>
#include <utility>
#include <string.h>
#include <stdio.h>
#include<algorithm>
#include <iostream>
using namespace std;
const int SUCCESS = 0;
const int FAILURE = 1;

struct Index
{
	string name;
	int length;
	Index(string _name,int _length){
		name = _name;
		length = _length;
	}
};

bool SortFunc(Index a,Index b);

class SimSearcher
{
public:
	SimSearcher();
	~SimSearcher();

	int createIndex(const char *filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
	void printIndex(map<string,vector<int>> mMap);	
	map<string,vector<int>> edIndex;
	map<string,vector<int>> jaccardIndex;
	vector<Index> edSortedList;
	vector<Index> jaccardSortedList;
};


