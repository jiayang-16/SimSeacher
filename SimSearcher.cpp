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
	qgram_length = q;
	char buf[1024];
	FILE* f = fopen(filename,"r");
	lineCount = 0;
	minLen = 1024;
	while(fgets(buf,1024,f) != NULL){
		//printf("%s", buf);
		int len = strlen(buf)-1;
		strLenList.push_back(len);
		string edKey;
		string jaccardKey;
		string str(buf);
		str = str.substr(0,len);
		for(int i = 0;i < len-q+1;i++){
			edKey = str.substr(i,3);
			if(edIndex[edKey].empty())
				edIndex[edKey].push_back(lineCount);
			else if(edIndex[edKey].back() != lineCount)
				edIndex[edKey].push_back(lineCount);
		}
		dataStr.push_back(str);
		str = str+" ";
		int lineLen = 0;
		for(int i = 0;i < len+1;i++){
			int pos = str.find(" ",i);
			if(pos < len+1){
				jaccardKey = str.substr(i,pos-i);
				if(jaccardIndex[jaccardKey].empty()){
					jaccardIndex[jaccardKey].push_back(lineCount);
					lineLen += 1;
				}
				else if(jaccardIndex[jaccardKey].back() != lineCount){
					jaccardIndex[jaccardKey].push_back(lineCount);
					lineLen += 1;					
				}
				i = pos;
			}
		}
		if(minLen > lineLen){
			minLen = lineLen;
		}
		dataStrCount.push_back(lineLen);
		lineCount ++;		
	}
	//printIndex(jaccardIndex);
/*	map<string,vector<int>>::iterator ite;
	for(ite = edIndex.begin();ite!=edIndex.end();ite++){
		edSortedList.push_back(Index(ite->first,(ite->second).size()));
	}
	for(ite = jaccardIndex.begin();ite!=jaccardIndex.end();ite++){
		jaccardSortedList.push_back(Index(ite->first,(ite->second).size()));
	}
	sort(edSortedList.begin(),edSortedList.end(),SortFunc);
	sort(jaccardSortedList.begin(),jaccardSortedList.end(),SortFunc);*/
	//printIndex(edIndex);
/*	for(int i = 0;i < edSortedList.size();i++){
		cout<<edIndex[edSortedList[i].name].size()<<endl;
	}*/
	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result)
{
	result.clear();
	set<string> queryList;
	string queryStr(query);
	queryStr = queryStr+" ";
	int len = strlen(query)+1;
	std::vector<Index> sortedList;
	for(int i = 0;i < len;i++){
		int pos = queryStr.find(" ",i);
		if(pos < len && pos != i){
			queryList.insert(queryStr.substr(i,pos-i));
			i = pos;
		}
	}
	for(set<string>::iterator i = queryList.begin();i != queryList.end();i++){
		map<string,vector<int>>::iterator ite = jaccardIndex.find(*i);
		if(ite != jaccardIndex.end())
			sortedList.push_back(Index(*i,(ite->second).size()));
	}
	sort(sortedList.begin(),sortedList.end(),SortFunc);
	//printJaccardSortedList(sortedList);
	int qlen = queryList.size();
	double x1 = threshold*qlen;
	double x2 = (qlen+minLen)*threshold/(1+threshold);
	int T = x1>x2? floor(x1):floor(x2);
	//cout<<"T:"<<T<<endl;
	int longLen = T/(ulogM+1);
	int shortLen = sortedList.size()-longLen;

	int shortT = T-longLen;//candidate must appear more than that
	std::vector<int> pList;//current index when mergeskiping
	for(int i = 0;i < sortedList.size();i++){
		pList.push_back(1);
	}
	std::vector<HeapEle> heap;
	for(int i = 0;i < shortLen;i++){
		int index = i+longLen;
		//cout<<index<<endl;
		heap.push_back(HeapEle(index,(jaccardIndex[sortedList[index].name])[0]));
	}
	make_heap(heap.begin(),heap.end(),SortFuncForHeap);
	std::vector<pair<int,int>> candidate;
	while(!heap.empty()){
		int popCount = 0;
		int popEle = (heap[0]).ele;
		//printf("heap top %d\n", popEle);
		std::vector<int> popedList;
/*		for(int i = 0;i < sortedList.size();i++){
			cout<<pList[i]<<" ";
		}
		cout<<endl;
		cout<<"heap:";
		for(int i = 0;i < heap.size();i++){
			cout<<heap[i].ele<<" ";
		}
		cout<<endl;*/
		while(heap[0].ele == popEle){
			pop_heap(heap.begin(),heap.end(),SortFuncForHeap);
			popedList.push_back((heap.back()).index);
			heap.pop_back();
			popCount++;
			if(heap.empty()) break;
			//printf("pop\n");
		}
		if(popCount >= shortT){
			//printf("get result %d popCount %d\n", popEle,popCount);
			candidate.push_back(pair<int,int>(popEle,popCount));
			vector<int>::iterator i;
			for(i = popedList.begin();i != popedList.end();i++){
				if(pList[*i]<sortedList[(*i)].length){//list have sth to pop
					heap.push_back(HeapEle((*i),(jaccardIndex[sortedList[*i].name])[pList[*i]]));
					push_heap(heap.begin(),heap.end(),SortFuncForHeap);
					pList[*i]++;
				}
			}
		}
		else{
			int p = shortT-1-popCount;
			//printf("not enough p:%d\n",p);
			for(int i = 0;i < p;i++){
				pop_heap(heap.begin(),heap.end(),SortFuncForHeap);
				popedList.push_back((heap.back()).index);
				heap.pop_back();
			}
			HeapEle top = heap[0];
			vector<int>::iterator i;
			//cout<<"find:compare to "<<top.ele<<" ";
			for(i = popedList.begin();i != popedList.end();i++){
				int loc = FindFirstGreater(jaccardIndex[sortedList[*i].name],top.ele);
				//cout<<loc<<" ";
				if(loc > 0)
					pList[*i] = loc;
				heap.push_back(HeapEle((*i),(jaccardIndex[sortedList[*i].name])[pList[*i]]));
				push_heap(heap.begin(),heap.end(),SortFuncForHeap);
				pList[*i]++;
			}
			//cout<<endl;			
		}
	}
	if(!candidate.empty()){
/*		for(int i = 0;i < candidate.size();i++){
			cout<<candidate[i].first<<" "<<candidate[i].second<<endl;
		}*/
	}
	vector<pair<int,int>>::iterator candIter;
	for(candIter = candidate.begin();candIter != candidate.end();candIter++){
		for(int j = 0;j < longLen;j++){
			if(binarySearch(jaccardIndex[sortedList[j].name],(*candIter).first)){
				(*candIter).second += 1;
			}
		}
	}
	for(candIter = candidate.begin();candIter != candidate.end();candIter++){
		if((*candIter).second >= T){
			double jaccardResult = (double)((*candIter).second)/(queryList.size()+dataStrCount[(*candIter).first]-(*candIter).second);
			if(jaccardResult >= threshold){//scan result
				//cout<<(*candIter).first<<","<<jaccardResult<<endl;
				result.push_back(pair<unsigned,unsigned>((*candIter).first,jaccardResult));
			}
		}
	}
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result)
{
	result.clear();
	std::vector<string> queryList;
	queryList.reserve(1024);
	int len = strlen(query);
	string queryStr(query);
	std::vector<Index> sortedList;
	int qgramCount = len-qgram_length+1;
	sortedList.reserve(qgramCount);
	for(int i = 0;i < qgramCount;i++){
		string str = queryStr.substr(i,3);
		queryList.push_back(str);		
		map<string,vector<int>>::iterator ite = edIndex.find(str);
		if(ite != edIndex.end()){
			sortedList.push_back(Index(str,(ite->second).size()));			
		}
	}
	int bias = -qgram_length+1-threshold*qgram_length;
	int T = strlen(query)+bias;
	int edResult;
	int longLen = T/(ulogM+1);
	int shortLen = sortedList.size()-longLen;
	int shortT = T-longLen;//candidate must appear more than that
	if(T <= 0 || shortLen <= 0 || len <= qgram_length){//scan all
		for(int i = 0;i < lineCount;i++){
			int dataLen = dataStr[i].size();
			if(dataLen <= len)
				edResult = GetED(dataStr[i].c_str(),query,threshold,dataLen,len);
			else{
				edResult = GetED(query,dataStr[i].c_str(),threshold,len,dataLen);
			}
			if(edResult <= threshold){//scan result
				//cout<<i<<","<<edResult<<endl;
				result.push_back(pair<unsigned,unsigned>(i,edResult));
			}
		}
		return SUCCESS;
	}
	//scan/merge 
	std::vector<pair<int,int>> candidate;
	candidate.reserve(128);
	//std::vector<pair<int,int>> finalCandidate;
	sort(sortedList.begin(),sortedList.end(),SortFunc);
	//printEdSortedList(sortedList);
	std::vector<int> pList;//current index when mergeskiping
	pList.reserve(sortedList.size());
	for(int i = 0;i < sortedList.size();i++){
		pList.push_back(1);
	}
	std::vector<HeapEle> heap;
	heap.reserve(1024);
	for(int i = 0;i < shortLen;i++){
		int index = i+longLen;
		//cout<<index<<endl;
		heap.push_back(HeapEle(index,(edIndex[sortedList[index].name])[0]));
	}
	std::vector<int> popedList;
	popedList.reserve(sortedList.size());
	make_heap(heap.begin(),heap.end(),SortFuncForHeap);
	while(!heap.empty()){
		int popCount = 0;
		int popEle = (heap[0]).ele;
		//printf("heap top %d\n", popEle);
		popedList.clear();
/*		for(int i = 0;i < sortedList.size();i++){
			cout<<pList[i]<<" ";
		}
		cout<<endl;
		cout<<"heap:";
		for(int i = 0;i < heap.size();i++){
			cout<<heap[i].ele<<" ";
		}
		cout<<endl;*/
		while(heap[0].ele == popEle){
			pop_heap(heap.begin(),heap.end(),SortFuncForHeap);
			popedList.push_back((heap.back()).index);
			heap.pop_back();
			popCount++;
			if(heap.empty()) break;
			//printf("pop\n");
		}
		if(popCount >= shortT){
			//printf("get result %d popCount %d\n", popEle,popCount);
			candidate.push_back(pair<int,int>(popEle,popCount));
			vector<int>::iterator i;
			for(i = popedList.begin();i != popedList.end();i++){
				if(pList[*i]<sortedList[(*i)].length){//list have sth to pop
					heap.push_back(HeapEle((*i),(edIndex[sortedList[*i].name])[pList[*i]]));
					push_heap(heap.begin(),heap.end(),SortFuncForHeap);
					pList[*i]++;
				}
			}
		}
		else{
			int p = shortT-1-popCount;
			//printf("not enough p:%d\n",p);
			for(int i = 0;i < p;i++){
				pop_heap(heap.begin(),heap.end(),SortFuncForHeap);
				popedList.push_back((heap.back()).index);
				heap.pop_back();
			}
			if (heap.empty())
			{
				break;
			}
			HeapEle top = heap[0];
			vector<int>::iterator i;
			//cout<<"find:compare to "<<top.ele<<" ";
			for(i = popedList.begin();i != popedList.end();i++){
				int loc = FindFirstGreater(edIndex[sortedList[*i].name],top.ele);
				//cout<<loc<<" ";
				if(loc > 0)
					pList[*i] = loc;
				heap.push_back(HeapEle((*i),(edIndex[sortedList[*i].name])[pList[*i]]));
				push_heap(heap.begin(),heap.end(),SortFuncForHeap);
				pList[*i]++;
			}
			//cout<<endl;			
		}
	}

	vector<pair<int,int>>::iterator candIter;
	vector<int> finalCandidate;
	finalCandidate.reserve(64);
	for(candIter = candidate.begin();candIter != candidate.end();candIter++){
		for(int j = 0;j < longLen;j++){
			if(binarySearch(edIndex[sortedList[j].name],(*candIter).first)){
				(*candIter).second += 1;
				if((*candIter).second >= T){
					finalCandidate.push_back((*candIter).first);
					break;
				}
			}
		}
	}
	vector<int>::iterator finalIte;
	for(finalIte = finalCandidate.begin();finalIte != finalCandidate.end();finalIte++){
		int dataLen = dataStr[(*finalIte)].size();
		if(dataLen <= len)
			edResult = GetED(dataStr[(*finalIte)].c_str(),query,threshold,dataLen,len);
		else{
			edResult = GetED(query,dataStr[(*finalIte)].c_str(),threshold,len,dataLen);
		}
		if(edResult <= threshold){//scan result
			//cout<<(*finalIte).first<<","<<edResult<<endl;
			result.push_back(pair<unsigned,unsigned>((*finalIte),edResult));
		}
	}
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

void SimSearcher::printEdSortedList(std::vector<Index> sortedList){
	vector<Index>::iterator i;
	cout<<"sortedList:"<<endl;
	for(i = sortedList.begin();i != sortedList.end();i++){
		cout<<"key:"<<(*i).name<<" ";
		vector<int>::iterator j;
		vector<int> list = edIndex[(*i).name];
		for(j = list.begin();j != list.end();j++){
			cout<<(*j)<<" ";
		}
		cout<<endl;
	}
}

void SimSearcher::printJaccardSortedList(std::vector<Index> sortedList){
	vector<Index>::iterator i;
	cout<<"sortedList:"<<endl;
	for(i = sortedList.begin();i != sortedList.end();i++){
		cout<<"key:"<<(*i).name<<" ";
		vector<int>::iterator j;
		vector<int> list = jaccardIndex[(*i).name];
		for(j = list.begin();j != list.end();j++){
			cout<<(*j)<<" ";
		}
		cout<<endl;
	}
}

bool SortFunc(Index a,Index b){
	return (a.length > b.length);
}

bool SortFuncForHeap(HeapEle a,HeapEle b){
	return (a.ele > b.ele);
}

int mAbs(int a){
	return a>0?a:-a;
}
int GetED(const char *s1,const char *s2,int threshold,int len1,int len2){
	if(len2-len1 > threshold) return threshold+1;//impossible
	int d[len1+1][len2+1];
    int i,j;
    for(i = 0;i <= threshold && i <= len1;i++)     
        d[i][0] = i;     
    for(j = 0;j <= threshold && j <= len2;j++)     
        d[0][j] = j;
    //printf("test\n");
	for(i = 1;i <= len1;i++){
		int begin = i-threshold>1 ? i-threshold : 1;
		int end = i+threshold<len2 ? i+threshold : len2;
        for(j = begin;j <= end;j++)  
        {  
            int cost = s1[i-1] == s2[j-1] ? 0 : 1;     
            int deletion = mAbs(i-1-j)>threshold?INT_MAX:d[i-1][j] + 1;     
            int insertion = mAbs(i-j+1)>threshold?INT_MAX:d[i][j-1] + 1;
            int substitution = d[i-1][j-1] + cost;     
            d[i][j] = min(deletion,insertion,substitution);     
        }
	}
    return d[len1][len2];
}
int GetEDVolient(const char *s1,const char *s2,int threshold,int len1,int len2){
	if(len2-len1 > threshold) return threshold+1;//impossible
	int** d=new int*[len1+1];
	int i,j;
   	for(int k=0;k<=len1;k++)  
        d[k]=new int[len2+1];
    for(i = 0;i <= len1;i++)     
        d[i][0] = i;     
    for(j = 0;j <= len2;j++)     
        d[0][j] = j;
    //printf("test\n");
	for(i = 1;i <= len1;i++){
        for(j = 1;j <= len2;j++)  
        {   
            int cost = s1[i-1] == s2[j-1] ? 0 : 1;     
            int deletion = d[i-1][j] + 1;     
            int insertion = d[i][j-1] + 1;     
            int substitution = d[i-1][j-1] + cost;     
            d[i][j] = min(deletion,insertion,substitution);     
        }
	}
	for(i = 0;i <= len1;i++){
		for(j = 0;j <= len2;j++){
			cout<<d[i][j]<<" ";
		}
		cout<<endl;		
	}
    int result = d[len1][len2];
    for(int k=0;i<=len1;k++)  
        delete[] d[k];
    delete[] d;
    return result;
}
int GetJaccard(vector<string> a,vector<string> b){

}

int FindFirstGreater(std::vector<int> v,int comp){
	if(comp > v.back())	return -1;
	int low = 0;
	int high = v.size()-1;
	int mid;
    while(low <= high)
    {
        mid = (low+high)/2;
        if(comp > v[mid])
            low = mid+1;
        else
            high = mid-1;
    }
    return low;
}

bool binarySearch(std::vector<int> v,int comp){
	int low = 0;
	int high = v.size()-1;
	int mid;
    while(low <= high)
    {
        mid = (low+high)/2;
        if(comp == v[mid])
        	return true;
        else if(comp > v[mid])
            low = mid+1;
        else
            high = mid-1;
    }
    return false;
}

int min(int a,int b,int c) {     
    int t = a < b ? a : b;     
    return t < c ? t : c;     
}