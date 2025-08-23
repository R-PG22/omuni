#include "mbed.h"
BufferedSerial pc(USBTX, USBRX, 115200);
BufferedSerial esp(PA_9, PA_10, 115200);
int16_t pwm[4] = {0, 0, 0, 0};
int X;
int Y;
uint8_t power = 150; // モーターの最高出力(22500)÷150

int main(){
    CAN can(PA_11, PA_12, (int)1e6); // canを出力するピンを指定
    CANMessage msg;// 変数「msg」の作成
    char buf[64];
    int buf_index = 0;

    while(1){
        if(esp.readable()){
            char c;
            int len = esp.read(&c, 1);
            if (len > 0){
                if (c == ','){
                    if(buf_index > 0){
                        buf[buf_index] = '\0';
                        X = atoi(buf);
                        buf_index = 0;
                    }
                }else if (c == '\n'){
                    if(buf_index > 0){
                        buf[buf_index] = '\0';
                        Y = atoi(buf);
                        buf_index = 0;
                    }
                }else{
                    if (buf_index < sizeof(buf) - 1){
                        buf[buf_index++] = c;
                    }
                }       
            }
        }
        float rad = 90.0 - (atan2(Y, X) * 180.0f / M_PI);
        if (rad < 0){
            rad += 360.0;
        }
        float speed = hypot(X, Y);
        pwm[0] = sin((rad - 45) * M_PI / 180.0f) * speed * power;
        pwm[1] = sin((rad - 135) * M_PI / 180.0f) * speed * power;
        pwm[2] = sin((rad - 225) * M_PI / 180.0f) * speed * power;
        pwm[3] = sin((rad - 315) * M_PI / 180.0f) * speed * power;
        CANMessage msg(2, (const uint8_t *)pwm, 8); //特に理由がない限りwhile直下
        can.write(msg); //特に理由がない限りwhile直下
        // printf("%d %d\n", pwm[3],pwm[0]);
        // printf("%d %d\n", pwm[2],pwm[1]);
        // printf("\n");
        // ThisThread::sleep_for(chrono::milliseconds(100));
    }
}