#include "v3_rmd_can/cancan.h"

CAN::CAN(string port_name_, string device_name_)
{
  this->port_name = port_name_;
  this->device_name = device_name_;
}

CAN::~CAN(){
  close_port();
}

int CAN::open_port(const char *port)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    /* open socket */
    soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(soc < 0){
        return (-1);
    }

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, port);

    if (ioctl(soc, SIOCGIFINDEX, &ifr) < 0){
        return (-1);
    }

    addr.can_ifindex = ifr.ifr_ifindex;
    fcntl(soc, F_SETFL, O_NONBLOCK);

    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        return (-1);
    }

    return 0;
}

int CAN::send_port(struct can_frame *frame)
{
    int retval;
    retval = write(soc, frame, sizeof(struct can_frame));
    if (retval != sizeof(struct can_frame)){
      return (-1);
    }
    else{
      return (0);
    }
}

int CAN::close_port()
{
    close(soc);
    return 0;
}

void CAN::CAN_initialize(int bit_rate_mode){
/*
  * (can bit-rate)
  * -s0 : 10k(bps)
  * -s1 : 20k
  * -s2 : 50k
  * -s3 : 100k
  * -s4 : 125k
  * -s5 : 250k
  * -s6 : 500k
  * -s7 : 750k
  * -s8 : 1M
  */

/*
 * Note
 * You have to replace '111111' to your linux Password.
 */

  string command_0 = "echo '111111' | sudo -S slcand -o -c -s0 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
  string command_1 = "echo '111111' | sudo -S slcand -o -c -s1 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";//"sudo slcand -o -c -s1 /dev/"+ port_name + " can1 && sudo ifconfig can1 up && sudo ifconfig can1 txqueuelen 1000";
  string command_2 = "echo '111111' | sudo -S slcand -o -c -s2 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
  string command_3 = "echo '111111' | sudo -S slcand -o -c -s3 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
  string command_4 = "echo '111111' | sudo -S slcand -o -c -s4 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
  string command_5 = "echo '111111' | sudo -S slcand -o -c -s5 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
  string command_6 = "echo '111111' | sudo -S slcand -o -c -s6 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
  string command_7 = "echo '111111' | sudo -S slcand -o -c -s7 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
  string command_8 = "echo '111111' | sudo -S slcand -o -c -s8 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";

  string command = command_8; //초기화
  string bps_s = "10k"; //초기화

  switch (bit_rate_mode)
  {
  case 0:
    command = command_0;
    bps_s = "10k";
    break;
  case 1:
    command = command_1;
    bps_s = "20k";
    break;
  case 2:
    command = command_2;
    bps_s = "50k";
    break;
  case 3:
  command = command_3;
  bps_s = "100k";
  break;
  case 4:
    command = command_4;
    bps_s = "125k";
    break;
  case 5:
    command = command_5;
    bps_s = "250k";
    break;
  case 6:
    command = command_6;
    bps_s = "500k";
    break;
  case 7:
    command = command_7;
    bps_s = "750k";
    break;
  case 8:
    command = command_8;
    bps_s = "1M";
    break;

  default:
    command = command_5;
    bps_s = "10k";
    break;
  }

  const char *c = command.c_str();
  const char *c2 = bps_s.c_str();

  int i;
  int count = 0;


    for(i=0;i<5;i++){
    if(system(c) == 0){       //터미널에 명령 전달
      ROS_INFO("Set bit_rate %s",c2); //-s5
      ROS_INFO("%s",c);
      break;
    }
    else{
      count ++;
    }
  }

  if(count>4){
    ROS_ERROR("CAN init Setting Failed!!!");
  }


  const char *port_name_s = this->port_name.c_str();

  for(i=0;i<5;i++){

    if( open_port(port_name_s) == -1 ){            //port_s : can0, can1 etc...
      ROS_WARN("CAN_initalize_Failed");
    }
    else{
      ROS_INFO("initailze_CAN");
      break;
    }
  }

}

void CAN::set_can_frame(struct can_frame &canframe, int CAN_id, u_int8_t CAN_data_len, bool is_ext_mode)
{
  canframe.can_id = CAN_id;
  canframe.can_dlc = CAN_data_len;
  //printf("[%X]\n", canframe.can_id);

  if(is_ext_mode){
    canframe.can_id |= CAN_EFF_FLAG;    //extended CAN mode FLAG
  }
}

struct can_frame CAN::CAN_read(void)
{
  struct can_frame frame_rd;
  int result = read(soc, &frame_rd, sizeof(struct can_frame));
  if(result<1) {
      ROS_WARN("CAN_read_failed");
  }

  return frame_rd;
}

bool CAN::read_CAN(struct can_frame &recv_frame)
{
  struct can_frame frame_rd; 
  int result = read(soc, &frame_rd, sizeof(struct can_frame));
  if(result<1){
      //ROS_WARN("CAN_read_failed");
      return false;
  }
  recv_frame = frame_rd;
  return true;
}

double CAN::Read_RMD_Data_01() { //void
  struct can_frame recv_frame;
  read_CAN(recv_frame);

  //Enco_Data = (int16_t)recv_frame.data[6]+((int16_t)recv_frame.data[7]<<8);

  if(recv_frame.can_id == 0x141) {
    Enco_Data_01 = (int16_t)recv_frame.data[6]+((int16_t)recv_frame.data[7]<<8);
  }
  else if(recv_frame.can_id == 0x142) {
    Enco_Data_02 = (int16_t)recv_frame.data[6]+((int16_t)recv_frame.data[7]<<8);
  }
  printf("01_Enco_Data = [%d]\n", Enco_Data_01);
  printf("02_Enco_Data = [%d]\n", Enco_Data_02);
  //printf("Enco_Deg = [%f]\n", Enco_Data*ENC2DEG);

  return Enco_Data_01;
}

void CAN::Read_RMD_Data() { //double
  struct can_frame recv_frame;
  read_CAN(recv_frame);

  if(recv_frame.can_id == 0x241) {
    Enco_Data_01 = (int16_t)recv_frame.data[6]+((int16_t)recv_frame.data[7]<<8);
    printf("0x141_RMD_Present_Posi_Read   ");
    printf("01_Enco_Data = [%d]\n", Enco_Data_01);
  }
  else if(recv_frame.can_id == 0x242) {
    Enco_Data_02 = (int16_t)recv_frame.data[6]+((int16_t)recv_frame.data[7]<<8);
    printf("0x142_RMD_Present_Posi_Read   ");
    printf("02_Enco_Data = [%d]\n", Enco_Data_02);
  }

  else if(recv_frame.can_id == 0x243) {
    Enco_Data_03 = (int16_t)recv_frame.data[6]+((int16_t)recv_frame.data[7]<<8);
    printf("0x143_RMD_Present_Posi_Read   ");
    printf("03_Enco_Data = [%d]\n", Enco_Data_03);
  }

  //return Enco_Data_02;
}

void CAN::CAN_write(struct can_frame &frame, BYTE data_array[]){

  memcpy(frame.data, data_array, 8); //copy (data_array)->(frame)

  if(send_port(&frame) == -1) {
    ROS_WARN("CAN_write_fuction error");
  }
}

void CAN::RPM_Control(int RMD_id, int32_t rpm)
{
  int32_t speed = rpm  * DEG2LSP;// * RPM2DSP;

  struct can_frame rx_frame1;
  set_can_frame(rx_frame1, RMD_id, 8, false);

  BYTE can_arr1[8]={0, };
  can_arr1[0] = SPEED_CLOESED_LOOP_CMMD;  //0xA2
  can_arr1[1] = 0x00;
  can_arr1[2] = 0x00;
  can_arr1[3] = 0x00;
  can_arr1[4] = (uint8_t)(speed);
  can_arr1[5] = (uint8_t)(speed>>8);
  can_arr1[6] = (uint8_t)(speed>>16);
  can_arr1[7] = (uint8_t)(speed>>24);

  CAN_write(rx_frame1, can_arr1);
}

void CAN::Position_Go(int RMD_id, int Position_Deg)
{
  int32_t angleControl = Position_Deg * DEG2LSP;
  // 0.01degree/LSB ex)36000=360° 1:6 *6

  struct can_frame rx_frame2;
  set_can_frame(rx_frame2, RMD_id, 8, false); //rmd == standard CAN mode FLAG

  BYTE can_arr2[8]={0, };
  can_arr2[0] = POSITION_CLOSED_LOOP_CMMD_1; //0xA3
  can_arr2[1] = 0x00;
  can_arr2[2] = 0x00;
  can_arr2[3] = 0x00;
  can_arr2[4] = *(uint8_t*)(&angleControl);
  can_arr2[5] = *((uint8_t*)(&angleControl)+1);
  can_arr2[6] = *((uint8_t*)(&angleControl)+2);
  can_arr2[7] = *((uint8_t*)(&angleControl)+3);

  CAN_write(rx_frame2, can_arr2);
}

void CAN::Motor_Stop(int RMD_id)
{
  struct can_frame rx_frame3;
  set_can_frame(rx_frame3, RMD_id, 8, false);

  BYTE can_arr3[8]={0, };
  can_arr3[0] = MOTOR_STOP_CMMD; //0x81
  can_arr3[1] = 0x00;
  can_arr3[2] = 0x00;
  can_arr3[3] = 0x00;
  can_arr3[4] = 0x00;
  can_arr3[5] = 0x00;
  can_arr3[6] = 0x00;
  can_arr3[7] = 0x00;

  CAN_write(rx_frame3, can_arr3);
}

void CAN::CanId_Set(uint16_t RMD_id)
{
  uint16_t canid = RMD_id;
  bool wReadWriteFlag = true;

  struct can_frame rx_frame1;
  set_can_frame(rx_frame1, 0x300, 8, false); //0x300 = broadcast ID

  BYTE can_arr3[8]={0, };
  can_arr3[0] = CANID_SETTING_CMMD;  //0x79
  can_arr3[1] = 0x00;
  can_arr3[2] = wReadWriteFlag;//wReadWriteFlag;
  can_arr3[3] = 0x00;
  can_arr3[4] = 0x00;
  can_arr3[5] = 0x00;
  can_arr3[6] = 0x00;
  can_arr3[7] = canid; //canid;

  CAN_write(rx_frame1, can_arr3);
}

void CAN::Position_GoV3(int RMD_id, int32_t Pos_Deg)
{
  int32_t angleControl = Pos_Deg * DEG2LSP;
  // 0.01degree/LSB ex)36000=360° 1:9 *9

  struct can_frame rx_frame2;
  set_can_frame(rx_frame2, RMD_id, 8, false); //rmd == standard CAN mode FLAG

  uint16_t maxSpeed = 360; //1350;

  BYTE can_arr2[8]={0, };
  can_arr2[0] = POSITION_CLOSED_LOOP_CMMD_2; //0xA4
  can_arr2[1] = 0x00;
  can_arr2[2] = (uint8_t)(maxSpeed);
  can_arr2[3] = (uint8_t)(maxSpeed>>8);
  can_arr2[4] = (uint8_t)(angleControl);
  can_arr2[5] = (uint8_t)(angleControl>>8);
  can_arr2[6] = (uint8_t)(angleControl>>16);
  can_arr2[7] = (uint8_t)(angleControl>>24);

  CAN_write(rx_frame2, can_arr2);
}


/* double CAN::ReadAngleCmmd(int RMD_id)
{
  struct can_frame rx_frame5;
  set_can_frame(rx_frame5, RMD_id, 8, false); //rmd == standard CAN mode FLAG

  BYTE can_arr5[8]={0, };
  can_arr5[0] = 0x92;
  can_arr5[1] = 0x00;
  can_arr5[2] = 0x00;
  can_arr5[3] = 0x00;
  can_arr5[4] = 0x00;
  can_arr5[5] = 0x00;
  can_arr5[6] = 0x00;
  can_arr5[7] = 0x00;

  CAN_write(rx_frame5, can_arr5);
  //read_CAN(rx_frame5);

  //M2_Enco = (int32_t)rx_frame5.data[4]+((int32_t)rx_frame5.data[5]<<8)+((int32_t)rx_frame5.data[6]<<16)+((int32_t)rx_frame5.data[7]<<24);
  M2_Enco = ((int32_t)rx_frame5.data[4]<<24)+((int32_t)rx_frame5.data[5]<<16)+((int32_t)rx_frame5.data[6]<<8)+(int32_t)rx_frame5.data[7];
  printf("M2_Enco = [%d]\n", M2_Enco);

  return M2_Enco;
} */

/* struct CAN::ReadAngleCmmd(void)
{
  struct can_frame rx_frame5;
  set_can_frame(rx_frame5, 0x142, 8, false); //rmd == standard CAN mode FLAG

  BYTE can_arr5[8]={0, };
  can_arr5[0] = 0x92;
  can_arr5[1] = 0x00;
  can_arr5[2] = 0x00;
  can_arr5[3] = 0x00;
  can_arr5[4] = 0x00;
  can_arr5[5] = 0x00;
  can_arr5[6] = 0x00;
  can_arr5[7] = 0x00;

  CAN_write(rx_frame5, can_arr5);
  //read_CAN(rx_frame5);


  //M2_Enco = ((int32_t)rx_frame5.data[4]<<24)+((int32_t)rx_frame5.data[5]<<16)+((int32_t)rx_frame5.data[6]<<8)+(int32_t)rx_frame5.data[7];
  //printf("M2_Enco = [%d]\n", M2_Enco);

  return rx_frame5;
}*/

/* double CAN::Read_RMD_Data_3() {
  struct can_frame recv_frame = ReadAngleCmmd();
  read_CAN(recv_frame);

  M2_Enco = (int32_t)recv_frame.data[4]+((int32_t)recv_frame.data[5]<<8)+((int32_t)recv_frame.data[6]<<16)+((int32_t)recv_frame.data[7]<<24);
  printf("Enco_Data = [%d]\n", M2_Enco);
  //printf("Enco_Deg = [%f]\n", Enco_Data*ENC2DEG);

  return Enco_Data;
} */




/*
int16_t a = 0b1100110011001101; //0b = 2진수 표현, 0x = 16진수 표현

int8_t b = (int8_t)a;  // 16bit 정수 a를 8bit 정수 b에 형변환￣여 대입

b == 0b11001101;



8bit 정수 2개를 입력받아 1개 16bit 정수로 리턴받는 함수

int16_t byte2int16(int8_t low, int8_t high)
{
  return (int16_t)low || ((int16_t)high)<<8 ;

}

int16_t a = byte2int16(b,c);
*/


