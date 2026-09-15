//#include<iostream>
//using namespace std;
//int main(int argc,char* argv[]){
//if(argc > 1){
//	cout << "Hello," << argv[1] <<"!\n";
//}
//}
//task2-
//#include<iostream>
//using namespace std;
//#include<fstream>
//int main(int argc,char* argv[]){
// 
// std::ofstream file("diary.txt");
//file << "Hello OS\n";
//file.close();}
//task3-
//#include<iostream>
//using namespace std;
//#include<windows.h>
//int main(int argc,char* argv[]){
//  std::cout << "My Process ID is: "<< GetCurrentProcessId() << "\n";}
//task4-
//#include<iostream>
//#include<thread>
//#include<chrono>
//using namespace std;
//int main(){
//  cout << "Going to sleep...\n";
//this_thread::sleep_for(chrono::seconds(3));
//cout << "Waking up!\n";
//return 0;}
//task5-
//#include<iostream>
//#include <cstdlib>
//int main(){
//std::system("dir");}
//task6-
//#include<iostream>
//#include<cstdlib>
//int main(){
//  const char* user = std::getenv("USERNAME");
//if (user != nullptr)
//std::cout << "Current user is: "
//<< user << "\n";
//}
//task7-
//#include<iostream>
//#include<cstdio>
//int main(){
//FILE* file = fopen("ghost.txt", "r");
//if (file == nullptr)
//perror("Failed to open file");}

//Task8-
//#include<iostream>
//#include<windows.h>
//using namespace std;
//
//
//BOOL WINAPI handler(DWORD signal){
//	if(signal==CTRL_C_EVENT){
//		std::cout<<"You can't close me!\n";
//		return true;
//	}
//	return false;
//}
//int main(){
//
//SetConsoleCtrlHandler(handler,TRUE);
//while(true){
//	std::cout<<"Running....\n";
//	sleep(1000);
//}}

//Task9-
#include<iostream>
#include<windows.h>
using namespace std;
int main(){

STARTUPINFOA si{};
PROCESS_INFORMATION pi{};
si.cb = sizeof(si);
char command[] = "cmd.exe/C echo I am the child";
if(CreateProcessA(
nullptr,
command,
nullptr,
nullptr,
FALSE,
0,
nullptr,
nullptr,
&si,
&pi)){
	std::cout<<"I am The parent\n";
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}}



