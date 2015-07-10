#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

int main(){
	vector<int> v1(10);
	vector<int> v2(10);
	int count = 0;
	
	for(int i = 0; i < 5; ++i){
		for(int j = i+1; j < 5; ++j){
			int temp = count;
			v1[count]= temp;
			//count++;
			v2[count++]=temp;
		}
	}
	
	for(int i = 0; i < 10; ++i){
		cout << "(" << v1[i] << ", " << v2[i] << ")" << endl;
	}
	return 0;
}