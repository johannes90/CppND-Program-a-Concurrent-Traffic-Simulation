#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

enum TrafficLightPhase {red, green}; // FP1 TrafficLightPhase is an enum that 
// can be either „red“ or „green“

// forward declarations to avoid include cycle
class Vehicle;


// FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 

template <class T> //T = TrafficLight because other types should work as well
class MessageQueue
{
public:
    void send(T&& message); //FP.3 (but had to implement that allready in FP.2 -> what is going on with the order of tasks?)
    T receive(); //FP.3

private:
std::deque<T> _queue; //FP.3
std::condition_variable _condition; //FP.3
std::mutex _mutex;   //allready needed for FP.2
    
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject. 
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

class TrafficLight : public TrafficObject // FP.1 (..child class of TrafficObject..)
{
public:
    // constructor / desctructor
    TrafficLight(); //FP.1
    ~TrafficLight(); //FP.1

    // getters / setters
    TrafficLightPhase getCurrentPhase(); //FP.1

    // typical behaviour methods
    void waitForGreen(); //FP.1
    void simulate(); //FP.1


private:
    // typical behaviour methods
    void cycleThroughPhases();    
    TrafficLightPhase _currentPhase; //FP.1

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.
    MessageQueue<TrafficLightPhase> _messages; //FP.4b

   
};

#endif