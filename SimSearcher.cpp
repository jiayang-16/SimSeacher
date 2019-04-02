#include "SimSearcher.h"
#include <cstdlib>

using namespace std;

SimSearcher::SimSearcher()
{
}

SimSearcher::~SimSearcher()
{
}

int SimSearcher::createIndex(const char *filename, unsigned q)
{
	char buf[1024];
	FILE* f = fopen(filename,"r");
	int line = 1;
	while(fgets(buf,1024,f) != NULL){
		//printf("%s", buf);
		int len = strlen(buf)-1;
		string edKey;
		string jaccardKey;
		string str(buf);
		str = str.substr(0,len);
		for(int i = 0;i < len-q+1;i++){
			edKey = str.substr(i,3);
			edIndex[edKey].push_back(line);
		}
		str = str+" ";
		for(int i = 0;i < len+1;i++){
			int pos = str.find(" ",i);
			if(pos < len+1){
				jaccardKey = str.substr(i,pos-i);
				jaccardIndex[jaccardKey].push_back(line);
				i = pos;
			}
		}
		line ++;
	}
	map<string,vector<int>>::iterator ite;
	for(ite = edIndex.begin();ite!=edIndex.end();ite++){
		edSortedList.push_back(Index(ite->first,(ite->second).size()));
	}
	for(ite = jaccardIndex.begin();ite!=jaccardIndex.end();ite++){
		jaccardSortedList.push_back(Index(ite->first,(ite->second).size()));
	}
	sort(edSortedList.begin(),edSortedList.end(),SortFunc);
	sort(jaccardSortedList.begin(),jaccardSortedList.end(),SortFunc);
/*	for(int i = 0;i < edSortedList.size();i++){
		cout<<edIndex[edSortedList[i].name].size()<<endl;
	}*/
	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result)
{
	result.clear();
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result)
{
	result.clear();
	return SUCCESS;
}

void SimSearcher::printIndex(map<string,vector<int>> mMap){
	map<string,vector<int>>::iterator i;
	for(i = mMap.begin();i!=mMap.end();i++){
		cout<<"key:"<<i->first<<" "<<"list:";
		for(int j = 0;j < (i->second).size();j++){
			printf("%d ", (i->second)[j]);
		}
		printf("\n");
	}
}

bool SortFunc(Index a,Index b){
	return (a.length > b.length);
}