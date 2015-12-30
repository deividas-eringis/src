#include "ros/ros.h"
#include <mini_project/Chat.h>
#include <string>
#include <fstream>
#include <pthread.h>
using namespace mini_project;

std::vector<std::string> msg;
std::vector<std::string> name;
int last_update;
void update()
{
    int maxl;
    maxl=0;
    for (int i=0; i<name.size(); i++)
        if (name[i].length()>maxl)
            maxl=name[i].length();
    for (int i=0; i<100; i++) std::cout << std::endl;
    for (int i=0; i<name.size(); i++)
    {
        std::cout << std::setw(maxl) << std::left << name[i];
        std::cout << "|" << msg[i] << std::endl;
    }
}



void *refresh(void *ptr)
{
    ros::NodeHandle n;
    ros::ServiceClient client = n.serviceClient<Chat>("chat_server");
    Chat srv;
    srv.request.update=false;
    srv.request.name="IntervalRefresh";
    srv.request.msg="IntervalRefresh";
    while (ros::ok())
    {
        srv.request.last_update=last_update;
        if (client.call(srv))
        {
            for (int i=0; i<srv.response.name.size(); i++)
            {
                name.push_back(srv.response.name[i]);
                msg.push_back(srv.response.msg[i]);
            }
            update();
            last_update=ros::WallTime::now().toSec();
            ros::Duration(5).sleep();
        }
    }
}





int main(int argc, char **argv)
{

    Chat srv;
    srv.request.name="";
    while (srv.request.name.compare("")==0||(srv.request.name.length()>20&&srv.request.name.length()>0))
    {
        std::cout << "Choose a username: ";
        std::getline(std::cin,srv.request.name);
        if (srv.request.name.length()>20)
            std::cout << "name must be less than 20 characters" << std::endl;
        if (srv.request.name.length()==0)
            std::cout << "name must have at least 1 character" << std::endl;
    }
    ros::init(argc, argv, "chat_client"+srv.request.name);

    ros::NodeHandle n;
    ros::ServiceClient client = n.serviceClient<Chat>("chat_server");
    if (ros::service::waitForService("chat_server",10000)==false)
    {
        ROS_ERROR("Server unavailable");
        return 0;
    }
    last_update=0;
    pthread_t thread;
    pthread_create (&thread, NULL, *refresh, NULL);
    while (ros::ok())
    {
        std::getline(std::cin,srv.request.msg);
        srv.request.time=ros::WallTime::now().toSec();
        srv.request.last_update=last_update;
        srv.request.update=true;
        if (client.call(srv))
        {
            for (int i=0; i<srv.response.name.size(); i++)
            {
                name.push_back(srv.response.name[i]);
                msg.push_back(srv.response.msg[i]);
            }
            last_update=ros::WallTime::now().toSec();
            update();
        }
        else
        {
            ROS_ERROR("Failed to reach the server");
        }
    }
    return 0;
}
