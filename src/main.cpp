#include "mbed.h"
BufferedSerial pc(USBTX, USBRX, 115200);
int16_t pwm[4] = {0, 0, 0, 0};
float X;
float Y;

int main(){
    CAN can(PA_11, PA_12, (int)1e6); // canを出力するピンを指定
    CANMessage msg;// 変数「msg」の作成
    char buf[64];
    int buf_index = 0;

    while(1){
        if(pc.readable()){
            char c;
            int len = pc.read(&c, 1);
            if (len > 0){
                if (c == ','){
                    if(buf_index > 0){
                        buf[buf_index] = '\0';
                        X = atoi(buf);
                        buf_index = 0;
                    }
                }else if (c == 'n'){
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
        float rad = atan2(Y, X);
        int speed = hypot(X, Y);
        pwm[0] = sin((rad - 45) * M_PI / 180.0f) * speed;
        pwm[1] = sin((rad - 135) * M_PI / 180.0f) * speed;
        pwm[2] = sin((rad - 225) * M_PI / 180.0f) * speed;
        pwm[3] = sin((rad - 315) * M_PI / 180.0f) * speed;
        CANMessage msg(2, (const uint8_t *)pwm, 8); //特に理由がない限りwhile直下
        can.write(msg); //特に理由がない限りwhile直下
    }
}