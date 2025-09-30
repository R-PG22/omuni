#include "mbed.h"
BufferedSerial pc(USBTX, USBRX, 115200);
BufferedSerial esp(PA_9, PA_10, 115200);
int16_t pwm[4] = {0, 0, 0, 0};
int rx_X = 0;
int rx_Y = 0;
int rx_Turn = 0;
int read_num;
const int Motor_Max_Power = 22500;
uint8_t power;
int R_adj; // 旋回の補正値

int main(){
    CAN can(PA_11, PA_12, (int)1e6); // canを出力するピンを指定
    CANMessage msg;// 変数「msg」の作成
    char buf[64];
    int buf_index = 0;
    power = Motor_Max_Power / 150; // モーター出力の補正値:150
    R_adj = Motor_Max_Power / 127; // 旋回の補正値:177

    while(1){
        if(esp.readable()){
            char c;
            int len = esp.read(&c, 1);
            if (len > 0){
                if (c == '|'){
                    if(buf_index > 0){
                        buf[buf_index] = '\0';
                        buf_index = 0;
                        read_num = atoi(buf);
                        if (read_num > 383){
                            rx_Turn = (read_num - 512) * R_adj;
                        }
                        else if (read_num > 127){
                            rx_Y = read_num - 256;
                        }else {
                            rx_X = read_num;
                        }
                    }
                }else{
                    if (buf_index < sizeof(buf) - 1){
                        buf[buf_index++] = c;
                    }
                }
            }
        }
        float rad = 90.0f - (atan2(rx_Y, rx_X) * 180.0f / M_PI);
        if (rad < 0){
            rad += 360.0;
        }
        float speed = hypot(rx_X, rx_Y);
        if (abs(rx_X) <= 10 && abs(rx_Y) <= 10) speed = 0;
        if (abs(rx_Turn) <= 10) rx_Turn = 0;
        pwm[0] = (sin((rad - 45) * M_PI / 180.0f) * speed * power) + rx_Turn;
        pwm[1] = (sin((rad - 135) * M_PI / 180.0f) * speed * power) + rx_Turn;
        pwm[2] = (sin((rad - 225) * M_PI / 180.0f) * speed * power) + rx_Turn;
        pwm[3] = (sin((rad - 315) * M_PI / 180.0f) * speed * power) + rx_Turn;
        CANMessage msg(2, (const uint8_t *)pwm, 8); //特に理由がない限りwhile直下
        can.write(msg); //特に理由がない限りwhile直下
        
        printf("%d %d\n", pwm[3],pwm[0]);
        printf("%d %d\n", pwm[2],pwm[1]);
        // printf("%d\n",rx_X);
        // printf("%d, %d\n",rx_X,rx_Y);
        printf("\n");
        // ThisThread::sleep_for(chrono::milliseconds(10));
    }
    return 0;
}