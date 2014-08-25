#ifndef __I2C_H
#define __I2C_H 1

#include <stm32f10x.h>

#define I2Cx_COUNT 2

enum I2C_Status_Flags {
    I2C_ENABLED = 1,
    I2C_ISOPEN = 8,
};

/** Initialize one of the i2c hardware pariphrial drivers   **
 ** Arguments:                                              **
 **  i2c_no  - The zero bassed ID of the i2c peripheral     **
 **  *conf   - The I2C_InitType used for configuration.     **
 **                                                         **
 ** Returns:                                                **
 **  If success, returns the i2c_no                         **
 **  Else, returns -1 or less.                              **/
int i2c_Install(uint32_t i2c_no, I2C_InitTypeDef *conf);


/** Write to a I2C device register on the chain             **
 ** Arguments:                                              **
 **  i2cx    - The zero bassed ID of the i2c peripheral     **
 **  slave   - The i2c slave address.                       **
 **  buffer  - The value to write to the register           **
 **  addr    - The register address to write to.            **
 **                                                         **
 ** Returns:                                                **
 **  None                                                   **/
void i2c_WriteByte(int i2cx, uint8_t slave, uint8_t buffer, uint8_t addr);

/** Read from a I2C device register on the chain            **
 ** Arguments:                                              **
 **  i2cx    - The zero bassed ID of the i2c peripheral     **
 **  slave   - The i2c slave address.                       **
 **  *buffer - A pointer to a buffer to store the results   **
 **  addr    - The register address to read from.           **
 **                                                         **
 ** Returns:                                                **
 **  None                                                   **/
void i2c_BufferedRead(int i2cx, uint8_t slave, uint8_t *buffer, uint8_t addr, int count);
#endif /** __I2C_H **/
