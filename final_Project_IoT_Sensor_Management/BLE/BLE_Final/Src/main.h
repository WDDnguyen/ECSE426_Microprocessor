#include <stdint.h>

#define DATA_SIZE     250
#define RX_ARR_SIZE   250
#define BLE_CHAR_SIZE 10
#define TIMEOUT       2000

extern uint8_t pData_unfilt_x1[RX_ARR_SIZE];
extern uint8_t pData_unfilt_x2[RX_ARR_SIZE];
extern uint8_t pData_unfilt_y1[RX_ARR_SIZE];
extern uint8_t pData_unfilt_y2[RX_ARR_SIZE];
extern uint8_t pData_unfilt_z1[RX_ARR_SIZE];
extern uint8_t pData_unfilt_z2[RX_ARR_SIZE];
extern uint8_t pData_filt[DATA_SIZE];

void t_disc(void);
void r_disc(void);
void t_disc_test(void);


