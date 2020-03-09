#include <iostream>
//#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <fstream>
#include <ros/ros.h>
#include <math.h>

using namespace std;

double x=0,y=0,z=0,yaw=0,yaw_raw=0;


void PoseCallback(const geometry_msgs::PoseStamped::ConstPtr& input)
{
	x = input->pose.position.x;
	y = input->pose.position.y;
	z = input->pose.position.z;
	yaw_raw = input->pose.orientation.z;
        yaw = 2*asin(yaw_raw);


}

int main(int argc, char** argv)
{


  double x_saved=0,y_saved=0,z_saved=0,yaw_saved=0;

  ros::init(argc, argv, "gnss_data_saver");

  ros::NodeHandle n;

  //ros::Publisher pub = n.advertise<nmea_msgs::Sentence>("/gnss_sentence", 100);
  
  ros::Subscriber pose_sub = n.subscribe("/gnss_pose", 100, PoseCallback);//记录gnss_pose的信息

  ros::Rate loop_rate(10);//一秒发送多少次

  ofstream in;
  
  in.open("/home/light/2012_ws/src/gnss_data/csv/test.csv",ios::trunc); //ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建
  in<<"x,y,z,yaw,velocity,change_flag"<<"\n";

  //ros::spin();
  while (ros::ok())
  {
    if(x!=0&&x_saved==0)  //记录第一个点
    {
    	in<<x<<","<<y<<","<<z<<","<<yaw<<",2.0000,0";
	x_saved = x;
        y_saved = y;
        z_saved = z;
        yaw_saved = yaw;
    }

    if(sqrt((x-x_saved)*(x-x_saved)+(y-y_saved)*(y-y_saved))>2) //记录
    {
    	in<<"\n"<<x<<","<<y<<","<<z<<","<<yaw<<",40.0000,0";
	x_saved = x;
        y_saved = y;
        z_saved = z;
        yaw_saved = yaw;
	ROS_INFO("Save data !----------------");
	ROS_INFO("x: %f",x);
	ROS_INFO("y: %f",y);
	ROS_INFO("z: %f",z);
	ROS_INFO("yaw: %f",yaw);

    }

    
    ros::spinOnce();
 
    loop_rate.sleep();

  }

  in.close();//关闭文件

  return 0;
}

