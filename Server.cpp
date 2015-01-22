/*

 * Gerald Blake

 * CS 4323 Program 2

 * CWID:11034027
 * CSX ID:geralab

 *

 */

#include <iostream>
#include<thread>
#include<pthread.h>
#include<semaphore.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
using namespace std;
ostringstream convert;
int T_Valve_Numbers = 6;
int T_Number_Wrenches;
void clearStream();
//using namespace std;
#define NTHREADS 10

void hello();
int main( int argc, const char* argv[] )
{
    vector<thread> threads;
    
    for(int i = 0; i < 40; ++i)
    {
        threads.push_back(thread(hello));
    }
    
    for(auto& thread : threads){
        thread.join();
    }
    
    return 0;
}

void hello(){
    cout << "Hello from thread " << this_thread::get_id() << endl;
}

void clearStream()
{
    convert.str("");
    convert.clear();
}

