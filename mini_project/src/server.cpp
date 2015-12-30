#include "ros/ros.h"
#include "std_msgs/String.h"
#include <mini_project/Chat.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>

using namespace mini_project;


bool process(Chat::Request &req, Chat::Response &res) // this is a function to process messages received from clients.
{
    if (req.update==true) // if we said that we sent an actual message to server from client
    {
        if (req.name.length()>0&&req.msg.length()>0) // and if it's not empty
        {
            std::ofstream file("session.txt", std::ofstream::app); //open a file called session.txt, and write to the end of the file, not removing what was written before
            file << req.time << "\n"; // write the time of the message to file
            file << req.name << "\n"; // write the name of the user to file
            file << req.msg << "\n"; // write the message to file.
            file.close(); //close the file.
        }
        else
            ROS_ERROR("msg dropped"); // if the message was empty write msg dropped
    }
    std::ifstream fd("session.txt"); // open the file session.txt
    char temp_name[21],temp_msg[256]; // create temporary variables to store name and message
    int temp_time; // create temporary variable for the time.
    while (!fd.eof()) //while not at the end of file
    {
        fd >> temp_time;  // read the time of the message that was saved in file.
        fd.ignore(256,'\n'); // go to new line
        fd.get(temp_name,256,'\n'); // read the name of the user that sent the message from file.
        fd.ignore(256,'\n'); // go to new line
        fd.get(temp_msg,256,'\n'); // read the message that was saved in the file.
        fd.ignore(256,'\n'); // go to new line.
        if (temp_time>req.last_update) // if the time of the message written in file is more recent than the last update time that was given to us from the client
        {
            res.time.push_back(temp_time); // save the time to the response message to the client
            res.name.push_back(temp_name); // save the name to the response message to the client
            res.msg.push_back(temp_msg); // save the message to the response message to the client
        }
        if (res.name.size()>0&&res.msg.size()>0) // if there's more than one message to send
            if (res.name.back().compare("")==0&&res.msg.back().compare("")==0) // if the last message saved to send to the client is empty
            {
                res.time.pop_back(); // remove the last entry
                res.name.pop_back(); // remove the last entry
                res.msg.pop_back(); // remove the last entry
            }
    }
    return true; //return true to the client.
}



int main(int argc, char **argv)
{

    ros::init(argc, argv, "chat_server"); // initialize ros in this program. and give this node a name "chat server"

    ros::NodeHandle n; // create node handle

    std::ofstream file("session.txt"); // open a file in ouput mode and delete everything that was in it
    file << std::fixed << (int)ros::WallTime::now().toSec() << "\n" << "Server" << "\n" << "Server started"<<"\n"; // fixed makes sure the time isn't written in scientific notation, we would lose the last digits because of that, and writes other stuff into the file.
    file.close(); // close the file.

    ros::ServiceServer service = n.advertiseService("chat_server",process);// tell to roscore that chat server exists and other programs can send and receive messages from us and when we receive a message call process function.
    ROS_INFO("Chat server running"); // print out that the server is running
    ros::spin(); // and wait for messages to come.

    return 0; // end program, while the program is running the ros::spin will prevent this but as soon as we press CTRL+c ros::spin will stop and this will end the program.
}




