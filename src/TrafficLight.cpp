#include <iostream>
#include <random>
#include "TrafficLight.h"

#include <stdio.h>      /* NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> lock(_mutex);
    // Do not wait without a condition: it is good for detecting the sporadic wake-up,
    // https://www.modernescpp.com/index.php/c-core-guidelines-be-aware-of-the-traps-of-condition-variables
    _condition.wait(lock, [this] { return !_queue.empty(); });
    T message = std::move(_queue.front());
    _queue.pop_front();
    return message;
}


template <typename T>
// logically unnecessary copying and to provide perfect forwarding functions.
// It is just a matter of performance as with move you avoid copying
void MessageQueue<T>::send(T &&message)// (had to be implemented in FP.2 allready)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lock(_mutex); 
    _queue.clear(); // make the performance better
    _queue.emplace_back(std::move(message));
    _condition.notify_one();
}



/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        if(_messages.receive() == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method ???cycleThroughPhases??? should be started in a thread when the public method ???simulate??? is called. 
    // To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));

}


// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    
    // similar implementation in Vehicle::drive() (-> I reuse what makes sense)
    int randCycleDuration = rand()%(6000-4000 + 1) + 4000;
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // Init stop watch
    lastUpdate = std::chrono::system_clock::now();
    // infinite loop
    while(true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= randCycleDuration)
        {
            // reset the clycle duration here to have a new time at each iteration
            randCycleDuration = rand()%(6000-4000 + 1) + 4000;

            // toggle between red and green phase
            if (_currentPhase == red)
            {
                _currentPhase = green;
            }
            else
            {
                _currentPhase = red;
            }
            // send update to the message queue using move semantics
            _messages.send(std::move(_currentPhase));  // that msg is first build in Task FP.4

            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
        }

    }

}

