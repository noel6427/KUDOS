#include "v3_rmd_can/cancan.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Float32.h"
#include <stdint.h>

#include <pthread.h>
#include <unistd.h>
#include <time.h>

void print_CAN(can_frame &frame);

int Control_Cycle = 30; //ms 30
bool run_status = true;

double dt = 30;   //ms 30
double T = 3300; //5 sec //3300
double t = 0;

int Rmd_Goal_Posi[5] = {0, 180, 360, 540, 720};
int indx = 0;

double Present_Posi_01 = 0;
double Present_Posi_02 = 0;
double Goal_Pos2;
//double Goal_Posi = 360.0 * DEG2RAD;
float Cmmd_Posi_01 = 0; //double
float Cmmd_Posi_02 = 0;  //double
double Now_pos_01 = 0;
double Now_pos_02 = 0;

double posi_test = 0;
CAN _can("can0", "CAN0");

void process() //double Goal_Posi
{
  Present_Posi_01 = _can.Enco_Data_01 * DEG2RAD;
  Present_Posi_02 = _can.Enco_Data_02 * DEG2RAD; // _can.Enco_Data * DEG2RAD;

  if(t <= T) { //t <= T
    //Cmmd_Posi_01 = Present_Posi_01 + (Goal_Posi*DEG2RAD - Present_Posi_01)*0.5*(1.0-cos(PI*(t/T)));
    Cmmd_Posi_01 = Present_Posi_01 + (Rmd_Goal_Posi[indx] *DEG2RAD - Present_Posi_01)*0.5*(1.0-cos(PI*(t/T)));
    //Cmmd_Posi_02 = Present_Posi_02 + (Goal_Posi*DEG2RAD - Present_Posi_02)*0.5*(1.0-cos(PI*(t/T)));
    Cmmd_Posi_02 = Present_Posi_02 + (Rmd_Goal_Posi[indx] *DEG2RAD - Present_Posi_02)*0.5*(1.0-cos(PI*(t/T)));
    printf("t: [%lf]\n", t);
    //Now_pos_01 = _can.Read_RMD_Data_01();
    //Now_pos_02 = _can.Read_RMD_Data_02();  //_can.Read_RMD_Data();
    printf("01_Deg_Enco: %lf    ", Cmmd_Posi_01* RAD2DEG);
    printf("02_Deg_Enco: %lf\n", Cmmd_Posi_02* RAD2DEG);
    //ROS_INFO("%lf\n", Now_pos_01);
    //ROS_INFO("%lf\n", Now_pos_02);

    _can.Position_GoV3(0x141, Cmmd_Posi_01 * RAD2DEG);
    usleep(2000);
    _can.Position_GoV3(0x142, Cmmd_Posi_02 * RAD2DEG);

    t = t + dt;
  }

  else if (indx == 0) {
      indx = 1;
      Present_Posi_01 = Rmd_Goal_Posi[0];
      Present_Posi_02 = Rmd_Goal_Posi[0];
      t = 0;
      //usleep(2000);
      }
   else if (indx == 1) {
      indx = 2;
      Present_Posi_01 = Rmd_Goal_Posi[1];
      Present_Posi_02 = Rmd_Goal_Posi[1];
      t = 0;
      //usleep(2000);
      }
   else if (indx == 2) {
      indx = 3;
      Present_Posi_01 = Rmd_Goal_Posi[2];
      Present_Posi_02 = Rmd_Goal_Posi[2];
      t = 0;
      //usleep(2000);
      }
   else if (indx == 3){
      indx = 4;
      Present_Posi_01 = Rmd_Goal_Posi[3];
      Present_Posi_02 = Rmd_Goal_Posi[3];
      t = 0;
      //usleep(2000);
      }
  else if (indx == 4){
     //indx = 5;
     Present_Posi_01 = Rmd_Goal_Posi[4];
     Present_Posi_02 = Rmd_Goal_Posi[4];
     t = 0;
     //usleep(2000);
     }
     //t = 0;
    }

 /* else if (posi_test == 0) {
      t = 0;
      posi_test = 360;
      //usleep(1000);
    }
 else if (posi_test == 360) {
     t = 0;
     posi_test = 0;
    }

  }*/
  //usleep(1000);

 void *p_function(void * data)
{
  ROS_INFO("thread_created...");
  sleep(1);

  pid_t pid; //process id
  pthread_t tid; // thread id

  pid = getpid();
  tid = pthread_self();

  char* thread_name = (char *)data;
  int i = 0;

  static struct timespec next_time;
  static struct timespec curr_time;

  clock_gettime(CLOCK_MONOTONIC, &next_time);

  while(run_status)
  {
    next_time.tv_sec += (next_time.tv_nsec + Control_Cycle * 1000000) / 1000000000;
    next_time.tv_nsec = (next_time.tv_nsec + Control_Cycle * 1000000) % 1000000000;

    process(); //posi_test

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, NULL);

  }
  return 0;
}


int main(int argc, char **argv){
  ros::init(argc, argv, "x8_v3_can");
  ros::NodeHandle nh;

  ros::Publisher cmmd_pub_01 = nh.advertise<std_msgs::Float32>("cmmd_pub_01", 1000);
  ros::Publisher cmmd_pub_02 = nh.advertise<std_msgs::Float32>("cmmd_pub_02", 1000);

  //CAN CAN("can0", "ttyACM0");
  _can.CAN_initialize(_1M);

  struct can_frame rx_frame;

  for(int i =0; i<3; i++) {
  _can.RPM_Control(0x141, 0);
  usleep(2000);
  _can.RPM_Control(0x142, 0);
  usleep(500000);
  }

    _can.Read_RMD_Data();
    _can.Read_RMD_Data();
    _can.Read_RMD_Data();

    //_can.close_port();
    //return  0;

  pthread_t pthread_1;
  int thr_id;
  int status;
  char p1[] = "thread_1";

  sleep(1); //1

  thr_id = pthread_create(&pthread_1, NULL, p_function, (void*)p1);

  ros::Rate loop_rate(100); //
  while (ros::ok())
  {
    //CAN.CanId_Set(0);

    //_can.Position_GoV3(0x141, 0);
    //_can.Position_GoV3(0x142, 0);
    //CAN.Position_GoV3(0x143, 0);
    //CAN.Read_RMD_Data();
    //print_CAN(rx_frame);

    //CAN.Motor_Stop(0x142);

    //_can.ReadAngleCmmd(0x142);

    std_msgs::Float32 msg1;
    msg1.data = Cmmd_Posi_01;
    std_msgs::Float32 msg2;
    msg2.data = Cmmd_Posi_02;
    cmmd_pub_01.publish(msg1);
    cmmd_pub_02.publish(msg2);

    ros::spinOnce();

    loop_rate.sleep();
  }

  run_status = false;

  _can.close_port();
  return 0;
}

void print_CAN(struct can_frame &frame) {
  printf("\n");
  printf("can_id     : %x\n",frame.can_id);
  printf("can_length : %d\n",frame.can_dlc);
  printf("can_data   :\n");
  printf("[0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X]\n",
         frame.data[0],
         frame.data[1],
         frame.data[2],
         frame.data[3],
         frame.data[4],
         frame.data[5],
         frame.data[6],
         frame.data[7] );
  printf("\n");
}


