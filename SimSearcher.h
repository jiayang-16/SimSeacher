#pragma once
#include <vector>
#include <map>
#include <utility>
#include <string.h>
#include <stdio.h>
#include<algorithm>
#include <iostream>
#include <math.h>
#include <set>
using namespace std;
const int SUCCESS = 0;
const int FAILURE = 1;

#define ulogM 1
struct Index
{
	string name;
	int length;
	Index(string _name,int _length){
		name = _name;
		length = _length;
	}
};

struct HeapEle
{
	int index;//sorted short list's index
	int ele;//used for compare
	HeapEle(int _index,int _ele){
		index = _index;
		ele = _ele;
	}
};

bool SortFunc(Index a,Index b);
bool SortFuncForHeap(HeapEle a,HeapEle b);
unsigned GetED(const char *s1,const char *s2,unsigned threshold,int len1,int len2);
//int GetEDVolient(const char *s1,const char *s2,int threshold,int len1,int len2);
int GetJaccard(vector<string> a,vector<string> b);
int min(int a,int b,int c);
int FindFirstGreater(std::vector<int> v,int comp);
bool binarySearch(std::vector<int> v,int comp);
class SimSearcher
{
public:
	SimSearcher();
	~SimSearcher();
	int minLen;//jaccard string with shortest length
	int maxLen;
	int lineCount;
	int qgram_length;
	std::vector<string> dataStr;
	vector<int> dataStrCount;
	int createIndex(const char *filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
	void printIndex(map<string,vector<int>> mMap);	
	void printEdSortedList(std::vector<Index> sortedList);
	void printJaccardSortedList(std::vector<Index> sortedList);
	map<string,vector<int>> edIndex;
	map<string,vector<int>> jaccardIndex;
	std::vector<int> strLenList;
	vector<Index> edSortedList;
	vector<Index> jaccardSortedList;
	std::vector<HeapEle> heap;
	std::vector<string> queryList;
	std::vector<Index> sortedList;
	std::vector<pair<int,int>> candidate;
	std::vector<int> popedList;
	std::vector<int> pList;//current index when mergeskiping
};


