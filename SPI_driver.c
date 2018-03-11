/*
 * SPI_driver.c
 *
 *  Created on: Mar 10, 2018
 *      Author:
 */

#include "SPI_driver.h"
#include "fsl_dspi.h"
#include "fsl_port.h"

#define EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER kDSPI_MasterPcs0

volatile bool isTransferCompleted = false;
dspi_master_handle_t g_m_handle;
dspi_transfer_t masterXfer;

void DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle,
                             status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        __NOP();
    }

    isTransferCompleted = true;
}

void SPI_configuration()
{
    CLOCK_EnableClock(kCLOCK_PortD);
    CLOCK_EnableClock(kCLOCK_Spi0);
    port_pin_config_t config_spi = { kPORT_PullDisable, kPORT_SlowSlewRate,
        kPORT_PassiveFilterDisable, kPORT_OpenDrainDisable,
        kPORT_LowDriveStrength, kPORT_MuxAlt2, kPORT_UnlockRegister, };
    PORT_SetPinConfig(PORTD, 1, &config_spi);
    PORT_SetPinConfig(PORTD, 2, &config_spi);
    dspi_master_config_t masterConfig;
    DSPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
    masterConfig.ctarConfig.baudRate = 9600;
    DSPI_MasterInit(SPI0, &masterConfig, CLOCK_GetBusClkFreq());
    DSPI_SetFifoEnable(SPI0, false, false);
    DSPI_MasterTransferCreateHandle(SPI0, &g_m_handle, DSPI_MasterUserCallback,
                                    NULL);
    //DSPI_Enable(SPI0, true);
    //DSPI_EnableInterrupts(SPI0, kDSPI_AllInterruptEnable);
}

void SPI_sendOneByte(uint8 byte)
{
    isTransferCompleted = false;
    uint8_t masterTxData[1];
    masterTxData[0] = byte;
    masterXfer.txData = masterTxData;
    masterXfer.rxData = NULL;
    masterXfer.dataSize = sizeof(masterTxData);
    masterXfer.configFlags = kDSPI_MasterCtar0
            | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
    DSPI_MasterTransferNonBlocking(SPI0, &g_m_handle, &masterXfer);
    while (!isTransferCompleted)
    {
    }

}
