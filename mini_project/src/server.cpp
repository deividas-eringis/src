#include "ros/ros.h"
#include "std_msgs/String.h"
#include <mini_project/Chat.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
using namespace mini_project;


bool process(Chat::Request &req, Chat::Response &res){
    if (req.update==true){
	std::ofstream file("session.txt", std::ofstream::app);
	file << req.time << "\n";
	file << req.name << "\n";
	file << req.msg << "\n";
	file.close();
    }
	std::ifstream fd("session.txt");
	char temp_name[21],temp_msg[256];
	int x;
	while (!fd.eof()){
	fd >> x;
    fd.ignore(256,'\n');
	fd.get(temp_name,256,'\n');
    fd.ignore(256,'\n');
	fd.get(temp_msg,256,'\n');
    fd.ignore(256,'\n');
    if (x>=req.time){
    res.time.push_back(x);
    res.name.push_back(temp_name);
    res.msg.push_back(temp_msg);
    }
    if (res.name.size()>0&&res.msg.size()>0)
    if (res.name.back().compare("")==0&&res.msg.back().compare("")==0){
        res.time.pop_back();
        res.name.pop_back();
        res.msg.pop_back();
    }
	}
	return true;
}



int main(int argc, char **argv)
{

  ros::init(argc, argv, "chat_server");

  ros::NodeHandle n;
std::cout << "please?" << std::endl;
std::ofstream file("session.txt");

file << std::fixed<< (int)ros::WallTime::now().toSec() << "\n" << "Server" << "\n" << "Server started"<<"\n";

file.close();
std::cout << "first line" << std::endl << "second line" << std::endl << "\33[1A" << "test";
  ros::ServiceServer service = n.advertiseService("chat_server",process);
  ROS_INFO("Chat server running");
  ros::spin();

  return 0;
}




