#ifndef CAN_OLA_H
#define CAN_OLA_H

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int8.h"

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdlib.h>

#define PI 3.14159265

/***************RPM*************/
#define DPS2RPM       0.166666
#define RPM2DSP       6.0000024
/*******************************/

/***************ENO************/
#define DEG2LSP       100
#define ENCODER_RANGE 32767      // 16bit encoder // +-32767 // 0~65535 // 16383
#define DEG2ENC       91.0194   // 65536 / 360(deg)=182.0444 // 16383/360=45.50833
#define ENC2DEG       0.010987  // 360/32767=0.010987 // 360 / 65536=0.005493   // 360/16383=0.002197
#define DEG2RAD       0.017453   // PI / 180
#define RAD2DEG       57.29578   // 180 / PI
/******************************/

/**************************Motor Command**********************/
#define CANID_SETTING_CMMD                               0x79
#define MOTOR_OFF_CMMD                                   0X80
#define MOTOR_STOP_CMMD                                  0X81
#define MOTOR_RUNNING_CMMD                               0X88
#define SPEED_CLOESED_LOOP_CMMD                          0XA2
#define POSITION_CLOSED_LOOP_CMMD_1                      0XA3
#define POSITION_CLOSED_LOOP_CMMD_2                      0xA4
#define POSITION_CLOSED_LOOP_CMMD_3                      0xA5
#define POSITION_CLOSED_LOOP_CMMD_4                      0xA6
#define MULTITURN_INTRIMENTAL_POSITION_CONTROL_CMMD_1    0XA7
#define MULTITURN_INTRIMENTAL_POSITION_CONTROL_CMMD_2    0XA8
/**************************************************************/

using namespace std;

typedef unsigned char  BYTE;  //8bit
typedef unsigned short BYTE2; //16bit
typedef unsigned int BYTE4;   //32bit


/**
 * struct can_frame - basic CAN frame structure
 * @can_id:  the CAN ID of the frame and CAN_*_FLAG flags, see above.
 * @can_dlc: the data length field of the CAN frame
 * @data:    the CAN frame payload.
 */
/// https://sites.uclouvain.be/SystInfo/usr/include/linux/can.h.html



enum Bit_rate {
  _10k = 0,
  _20k,
  _50k,
  _100k,
  _125k,
  _250k,
  _500k,
  _750k,
  _1M
};

class CAN
{

  private:
    int soc;
    int read_can_port;
    string port_name;        //socket CAN port name. such as "can0"
    string device_name;  //Port of USB_to_CAN device port such as "ttyACM0". Can find it in "/dev" directory.

  public:
    CAN(string port_name_, string device_name_);
    ~CAN();

    int16_t Enco_Data_01;
    int16_t Enco_Data_02;
    int16_t Enco_Data_03;
    int32_t M2_Enco;
    double dt = 1;   //1 ms
    double T = 5000; //5 sec
    double t = 0;
    double Present_Posi = 0;
    double Goal_Posi = 0.0 * DEG2RAD;
    //double Cmmd_Posi = 0;
    //double PI = 3.14;

    int open_port(const char *port);
    int send_port(struct can_frame *frame);
    void read_port();
    int close_port();

    void CAN_initialize(int bit_rate_mode);
    void set_can_frame(struct can_frame &canframe, int CAN_id, u_int8_t CAN_data_len, bool is_ext_mode);
    void set_can_frame2(struct can_frame &canframe, u_int8_t CAN_data_len, bool is_ext_mode);
    void CAN_write(struct can_frame &frame, BYTE data_array[]);

    struct can_frame CAN_read(void);
    bool read_CAN(struct can_frame &recv_frame);
    void Read_X8_Data();
    void Read_RMD_Data();

    bool read_CAN_01(struct can_frame &recv_frame);
    double Read_RMD_Data_01();
    bool read_CAN_02(struct can_frame &recv_frame);
    double Read_RMD_Data_02();

    void *p_function(void * data);
    void process(double Goal_Posi);

    //double Read_RMD_Data_3();

    //struct ReadAngleCmmd(void);
    void CanId_Set(uint16_t RMD_id);
    void RPM_Control(int RMD_id, int32_t rpm);
    void Position_Go(int RMD_id, int Position_Deg);
    void Position_GoV3(int RMD_id, int Pos_Deg);
    void Motor_Stop(int RMD_id);
    void rmdx8_write(BYTE data_array[]);
};



#endif // CAN_OLA_H
