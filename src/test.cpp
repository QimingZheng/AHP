#include <iostream>
#include <fstream>
#include <string.h> 

using namespace std;

int main(int argc, char *argv[]) {
	std::ofstream fout;
	fout.open("test",ios::out);
	fout<<"Hello";
    	string a = "exy" + std::to_string(123);
	cout<<a;
	fout.close();
	return 0;
}
