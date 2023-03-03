#include "ros/ros.h"
#include "open_manipulator_teleop/open_manipulator_teleop_keyboard.h"
#include <cstdlib>\

bool setxyz_(open_manipulator_msgs::setxyz::Request &req,
            open_manipulator_msgs::setxyz::Response &res)
{
  float input_x,input_y,input_z;
  std::vector<double> kinematics_pose;
  double path_time = 2.0;

//ROS_INFO("request: x=%ld, y=%ld", (long int)req.a, (long int)req.b);
//ROS_INFO("sending back response: [%ld]", (long int)res.sum);

return true;
}

int main(int argc, char **argv)
{
ros::init(argc, argv, "setxyz_client");
ros::NodeHandle n;

ros::ServiceServer service = n.advertiseService("setxyz_ints", add);
ROS_INFO("x=%.3lf, y=%.3lf, z=%.3lf\n",input_x,input_y,input_z);
ros::spin();

return 0;
}
