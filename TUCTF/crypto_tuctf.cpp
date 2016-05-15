//Brit Cornwell
//TU CTF
//May 13-15 2016

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <fstream>

using namespace std;

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "24069"
#define DEFAULT_IP "146.148.102.236"

string currentSpaceChar[3] = {" "};

char ALPHA[72] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890abcdefghijklmnopqrstuvwxyz";
char SENDALPHA[21] = "      ";
  
int level1Elements = 27;              
string level1[27] = {".-","-...","-.-.","-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", " "}; 
int level2Elements = 63;
string level2[63] = {"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_", "`", "a", "b", "c", "d", "e", "f", "g", "-", ">", "?", "@", "A", "B", "C", "D", "E", "F", "=", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", " ", "!", "\"", "#", "$", "%", "&", "'", "("};
int level3Elements = 63;
string level3[63] = {"A", "X", "J", "E", ">", "U", "I", "D", "C", "H", "T", "N", "M", "B", "R", "L", "\"", "P", "O", "Y", "G", "K", "<", "Q", "F", ":", " ", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "a", "x", "j", "s", "f", "t", "d", "h", "u", "n", "e", "i", "m", "k", "y", ";", "q", "p", "r", "g", "l", "v", "w", "x", "j", "z"};
int level4Elements = 63;
string level4[63] = {"D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "V", "W", "X", "Y", "Z", "[", "!", "2", "3", "4", "5", "6", "7", "8", "9", ":", "1", ",", "-", ".", "/", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N"};


string wordBank[] = {"MOONCHILD", "MYLUCKDRAGON", "DONTFORGETAURYN", "ATREYUVSGMORK", "GIANTTURTLE", "SAVETHEPRINCESS", "WELCOMEATREYU", "READINGISDANGEROURS", "THENOTHING", "GMORKSCOOL", "IOWEYOU", "TRYSWIMMING", "THEORACLE", "BASTIAN", "FRAGMENTATION"};
string fixedBank[] = {"MOON CHILD", "MY LUCKDRAGON", "DONT FORGET AURYN", "ATREYU VS GMORK", "GIANT TURTLE", "SAVE THE PRINCESS", "WELCOME ATREYU", "READING IS DANGEROURS", "THE NOTHING", "GMORKS COOL", "I OWE YOU", "TRY SWIMMING", "THE ORACLE", "BASTIAN", "FRAGMENTATION"};

string currentLevel = "0";

SOCKET ConnectSocket = INVALID_SOCKET;
int netResult = 0;

void sendData(string toSend);
string extractString(string toExtract);
string formatString(string toFormat);
void writeLog(string toWrite, bool append);
void updateLevel(string str);
void respond(string str);
string toLower(string in);
void updateSpace(string str);

void writeLog(string toWrite, bool append){
    ofstream outFile;
    if(append){
        outFile.open("log.txt", ios::app);
    }
    else{
        outFile.open("log.txt");
    }
    outFile << toWrite << endl;
    outFile.close();
}

int main(int argc, char **argv) 
{
    writeLog("Beginning crypto", false);
    WSADATA wsaData;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    char recvbuf[DEFAULT_BUFLEN];
    netResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(netResult!=0){
        writeLog("WSAStartup failed", true);
        return 1;
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    netResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
    if(netResult!=0){
        writeLog("Getaddrinfo failed", true);
        WSACleanup();
        return 1;
    }
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next){
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if(ConnectSocket==INVALID_SOCKET){
            writeLog("Socket failed", true);
            WSACleanup();
            return 1;
        }
        netResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if(netResult==SOCKET_ERROR){
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    if(ConnectSocket==INVALID_SOCKET){
        writeLog("Unable to connect to server", true);
        WSACleanup();
        return 1;
    }
    do{
        ZeroMemory(&recvbuf, sizeof(recvbuf));
        netResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
        string str = recvbuf;
        writeLog("Received: " + str, true);
        if(netResult>0){
            if(str.find("This is level")!=string::npos){
                updateLevel(str);
            }
            if(str.find("Give me some text:")!=string::npos){
                sendData(SENDALPHA);
            }
            else if(str.find("No... I am leaving")!=string::npos){
                writeLog("Failure", true);
                break;
            }
            else{
                updateSpace(str);
                respond(str);
            }
        }
        else if(netResult==0)
            writeLog("Connection Closed", true);
        else
            writeLog("Receive failed", true);

    } while(netResult>0);
    netResult = shutdown(ConnectSocket, SD_SEND);
    if(netResult==SOCKET_ERROR){
        writeLog("Shutdown failed", true);
    }
    closesocket(ConnectSocket);
    WSACleanup();
    writeLog("Done", true);
    return 0;
}

void respond(string str){
    writeLog("Level " + currentLevel + " response", true);
    str = extractString(str);
    writeLog("Extracted: " + str, true);
    if(currentLevel=="3"){
        writeLog("Checking if " + str + " is decrypted already", true);
        int length = sizeof(fixedBank)/sizeof(fixedBank[0]);
        for(int i=0;i<length;++i){
            if(str==toLower(fixedBank[i])){
                writeLog(str + " is decrypted already", true);
                sendData(str);
                return;
            }
        }
    }
    str = formatString(str);
    writeLog("Formatted: " + str, true);
    sendData(str);
}

void sendData(string toSend){
    toSend += "\n";
    writeLog("Sending: " + toSend, true);
    netResult = send(ConnectSocket, toSend.c_str(), toSend.length(), 0);
    if (netResult==SOCKET_ERROR){
        writeLog("Send failed", true);
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }
}

char level1ToText(string c){
    int index = 0;
    int len = sizeof(level1)/sizeof(level1[0]);
    for(index;index<len;++index){
        if(level1[index]==c)
            break;
    }
    if(index>-1 && index<level1Elements)
        return ALPHA[index];
     else
        return '\0';
}

char level2ToText(char c){
    int index = 0;
    int len = sizeof(level2)/sizeof(level2[0]);
    for(index;index<len;++index){
        if(level2[index][0]==c)
            break;
    }
    if(index>-1 && index<level2Elements)
        return ALPHA[index];
     else
        return '\0';
}

char level3ToText(char c){
    int index = 0;
    int len = sizeof(level3)/sizeof(level3[0]);
    for(index;index<len;++index){
        if(level3[index][0]==c)
            break;
    }
    if(index>-1 && index<level3Elements)
        return ALPHA[index];
     else
        return currentSpaceChar[0][0]+1;
}


string extractString(string toExtract){
    if(toExtract.length()<1){
        writeLog("Bad data", true);
        exit(-1);
    }
    int found = toExtract.find("What is ");
    if(found==-1){
        writeLog("'What is' was not found in '" + toExtract + "'", true);
        exit(-1);
    }
    toExtract = toExtract.erase(0, found+8);
    found = toExtract.find(" decrypted?");
    if(found==-1){
        writeLog("' decrypted?' was not found in '" + toExtract + "'", true);
        exit(-1);
    }
    toExtract = toExtract.erase(found, 13);
    writeLog("raw encrypted text: " + toExtract, true);
    string toReturn="", decrypted="";
    if(currentLevel=="1"){
        char tempArray[200];
        strcpy(tempArray, toExtract.c_str());
        char* temp = strtok(tempArray, " ");
        while(temp!=NULL)
        {
            toReturn += level1ToText(temp);
            temp = strtok(NULL, " ");
        }
    }
    else if(currentLevel=="2"){
        for(int i=0;i<toExtract.length();++i)
        {
            toReturn += level2ToText(toExtract[i]);
        }
    }
    else if(currentLevel=="3"){
        for(int i=0;i<toExtract.length();++i)
        {
            toReturn += level3ToText(toExtract[i]);
        }
    }
    else{
        toReturn = toExtract;
    }
    return toReturn;
}

string formatString(string toFormat){
    int i, length;
    if(currentLevel=="1"){
        length = sizeof(wordBank)/sizeof(wordBank[0]);
        for(i=0;i<length;++i){
            if(wordBank[i]==toFormat)
                return fixedBank[i];
        }
    }
    else if(currentLevel=="2"){
    	//level two is handled well enough without formatting
    }
    else{
        int spaceCharsToCheck = 1;
        if(currentLevel=="7"){
            spaceCharsToCheck = 3;
            writeLog("level 7+ format of " + toFormat + " with space character of '" + currentSpaceChar[0] + ", " + currentSpaceChar[1] + ", or " + currentSpaceChar[2]+ "'", true);
        }
        else{
             writeLog("level 3+ format of " + toFormat + " with space character of '" + currentSpaceChar[0] + "'", true);
        }
        length = sizeof(fixedBank)/sizeof(fixedBank[0]);
        for(i=0;i<length;++i){
            writeLog("Comparing " + toFormat + " to " + fixedBank[i], true);
            if(toFormat.length()==fixedBank[i].length()){
                int fixedSpace1, fixedSpace2, encryptedSpace;
                bool foundAll = true;
                fixedSpace1 = fixedBank[i].find(" ");
                if(fixedSpace1!=-1){
                    fixedSpace2 = fixedBank[i].find(" ", fixedSpace1+1);
                }
                else{
                    for(int q=0;q<spaceCharsToCheck;++q){
                        if(toFormat.find(currentSpaceChar[q])!=string::npos){
                            foundAll = false;
                            break;
                        }
                    }
                }
                for(int q=0;q<spaceCharsToCheck;++q){
                    if(toFormat[fixedSpace1]!=currentSpaceChar[q][0]){
                        foundAll = false;
                        break;
                    }
                    if(fixedSpace2!=-1){
                        for(int q=0;q<spaceCharsToCheck;++q){
                            if(toFormat[fixedSpace2]!=currentSpaceChar[q][0]){
                                foundAll = false;
                                break;
                            }
                        }
                    }
                }
                if(foundAll){
                    writeLog("The two words match", true);
                    return fixedBank[i];
                }
            }
        }
    }
    return toFormat;
}

void updateSpace(string str){
    int found = str.find("encrypted is ");
    if(found==-1){
        writeLog("'encrypted is ' was not found in '" + str + "'", true);
        exit(-1);
    }
    str = str.erase(0, found+13);
    found = str.find("What is");
    if(found==-1){
        writeLog("'What is' was not found in '" + str + "'", true);
        exit(-1);
    }
    str = str.erase(found, str.length()-found);
    if(currentLevel=="7"){
        currentSpaceChar[0] = str[0];
        currentSpaceChar[1] = str[1];
        currentSpaceChar[2] = str[2];
        writeLog("Current space character is now: '" + currentSpaceChar[0] + "', '" + currentSpaceChar[1] + "', or '" + currentSpaceChar[2] + "'", true);
    }
    if(str[1]==str[3] && str[3]==str[5]){
        currentSpaceChar[0] = str[1];
        writeLog("Current space character is now: '" + currentSpaceChar[0] + "'", true);
    }
    
}

void updateLevel(string str){
    currentLevel = str[str.find("This is level ") + 14];
    writeLog("Now at level " + currentLevel, true);
}

string toLower(string in){
    for(int i=0;i<in.length();++i){
        in[i] = tolower(in[i]);
    }
    return in;
}

//TODO: in formatString, loop through possible space characters to determine what can be ruled out.
//      this will likely not take care of level 7 entirely, so perhaps adding a word lookup when we do have spaces in words
//      for example, in the string Cvz55x8z $ @(m>xiC) with space possibilities O5$, see that z55x doesnt make sense, so 5 should go,
//     and Cvz55x8z @(m>xiC) --- when split on $, provides a word count of 8:8
//     however, Cvz5 x8z @(m>xiC) --- when split on second 5 and 8, maps to save the princess