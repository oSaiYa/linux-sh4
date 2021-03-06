/*
 * arch/sh/boards/mach-sat7111/setup.c
 *
 * Copyright (C) 2011 STMicroelectronics Limited
 * Author: Jon Frosdick (jon.frosdick@st.com)
 *
 * May be copied or modified under the terms of the GNU General Public
 * License.  See linux/COPYING for more information.
 *
 * STMicroelectronics sat7111 board support.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/leds.h>
#include <linux/delay.h>
#include <linux/phy.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/stm/platform.h>
#include <linux/stm/stx7111.h>
#include <linux/stm/emi.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/physmap.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <asm/irq-ilc.h>
#include <asm/irl.h>
#include <sound/stm.h>

/*****     2012-03-31     *****/
//YWDRIVER_MODI modify by lf for phy reset start
#if 1
#define SAT7111_PHY_RESET stm_gpio(5, 3)
#else
#define SAT7111_PHY_RESET stm_gpio(2, 4)
#endif
//YWDRIVER_MODI modify by lf end

/*****     2012-03-31     *****/
//YWDRIVER_MODI add by lf for flash protect start
#define SAT7111_SET_VPP stm_gpio(3, 4)
//YWDRIVER_MODI add by lf end

/* The sat7111 board is populated with NOR, NAND, and Serial Flash.  The setup
 * below assumes the board is in its default boot-from-NOR configuration.  Other
 * boot configurations are possible but require board-level modifications to be
 * made, and equivalent changes to the setup here.  Note, only boot-from-NOR has
 * been fully tested.
 */

static void __init sat7111_setup(char** cmdline_p)
{
	printk(KERN_INFO "STMicroelectronics sat7111 reference board initialisation\n");

	stx7111_early_device_init();

	stx7111_configure_asc(2, &(struct stx7111_asc_config) {
			.hw_flow_control = 1,
			.is_console = 1, });
	stx7111_configure_asc(3, &(struct stx7111_asc_config) {
			.hw_flow_control = 1,
			.is_console = 0, });
}



static struct platform_device sat7111_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev.platform_data = &(struct gpio_led_platform_data) {
		.num_leds = 2,
		.leds = (struct gpio_led[]) {
			{
				.name = "HB red",
				.default_trigger = "heartbeat",
				.gpio = stm_gpio(3, 5),
			}, {
				.name = "HB white",
				.gpio = stm_gpio(3, 0),
			},
		},
	},
};



static struct gpio_keys_button sat7111_buttons[] = {
	{
		.code = BTN_0,
		.gpio = stm_gpio(6, 2),
		.desc = "SW2",
	}, {
		.code = BTN_1,
		.gpio = stm_gpio(6, 3),
		.desc = "SW3",
	}, {
		.code = BTN_2,
		.gpio = stm_gpio(6, 4),
		.desc = "SW4",
	}, {
		.code = BTN_3,
		.gpio = stm_gpio(6, 5),
		.desc = "SW5",
	},
};

static struct platform_device sat7111_button_device = {
	.name = "gpio-keys",
	.id = -1,
	.num_resources = 0,
	.dev.platform_data = &(struct gpio_keys_platform_data) {
		.buttons = sat7111_buttons,
		.nbuttons = ARRAY_SIZE(sat7111_buttons),
	},
};
//YWDRIVER_MODI lwj modify start
#if 0
static struct platform_device sat7111_nor_flash = {
	.name		= "physmap-flash",
	.id		= -1,
	.num_resources	= 1,
	.resource	= (struct resource[]) {
		STM_PLAT_RESOURCE_MEM(0, 32*1024*1024),
	},
	.dev.platform_data = &(struct physmap_flash_data) {
		.width		= 2,
		.nr_parts	= 3,
		.parts		=  (struct mtd_partition []) {
			{
				.name = "NOR Flash 1",
				.size = 0x00080000,
				.offset = 0x00000000,
			}, {
				.name = "NOR Flash 2",
				.size = 0x00200000,
				.offset = MTDPART_OFS_NXTBLK,
			}, {
				.name = "NOR Flash 3",
				.size = MTDPART_SIZ_FULL,
				.offset = MTDPART_OFS_NXTBLK,
			},
		},


	},
};
#else

static void set_vpp(struct map_info * info, int enable)
{
	gpio_set_value(SAT7111_SET_VPP, enable);
}

static struct platform_device sat7111_nor_flash = {
	.name		= "physmap-flash",
	.id		= -1,
	.num_resources	= 1,
	.resource	= (struct resource[]) {
		STM_PLAT_RESOURCE_MEM(0, 8*1024*1024),
	},
	.dev.platform_data = &(struct physmap_flash_data) {
		.width		= 2,
		.set_vpp	= set_vpp,
		.nr_parts	= 3,
		.parts		=  (struct mtd_partition []) {
			{
				.name = "Boot firmware", //512K 384K+128K
				.size = 0x00080000,
				.offset = 0x00000000,
				//YWDRIVER_MODI for set mtdblock read-only	 begin
				.mask_flags = MTD_WRITEABLE, /* force read-only */
				//YWDRIVER_MODI for set mtdblock read-only	 begin
			}, {
				.name = "Kernel",  //7M
				.size = 0x00700000,
				.offset = 0x00080000,
				//YWDRIVER_MODI for set mtdblock read-only	 begin
				.mask_flags = MTD_WRITEABLE, /* force read-only */
				//YWDRIVER_MODI for set mtdblock read-only	 begin


			}, {
				.name = "Reserve",
				.size = MTDPART_SIZ_FULL,
				.offset = 0x00780000,
				//YWDRIVER_MODI for set mtdblock read-only	 begin
				//.mask_flags = MTD_WRITEABLE, /* force read-only */
				//YWDRIVER_MODI for set mtdblock read-only	 begin
			},
		},


	},
};

#endif
#if 1

static struct mtd_partition nand_partitions[] = {
	{
		.name	= "Spark kernel",
		.offset	= 0,
		.size 	= 0x00800000 //8M
	}, {
		.name	= "Spark Userfs",
		.offset	= 0x00800000,
		.size	= 0x17800000  //376M
	}, {
		.name	= "E2 kernel",
		.offset	= 0x18000000,
		.size	= 0x00800000 //8M
	}, {
		.name	= "E2 Userfs",
		.offset	= 0x18800000,
		.size	= 0x07700000 //119M	cc changed reserved 1024KB for u-boot bbt
	}
};




static struct plat_stmnand_data sat7111_nand_config = {
	.emi_bank		= 1,  //YWDRIVER_MODI lwj change 0 to 1
	.emi_withinbankoffset	= 0,

	/* Timings for NAND512W3A */
	.emi_timing_data = &(struct emi_timing_data) {
		.rd_cycle_time	= 40,		 /* times in ns */
		.rd_oee_start	= 0,
		.rd_oee_end	= 10,
		.rd_latchpoint	= 10,
		.busreleasetime = 0,

		.wr_cycle_time	= 40,
		.wr_oee_start	= 0,
		.wr_oee_end	= 10,

		.wait_active_low = 0,
	},

	.chip_delay		= 40,		/* time in us */
	.mtd_parts		= nand_partitions,
	.nr_parts		= ARRAY_SIZE(nand_partitions),
};

#else
//YWDRIVER_MODI lwj modify end

struct stm_nand_bank_data sat7111_nand_flash = {
	.csn		= 1,
	.options	= NAND_NO_AUTOINCR | NAND_USE_FLASH_BBT,
	.nr_partitions	= 2,
	.partitions	= (struct mtd_partition []) {
		{
			.name	= "NAND Flash 1",
			.offset	= 0,
			.size 	= 0x00800000
		}, {
			.name	= "NAND Flash 2",
			.offset = MTDPART_OFS_NXTBLK,
			.size	= MTDPART_SIZ_FULL
		},
	},
	.timing_data	= &(struct stm_nand_timing_data) {
		.sig_setup	= 50,		/* times in ns */
		.sig_hold	= 50,
		.CE_deassert	= 0,
		.WE_to_RBn	= 100,
		.wr_on		= 10,
		.wr_off		= 40,
		.rd_on		= 10,
		.rd_off		= 40,
		.chip_delay	= 30,		/* in us */
	},
};
#endif

/*****     2012-03-31     *****/
//YWDRIVER_MODI del by lf for no spi flash start
#if 0
/* Serial Flash */
static struct spi_board_info sat7111_serial_flash = {
	.modalias       = "m25p80",
	.bus_num        = 0,
	.chip_select    = stm_gpio(6, 7),
	.max_speed_hz   = 7000000,
	.mode           = SPI_MODE_3,
	.platform_data  = &(struct flash_platform_data) {
		.name = "m25p80",
		.type = "m25p16",
		.nr_parts	= 2,
		.parts = (struct mtd_partition []) {
			{
				.name = "Serial Flash 1",
				.size = 0x00080000,
				.offset = 0,
			}, {
				.name = "Serial Flash 2",
				.size = MTDPART_SIZ_FULL,
				.offset = MTDPART_OFS_NXTBLK,
			},
		},
	},
};
#endif
//YWDRIVER_MODI del by lf end

/*****     2012-03-31     *****/
//YWDRIVER_MODI modify by lf for more delay start
#if 1
static int sat7111_phy_reset(void *bus)
{
	gpio_set_value(SAT7111_PHY_RESET, 1);
	udelay(100);
	gpio_set_value(SAT7111_PHY_RESET, 0);
	mdelay(100);
	gpio_set_value(SAT7111_PHY_RESET, 1);
	return 1;
};
#else
static int sat7111_phy_reset(void *bus)
{
	gpio_set_value(SAT7111_PHY_RESET, 1);
	udelay(1);
	gpio_set_value(SAT7111_PHY_RESET, 0);
	udelay(1);
	gpio_set_value(SAT7111_PHY_RESET, 1);
	return 1;
};
#endif
//YWDRIVER_MODI modify by lf end

static struct stmmac_mdio_bus_data stmmac_mdio_bus = {
	.bus_id = 0,
	.phy_reset = &sat7111_phy_reset,
	.phy_mask = 0,
};

static struct platform_device *sat7111_devices[] __initdata = {
	&sat7111_leds,
	&sat7111_nor_flash,
	&sat7111_button_device,
};

static int __init sat7111_devices_init(void)
{
	int peripherals_i2c_bus;

	/*****     2012-03-31     *****/
	//YWDRIVER_MODI del by lf for no use start
	#if 0
	stx7111_configure_pwm(&(struct stx7111_pwm_config) {
			.out0_enabled = 1,
			.out1_enabled = 0, });
	#endif
	//YWDRIVER_MODI del by lf end

	/*****     2012-03-31     *****/
	//YWDRIVER_MODI del by lf for no spi start
	#if 0
	stx7111_configure_ssc_spi(0, NULL);
	#endif
	//YWDRIVER_MODI del by lf end
	stx7111_configure_ssc_i2c(1); /* J12=1-2, J16=1-2 */
	peripherals_i2c_bus = stx7111_configure_ssc_i2c(2);
	stx7111_configure_ssc_i2c(3);
	/*****     2012-03-31     *****/
	//YWDRIVER_MODI add by lf for add i2c start
	stx7111_configure_ssc_i2c(0);
	//YWDRIVER_MODI add by lf end

	stx7111_configure_usb(&(struct stx7111_usb_config) {
			.invert_ovrcur = 1, });

	stx7111_configure_ethernet(&(struct stx7111_ethernet_config) {
			.mode = stx7111_ethernet_mode_mii,
			.ext_clk = 0,
			.phy_bus = 0,
			/*****     2012-03-31     *****/
			//YWDRIVER_MODI modify by lf for change to 0 start
			#if 1
			.phy_addr = 0,
			#else
			.phy_addr = -1,
			#endif
			//YWDRIVER_MODI modify by lf end
			.mdio_bus_data = &stmmac_mdio_bus,});

	stx7111_configure_lirc(&(struct stx7111_lirc_config) {
			.rx_mode = stx7111_lirc_rx_mode_ir,
			.tx_enabled = 1,
			.tx_od_enabled = 0, });

	gpio_request(SAT7111_PHY_RESET, "PHY");

	/*****     2012-03-31     *****/
	//YWDRIVER_MODI modify by lf for change to 0 start
	#if 1
	gpio_direction_output(SAT7111_PHY_RESET, 0);
	#else
	gpio_direction_output(SAT7111_PHY_RESET, 1);
	#endif
	//YWDRIVER_MODI modify by lf end

	gpio_request(SAT7111_SET_VPP, "VPP");

	gpio_direction_output(SAT7111_SET_VPP, 0);

	//YWDRIVER_MODI jhy modify start
	#if 1
	stx7111_configure_nand(&sat7111_nand_config);
	#else
	stx7111_configure_nand(&(struct stm_nand_config) {
			.driver = stm_nand_flex,
			.nr_banks = 1,
			.banks = &sat7111_nand_flash,
			.rbn.flex_connected = 1,});
	#endif
	//YWDRIVER_MODI jhy modify end

	gpio_set_value(SAT7111_SET_VPP, 1);

	/*****     2012-03-31     *****/
	//YWDRIVER_MODI del by lf for no spi flash start
	#if 0
	spi_register_board_info(&sat7111_serial_flash, 1);
	#endif
	//YWDRIVER_MODI del by lf end

	return platform_add_devices(sat7111_devices,
				    ARRAY_SIZE(sat7111_devices));
}
arch_initcall(sat7111_devices_init);

static void __iomem *sat7111_ioport_map(unsigned long port, unsigned int size)
{
	/*
	 * If we have PCI then this should never be called because we
	 * are using the generic iomap implementation. If we don't
	 * have PCI then there are no IO mapped devices, so it still
	 * shouldn't be called.
	 */
	BUG();
	return (void __iomem *)CCN_PVR;
}

static void __init sat7111_init_irq(void)
{

}

struct sh_machine_vector mv_sat7111 __initmv = {
	.mv_name		= "sat7111",
	.mv_setup		= sat7111_setup,
	.mv_nr_irqs		= NR_IRQS,
	.mv_init_irq		= sat7111_init_irq,
	.mv_ioport_map		= sat7111_ioport_map,
};
