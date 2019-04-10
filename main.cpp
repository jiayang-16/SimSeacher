#include "SimSearcher.h"

using namespace std;

int main(int argc, char **argv)
{
	SimSearcher searcher;

	vector<pair<unsigned, unsigned> > resultED;
	vector<pair<unsigned, double> > resultJaccard;
	int q = 2;

	searcher.createIndex(argv[1], q);
	//searcher.searchJaccard("query", 0.2, resultJaccard);
	searcher.searchED("abcdefghijkabcdefghijk", 6, resultED);
/*	const char* str1 = " aaaabbbbbaaaaaaa aaaaaaaaaaadddd dddda  ddddddddddsssssssss sssssssssd ";
	const char* str2 = " aaaaaaaaaaaaaaaaaabbbbbbb bbbbbbbbbbbbbaaaaaaaaaa aaasddddddasddddddasssssssssss";
	int a,b;
	if(strlen(str1) < strlen(str2)){
		a = GetED(str1,str2,strlen(str1)+strlen(str2),strlen(str1),strlen(str2));
		b = GetEDVolient(str1,str2,strlen(str1)+strlen(str2),strlen(str1),strlen(str2));
	}
	cout<<"result:"<<a<<" "<<b<<endl;*/
	return 0;
}
