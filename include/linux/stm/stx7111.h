/*
 * (c) 2010 STMicroelectronics Limited
 *
 * Author: Pawel Moll <pawel.moll@st.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */



#ifndef __LINUX_STM_STX7111_H
#define __LINUX_STM_STX7111_H

#include <linux/device.h>
#include <linux/spi/spi.h>
#include <linux/stm/platform.h>


/* Sysconfig groups */

#define SYS_DEV 0
#define SYS_STA 1
#define SYS_CFG 2


void stx7111_early_device_init(void);


struct stx7111_asc_config {
	int hw_flow_control;
	int is_console;
};
void stx7111_configure_asc(int asc, struct stx7111_asc_config *config);


struct stx7111_ssc_spi_config {
	void (*chipselect)(struct spi_device *spi, int is_on);
};
/* SSC configure functions return I2C/SPI bus number */
int stx7111_configure_ssc_i2c(int ssc);
int stx7111_configure_ssc_spi(int ssc, struct stx7111_ssc_spi_config *config);


struct stx7111_lirc_config {
	enum {
		stx7111_lirc_rx_disabled,
		stx7111_lirc_rx_mode_ir,
		stx7111_lirc_rx_mode_uhf
	} rx_mode;
	int tx_enabled;
	int tx_od_enabled;
};
void stx7111_configure_lirc(struct stx7111_lirc_config *config);


struct stx7111_pwm_config {
	int out0_enabled;
	int out1_enabled;
};
void stx7111_configure_pwm(struct stx7111_pwm_config *config);


struct stx7111_ethernet_config {
	enum {
		stx7111_ethernet_mode_mii,
		stx7111_ethernet_mode_rmii,
		stx7111_ethernet_mode_reverse_mii,
	} mode;
	int ext_clk;
	int phy_bus;
	int phy_addr;
	struct stmmac_mdio_bus_data *mdio_bus_data;
};
void stx7111_configure_ethernet(struct stx7111_ethernet_config *config);


struct stx7111_usb_config {
	int invert_ovrcur;
};
void stx7111_configure_usb(struct stx7111_usb_config *config);
//YWDRIVER_MODI maple add begin

/* STM NAND configuration data (plat_nand/STM_NAND_EMI/FLEX/AFM) */
struct plat_stmnand_data {
	/* plat_nand/STM_NAND_EMI paramters */
	unsigned int	emi_withinbankoffset;  /* Offset within EMI Bank      */
	int		rbn_port;		/*  # : 'nand_RBn' PIO port   */
						/* -1 : if unconnected	      */
	int		rbn_pin;	        /*      'nand_RBn' PIO pin    */
						/* (assumes shared RBn signal */
						/*  for multiple chips)	      */

	/* STM_NAND_EMI/FLEX/AFM paramters */
	void		*timing_data;		/* Timings for EMI/NandC      */
	unsigned char	flex_rbn_connected;	/* RBn signal connected?      */
						/* (Required for NAND_AFM)    */

	/* Legacy data for backwards compatibility with plat_nand driver      */
	/*   will be removed once all platforms updated to use STM_NAND_EMI!  */
	unsigned int	emi_bank;		/* EMI bank                   */
	void		*emi_timing_data;	/* Timing data for EMI config */
	void		*mtd_parts;		/* MTD partition table	      */
	int		nr_parts;		/* Numer of partitions	      */
	unsigned int	chip_delay;		/* Read-busy delay	      */

};
void stx7111_configure_nand(struct plat_stmnand_data *data);


//YWDRIVER_MODI maple add end

//void stx7111_configure_nand(struct stm_nand_config *config);

void stx7111_configure_pci(struct stm_plat_pci_config *pci_config);
int  stx7111_pcibios_map_platform_irq(struct stm_plat_pci_config *pci_config,
                u8 pin);

#endif
