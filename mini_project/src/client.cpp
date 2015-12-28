#include "ros/ros.h"
#include <mini_project/Chat.h>
#include <string>
#include <fstream>
#include <pthread.h>
using namespace mini_project;



void *refresh(void *ptr)
{
    ros::NodeHandle n;
    ros::ServiceClient client = n.serviceClient<Chat>("chat_server");
        Chat srv;
        int maxl;
        srv.request.update=false;
        srv.request.name="IntervalRefresh";
        srv.request.msg="IntervalRefresh";
        while (ros::ok()){
        srv.request.time=0;//ros::WallTime::now().toSec();

        if (client.call(srv))
        {
            std::cout << "\33[s";
            for (int i=0; i<srv.response.name.size(); i++) std::cout << "\33[1F";
            maxl=0;
            for (int i=0; i<srv.response.name.size(); i++)
                if (srv.response.name[i].length()>maxl)
                    maxl=srv.response.name[i].length();
            for (int i=0; i<srv.response.name.size(); i++)
            {
                std::cout << "\33[0K" << std::setw(maxl) << std::left << srv.response.name[i];
                std::cout << "|" << srv.response.msg[i] << std::endl;
            }
            std::cout << "\33[u";
        }
        else
        {
            ROS_ERROR("Failed to reach the server");
        }
        ros::Duration(1).sleep();
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


    int maxl=0;
    //std::thread updates(refresh,client);
    pthread_t thread;
    pthread_create (&thread, NULL, *refresh, NULL);
    while (ros::ok())
    {
        std::cout << srv.request.name << ": ";
        std::getline(std::cin,srv.request.msg);
        srv.request.time=ros::WallTime::now().toSec();
        srv.request.update=true;
        if (client.call(srv))
        {
        /*
            for (int i=0; i<100; i++) std::cout << std::endl;
            for (int i=0; i<srv.response.name.size(); i++)
                if (srv.response.name[i].length()>maxl)
                    maxl=srv.response.name[i].length();
            for (int i=0; i<srv.response.name.size(); i++)
            {
                std::cout << std::setw(maxl) << std::left << srv.response.name[i];
                std::cout << "|" << srv.response.msg[i] << std::endl;
            }
            */
        }
        else
        {
            ROS_ERROR("Failed to reach the server");
        }
    }
    return 0;
}
