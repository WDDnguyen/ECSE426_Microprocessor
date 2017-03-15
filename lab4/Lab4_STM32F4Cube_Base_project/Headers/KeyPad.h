#define KeyPad_Port GPIOD
#define KeyPad_Column1 GPIO_PIN_1
#define KeyPad_Column2 GPIO_PIN_2
#define KeyPad_Column3 GPIO_PIN_3
#define KeyPad_Row1 GPIO_PIN_7
#define KeyPad_Row2 GPIO_PIN_8
#define KeyPad_Row3 GPIO_PIN_10
#define KeyPad_Row4 GPIO_PIN_11
#define KeyPad_GPIO_PORT_CLK_EN __HAL_RCC_GPIOD_CLK_ENABLE();

#define Key_0 0
#define Key_1 1
#define Key_2 2
#define Key_3 3
#define Key_4 4
#define Key_5 5
#define Key_6 6
#define Key_7 7
#define Key_8 8
#define Key_9 9
#define Key_star 10
#define Key_hash 11



void initializeKeyPad();
int KeyPadGetValue();