//
// Created by chanbin on 25. 7. 24.
//

#include "../include/Service.h"



void Service::start()
{
    if (startContinueMessage())
    {

    }
}


bool Service::startContinueMessage()
{
    string title = "Hello, Want to use  conditionSortLogger?";
    string description = "If you want to use the program, press ( 1 )\nelse if you want to exit, press ( 2 )";

    sleepCout(title);
    sleepCout(description);

    int input = 0;

    cin >> input;

    if (input == 1)
    {
        return true;
    }
    return false;

}


void Service::sleepCout(string_view str, int ms)
{
    for (auto i: str)
    {
        cout << i << flush;
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
    cout << "\n\n" << flush;
}


bool Service::ConditionSortLogger()
{

    string howToUse =
                "How to use\n"
                "1. Enter the path to the desired log.txt file.\n"
                "2. Enter a positive number N and N regular expressions to use.\n"
                "3. Enter the desired data storage file name\n"
                "4. Please wait";

    sleepCout(howToUse);

}