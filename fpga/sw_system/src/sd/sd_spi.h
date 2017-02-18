
#ifndef SW_SYSTEM_SD_SPI_H
#define SW_SYSTEM_SD_SPI_H

/* Commands */
typedef enum {
    SD_SPI_CMD0_GO_IDLE_STATE = 0,
    SD_SPI_CMD1_SEND_OP_COND
} t_sd_spi_cmd;

/* Function prototypes */
void sd_spi_init(void);
void sd_spi_send_command (t_sd_spi_cmd cmd);

#endif //SW_SYSTEM_SD_SPI_H
