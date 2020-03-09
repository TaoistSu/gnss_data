#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <nmea_msgs/Sentence.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <ros/ros.h>
#include <math.h>

static ros::Publisher estimate_twist_pub;
static geometry_msgs::TwistStamped estimate_twist_msg;

static nmea_msgs::Sentence gnss_msg;

static ros::Time previous_scan_time;

double x=0,y=0,z=0,yaw=0,yaw_raw=0; //当前位置信息
double x_pre=0,y_pre=0,z_pre=0,yaw_pre=0; //前一刻位置信息
double diff_time=0.0,current_velocity=0.0,angular_velocity=0.0;

void PoseCallback(const geometry_msgs::PoseStamped::ConstPtr& input)
{





    x = input->pose.position.x;
    y = input->pose.position.y;
    z = input->pose.position.z;
    yaw_raw = input->pose.orientation.z;
    yaw = 2*asin(yaw_raw);
    ros::Time current_scan_time = input->header.stamp; //获取当前时间做时间戳

    if(x_pre==0)
    {
	previous_scan_time=current_scan_time;
        x_pre = x;
        y_pre = y;
        z_pre = z;
        yaw_pre = yaw;

    }else{


   	diff_time = (current_scan_time - previous_scan_time).toSec();
        std::cout<<"diff_time:"<<diff_time<<std::endl;
        current_velocity = sqrt((x-x_pre)*(x-x_pre)+(y-y_pre)*(y-y_pre)+(z-z_pre)*(z-z_pre))/diff_time; //计算当前速度
	angular_velocity = (yaw-yaw_pre)/diff_time;  //计算当前角速度
	

	previous_scan_time=current_scan_time;
        x_pre = x;
        y_pre = y;
        z_pre = z;
        yaw_pre = yaw;

	    if(current_velocity!=0)
	    
	    {
		if(current_velocity>8)
		{
			current_velocity=8.0;
		}
	   	 estimate_twist_msg.header.stamp = current_scan_time;
	   	 estimate_twist_msg.header.frame_id = "/base_link";
	   	 estimate_twist_msg.twist.linear.x = current_velocity;
		 std::cout<<"current_velocity:"<<current_velocity<<std::endl;
	   	 estimate_twist_msg.twist.linear.y = 0.0;
	   	 estimate_twist_msg.twist.linear.z = 0.0;
	   	 estimate_twist_msg.twist.angular.x = 0.0;
	   	 estimate_twist_msg.twist.angular.y = 0.0;
	   	 estimate_twist_msg.twist.angular.z = angular_velocity;
		 std::cout<<"angular_velocity:"<<angular_velocity<<std::endl;

		}



   	 
	}













    



}

void nmeaSentenceCallback(const nmea_msgs::Sentence::ConstPtr& msg)
{

  if(msg->sentence[2] == 'P') //只要$GPGGA开头的数据
  {
  	std::cout<<msg->sentence<<std::endl;  

    	//nmea_msgs::Sentence gnss_msg;

    	//gnss_msg.sentence="$GPGGA,004325.80,3514.1850909,N,13700.2259989,E,1,14,0.78,40.9754,M,38.4545,M,,*60";
    
   	
	gnss_msg.sentence = msg->sentence;

	gnss_msg.header.frame_id="/gps";

   	ros::Time currentTime = ros::Time::now(); //生成时间戳

   	gnss_msg.header.stamp = currentTime;

   		
  }


}


int main(int argc, char** argv)
{



  ros::init(argc, argv, "gnss_data");

  ros::NodeHandle n;

  ros::Publisher pub = n.advertise<nmea_msgs::Sentence>("/gnss_sentence", 100);

  ros::Publisher estimate_twist_pub = n.advertise<geometry_msgs::TwistStamped>("/current_velocity", 100);
  
  ros::Subscriber sub = n.subscribe("/nmea_sentence", 100, nmeaSentenceCallback);

  ros::Subscriber pose_sub = n.subscribe("/gnss_pose", 100, PoseCallback);//predict_pose



  ros::Rate loop_rate(10);//一秒发送多少次



  //ros::spin();
  while (ros::ok())
  {
    //coder.extrinsic ('getTime');

    
    pub.publish(gnss_msg);  //发布解析后点gps消息


	estimate_twist_pub.publish(estimate_twist_msg);

    ros::spinOnce();
 
    loop_rate.sleep();

  }

  return 0;
}

