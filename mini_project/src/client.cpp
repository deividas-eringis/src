#include "ros/ros.h"
#include <mini_project/Chat.h>
#include <string>
#include <fstream>
#include <pthread.h>
using namespace mini_project;

std::vector<std::string> msg;   //Storage for all messages, vector because we need flexible size
std::vector<std::string> name;  //Storage for who wrote the messages.
int last_update;                //time from ros time, when the last update was done.


void update() //In general this fuction deals with printing stuff in the terminal
{
    int maxl; //initialize integer maxl, meaning max length.
    maxl=0;
    for (int i=0; i<name.size(); i++) // for every entry in the name storage, name.size() gives the number of entries in storage
        if (name[i].length()>maxl) // if the name is longer than value stored in maxl.
            maxl=name[i].length(); // change maxl to the length of the name.
    for (int i=0; i<100; i++) std::cout << std::endl; // for 100 times print new line in the terminal.
    for (int i=0; i<name.size(); i++) // again for every entry in the name storage.
    {
        std::cout << std::setw(maxl) << std::left << name[i]; // if used setw(20) and the name lenght is 10, then it will write the name and 10 more spaces and std::left makes the name allign to the left.
        std::cout << "|" << msg[i] << std::endl; // write | and the message without the setw and left formatting, and after message go to new line
    }
}



void *refresh(void *ptr) // this is like the main program, but for the new thread.
{
    ros::NodeHandle n; //node handle is required for the service to function
    ros::ServiceClient client = n.serviceClient<Chat>("chat_server"); //we create an object called "client" with witch we can send and receive messages from server, with a service name "chat_server"
    Chat srv; //create Chat object called "srv" that we will be sending to the server using the client object.
    srv.request.update=false; // the server checks for this true or false, if it's false server understand that we are not sending a message but we are asking for messages from other people
    srv.request.name="IntervalRefresh"; // just because they have to be filled in, the server will ignore this anyway
    srv.request.msg="IntervalRefresh"; // just because they have to be filled in, the server will ignore this anyway
    while (ros::ok()) //while the program is running and not asked to stop. if this wasn't here and we pressed CTRL+C the program wouldn't stop
    {
        srv.request.last_update=last_update; // fill in when the last update for our client was so the server knows which messages to send back to us and which ones we already have.
        if (client.call(srv)) // send srv object to the server and fill the srv object with the response from the server, so srv.response now has data from server
        {
            for (int i=0; i<srv.response.name.size(); i++) // for all entries in the response.name vector, (the name storage as before in the program)
            {
                name.push_back(srv.response.name[i]); // increase the size of the client's name storage and input the name entry into the storage
                msg.push_back(srv.response.msg[i]); // same here but for the messages.
            }
            update(); // run update fuction witch was previously written, basically updating the screen with all of the names and messages
            last_update=ros::WallTime::now().toSec(); // save when the last update was done
            ros::Duration(5).sleep(); // wait for 5 seconds until continuing.
        }
    }
}





int main(int argc, char **argv) // this is where the program actually starts, argc is the number of arguments and argv is the array of arguments but we are not using them in the program so just ignore these
{
    Chat srv; //create Chat object in the main function.
    srv.request.name=""; // set the name to nothing
    while (srv.request.name.compare("")==0||(srv.request.name.length()>20&&srv.request.name.length()>0)) // this continues if the name is nothing or the name is longer than 0 characters and less than 20
    {
        std::cout << "Choose a username: "; // print choose a username
        std::getline(std::cin,srv.request.name); // read what the user has written and save it to srv.request.name
        if (srv.request.name.length()>20) // if the name is longer than 20 charaters print that out to the terminal.
            std::cout << "name must be less than 20 characters" << std::endl;
        if (srv.request.name.length()==0) // if user inputed nothing say that.
            std::cout << "name must have at least 1 character" << std::endl;
    }
    ros::init(argc, argv, "chat_client"+srv.request.name); // initialize ros interface for this program, without this everything using ros wouldn't work.

    ros::NodeHandle n; // a node handle to handle our node.
    ros::ServiceClient client = n.serviceClient<Chat>("chat_server"); // create a client to communicate with the server.
    if (ros::service::waitForService("chat_server",10000)==false) // if the server doesn't turn on in 10 stop the program
    {
        ROS_ERROR("Server unavailable"); // same as printing, but prints it out in red and with time stamp.
        return 0;// this ends the function and because it's the main fuction ends the program.
    }
    last_update=0; // we haven't updated the messages in our client, so the last update was at the beggining of time
    pthread_t thread; // create a thread object
    pthread_create (&thread, NULL, *refresh, NULL); // and run the new thread using function refresh as the threads main function.
    while (ros::ok()) // while the program is running
    {
        std::getline(std::cin,srv.request.msg); // read what the user has inputted and save it to srv.request.msg
        srv.request.time=ros::WallTime::now().toSec(); // save at what time the message was inputed
        srv.request.last_update=last_update; // input when the last update was to the Chat object called srv that we will be sending to the server.
        srv.request.update=true; // set this to true so the server knows to save our message.
        if (client.call(srv)) // contact the server and fill srv.response with the response from the server.
        {
            for (int i=0; i<srv.response.name.size(); i++) // for every name in the response
            {
                name.push_back(srv.response.name[i]); //save name in our storage
                msg.push_back(srv.response.msg[i]); // save msg in our storage
            }
            last_update=ros::WallTime::now().toSec(); //save when the last update occured
            update(); // update the screen
        }
        else
        {
            ROS_ERROR("Failed to reach the server"); // if we failed to send and receive information from server print out failed to reach the server.
        }
    }
    return 0; // end the program, while the while loop is runing this will never happen.
}
