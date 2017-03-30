#include <iostream>
using namespace std;

int main(int argc, char** argv){
	cout << "Enter the characters to diff: ";
	char temp1, temp2;
	cin >> temp1 >> temp2;
	cout << "The diff is " << temp1-temp2 << endl;
}