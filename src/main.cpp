#include "mbed.h"
BufferedSerial pc(USBTX, USBRX, 115200);
BufferedSerial esp(PA_9, PA_10, 115200);
int16_t pwm[4] = {0, 0, 0, 0};
int rx_X = 0;
int rx_Y = 0;
int rx_Turn = 0;
int read_num;
const int Max_Motor_Power = 22500;
uint8_t Motor_adj;
int R_adj; // 旋回の補正値

int main(){
    CAN can(PA_11, PA_12, (int)1e6); // canを出力するピンを指定
    CANMessage msg;// 変数「msg」の作成
    char buf[64];
    int buf_index = 0;
    Motor_adj = Max_Motor_Power / 150; // モーター出力の補正値:150
    R_adj = Max_Motor_Power / 127; // 旋回の補正値:177

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

                        // 値の大きさによってX,Y,旋回量に格納される
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

        // 進む方向の計算
        float rad = 90.0f - (atan2(rx_Y, rx_X) * 180.0f / M_PI);
        if (rad < 0) rad += 360.0;
        
        // スピードの計算と調整
        float speed = hypot(rx_X, rx_Y);
        if (abs(rx_X) <= 10 && abs(rx_Y) <= 10) speed = 0;
        if (abs(rx_Turn) <= 10) rx_Turn = 0;

        // pwmにそれぞれ格納
        for (int i : pwm){
            pwm[i] = (int)((sin((rad - (45 + i * 90)) * M_PI / 180.0f) * speed * Motor_adj) + rx_Turn) % (Max_Motor_Power + 1);
        }

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