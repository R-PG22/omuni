#include "mbed.h"
#include <map>
BufferedSerial pc(USBTX, USBRX, 115200);
BufferedSerial esp(PA_9, PA_10, 115200);
int16_t wheel[4] = {0, 0, 0, 0};
int16_t other[4] = {0, 0, 0, 0};
constexpr int Max_Motor_Power = 20000;
constexpr int Motor_adj = Max_Motor_Power / 150; // 出力の補正値:150
constexpr int R_adj = Max_Motor_Power / 127; // 旋回の補正値:177
int rx_X = 0;
int rx_Y = 0;
int read_num;
int c_int;
int Turn = 0;
int is_turn = 0;


std::map<int, int> pwm_datas = {
    {107, 5000}, // r_up
    {108, 20000}, // r_
    {109, 10000},
    {110, 20000},
    {111, -20000}, // r_down
    {112, -20000},
    {113, -10000},
    {114, -20000}
};
 
int main(){
    CAN can(PA_11, PA_12, (int)1e6); // canを出力するピンを指定
    CAN can2(PB_12, PB_13, (int)1e6); // canを出力するピンを指定
    CANMessage msg; // 変数「msg」の作成
    CANMessage msg2; // 変数「msg」の作成

    char buf[64];
    int buf_index = 0;
    while(1){
        if(esp.readable()){
            char c;
            int len = esp.read(&c, 1);
            // printf("%c",c);
            if (len > 0){
                if (c == '\n'){
                    // printf("%s",buf);
                    if(buf_index > 0){
                        buf[buf_index] = '\0';
                        buf_index = 0;
                        read_num = atoi(buf);
                        // printf("%d",read_num);

                        if (read_num == 1001){
                            is_turn = 1;
                        }else if (read_num == 1000){
                            is_turn = -1;
                        }else {
                            is_turn = 0;
                        }

                        // 値の大きさによってX,Y,旋回量に格納される
                        if (400 > read_num && read_num > 127){
                            rx_Y = read_num - 256;
                            // printf("y");
                        }else if(128 > read_num && read_num > -128){
                            rx_X = read_num;
                            // printf("x");
                        }
                    }
                }else if (isalpha(c)){
                    // オムニ以外のモーターの出力
                    c_int = (int)c;
                    other[(c_int - 107) % 4] = pwm_datas[c_int];
                }else{
                    // ボタンを押してないときのリセット
                    for (int i = 0; i < 4; i++){
                        other[i] = 0;
                    }

                    if (buf_index < sizeof(buf) - 1){
                        buf[buf_index++] = c;
                    }
                }
            }
        }

        // 進む方向の計算
        // 真上を0°として時計回りに359°までの計算
        float rad = 90.0f - (atan2(rx_Y, rx_X) * 180.0f / M_PI);
        if (rad < 0) rad += 360.0f;
        
        // スピードの計算と調整
        float speed = hypot(rx_X, rx_Y);
        if (abs(rx_X) <= 10 && abs(rx_Y) <= 10) speed = 0;

        // pwmにそれぞれ格納
        if (is_turn == 0){
            for (int i = 0; i < 4; i++){
                wheel[i] = min((int)((sin((rad - (45 + i * 90)) * M_PI / 180.0f) * speed * Motor_adj)), Max_Motor_Power);
            }
        }else {
            for (int i = 0; i < 4; i++){
                wheel[i] = 20000 * is_turn;
            }
        }

        CANMessage msg(2, (const uint8_t *)wheel, 8); //特に理由がない限りwhile直下
        CANMessage msg2(4, (const uint8_t *)other, 8); //特に理由がない限りwhile直下
        can.write(msg); //特に理由がない限りwhile直下
        can.write(msg2); //特に理由がない限りwhile直下
        
        printf("%d %d\n", wheel[3],wheel[0]);
        printf("%d %d\n", wheel[2],wheel[1]);
        // printf("%d,%d",rx_X,rx_Y);
        
        printf("\n");
    }
    return 0;
}