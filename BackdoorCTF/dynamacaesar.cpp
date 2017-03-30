#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

#define offsetLength 26

char offsetList[offsetLength] = {'e'-'a', 't'-'a', 0, 'o'-'a', 'i'-'a', 'n'-'a', 's'-'a', 'h'-'a', 'r'-'a', 'd'-'a', 'l'-'a', 'c'-'a', 'u'-'a', 'm'-'a', 'w'-'a', 'f'-'a', 'g'-'a', 'y'-'a', 'p'-'a', 'b'-'a', 'v'-'a', 'k'-'a', 'j'-'a', 'x'-'a', 'q'-'a', 'z'-'a'};

void decrypt(char* toDecrypt, int offset, bool file);

int main(int argc, char** argv){
	char temp = 'n';
	if(argc==2){
		for(int i=0;i<offsetLength;++i){
			decrypt(argv[1], offsetList[i], true);
			cout << "Is the text decrypted? (y/n): ";
			cin.get(temp);
			cin.clear();
			cin.ignore(1000, '\n' );
			if(temp=='y'){
				break;
			}
		}
	}
	else if(argc==3 && !strcmp(argv[1], "text")){
		for(int i=0;i<offsetLength;++i){
			decrypt(argv[2], offsetList[i], true);
			cout << "Is the text decrypted? (y/n): ";
			cin.get(temp);
			cin.clear();
			cin.ignore(1000, '\n' );
			if(temp=='y'){
				break;
			}
		}
	}
	else if(argc>3){
		cout << "Please only enter one file name" << endl;
	}
	else{
		cout << "Please enter a file name" << endl;
	}
	return 0;
}

void decrypt(char* toDecrypt, int offset, bool file){
	cout << "Decrypting..." << endl << endl;
	if(file){
		ifstream in;
		in.open(toDecrypt);
		if(!in){
			cout << "Couldn't open the file" << endl;
		}
		else{
			char temp;
			while(in.get(temp)){
				if(isalpha(temp)){
					temp = tolower(temp);
	        		temp = (((temp - 97) + offset) % 26) + 97;
				}
				putchar(temp);
			}
		}
		in.close();
	}
	else{
		char temp;
		for(int i=0;i<strlen(toDecrypt);++i){
			temp = toDecrypt[i];
			if(isalpha(temp)){
				temp = tolower(temp);
        		temp = (((temp - 97) + offset) % 26) + 97;
			}
			putchar(temp);
		}
	}
	cout << endl << endl << "Finished decrypting" << endl;
}
