#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <nmea_msgs/Sentence.h>
#include <ros/ros.h>




int main(int argc, char** argv)
{



  ros::init(argc, argv, "gnss_data");

  ros::NodeHandle n;

  ros::Publisher pub = n.advertise<nmea_msgs::Sentence>("/gnss_sentence", 100);

  ros::Rate loop_rate(10);//一秒发送多少次



  //ros::spin();
  while (ros::ok())
  {
    //coder.extrinsic ('getTime');

    nmea_msgs::Sentence gnss_msg;

    gnss_msg.sentence="$GPGGA,004325.80,3514.1850909,N,13700.2259989,E,1,14,0.78,40.9754,M,38.4545,M,,*60";
    
    gnss_msg.header.frame_id="/gps";

    ros::Time currentTime = ros::Time::now(); //生成时间戳

    gnss_msg.header.stamp = currentTime;

    pub.publish(gnss_msg);

    ros::spinOnce();
 
    loop_rate.sleep();

  }

  return 0;
}

