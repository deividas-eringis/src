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
	char temp[256];
	std::string temps;
	int x;
	while (!fd.eof()){
	fd >> x;

    res.time.push_back(x);
    std::cout << x << "@"<<std::endl;
    fd.ignore(256,'\n');
	fd.get(temp,256,'\n');
	res.name.push_back(temp);
    std::cout << temp << "?" <<std::endl;
    fd.ignore(256,'\n');
	fd.get(temp,256,'\n');
	res.msg.push_back(temp);
        std::cout << temp << "!"<< std::endl;
    fd.ignore(256,'\n');
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

  ros::ServiceServer service = n.advertiseService("chat_server",process);
  ROS_INFO("Chat server running");
  ros::spin();

  return 0;
}




