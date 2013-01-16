#include <common.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/mx5x_pins.h>
#include <asm/arch/iomux.h>
#include <asm/gpio.h>
#include <errno.h>
#include <linux/list.h>
#include <linux/fb.h>
#include <i2c.h>

#include <../drivers/video/ipu.h>

#define	EFIKASB_LVDS_I2C	0x3a
#define	EFIKAMX_HDMI_I2C	0x39

uint8_t lcd_init_code[] = {
	0x00, 0x20, 0xAF, 0x59, 0x2B, 0xDE, 0x51, 0x00,
	0x00, 0x04, 0x17, 0x00, 0x58, 0x02, 0x00, 0x00,
	0x00, 0x3B, 0x01, 0x08, 0x00, 0x1E, 0x01, 0x05,
	0x00, 0x01, 0x72, 0x05, 0x32, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x20, 0xA8, 0x02, 0x12, 0x00, 0x58,
	0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x02, 0x00,
	0x00, 0x02, 0x10, 0x01, 0x68, 0x03, 0xC2, 0x01,
	0x4A, 0x03, 0x46, 0x00, 0xF1, 0x01, 0x5C, 0x04,
	0x08, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x3A,
	0x18, 0x4B, 0x29, 0x5C, 0xDE, 0xF6, 0xE0, 0x1C,
	0x03, 0xFC, 0xE3, 0x1F, 0xF3, 0x75, 0x26, 0x45,
	0x4A, 0x91, 0x8A, 0xFF, 0x3F, 0x83, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x4E, 0x48,
	0x00, 0x01, 0x10, 0x01, 0x00, 0x00, 0x10, 0x04,
	0x02, 0x1F, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00,
	0x32, 0x00, 0x00, 0x04, 0x12, 0x00, 0x58, 0x02,
	0x02, 0x7C, 0x04, 0x98, 0x02, 0x11, 0x78, 0x18,
	0x30, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

struct fb_videomode mx51_efika_mode = {
	.name		= "1024x600",
	.refresh	= 60,
	.xres		= 1024,
	.yres		= 600,
	.pixclock	= 22800,
	.left_margin	= 80,
	.right_margin	= 40,
	.upper_margin	= 21,
	.lower_margin	= 21,
	.hsync_len	= 4,
	.vsync_len	= 4,
	.sync		= 0x100,	/* Active Low */
	.vmode		= FB_VMODE_NONINTERLACED,
	0,
};

struct fb_videomode mx51_efika_hd_modeX = {
	.name		= "1280x720",
	.refresh	= 60,
	.xres		= 1280,
	.yres		= 720,
	.pixclock	= 76230,
	.left_margin	= 110,
	.right_margin	= 220,
	.upper_margin	= 25,
	.lower_margin	= 20,
	.hsync_len	= 40,
	.vsync_len	= 5,
	.sync		= 0x100,	/* Active Low */
	.vmode		= FB_VMODE_NONINTERLACED,
	0,
};

static struct fb_videomode mx51_efikamx_default = {
	.name		= "640x480-60",
	.refresh	= 60,
	.xres		= 640,	/* Width */
	.yres		= 480,	/* Height */
	.pixclock	= 39721, /* pS 60 fps */
	.left_margin	= 48,
	.right_margin	= 16,
	.upper_margin	= 33,
	.lower_margin	= 10,
	.hsync_len	= 96,
	.vsync_len	= 2,
	/* Htotal 800 */
	/* Vtotal 525 */

	.sync		= FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT,
	.vmode		= FB_VMODE_NONINTERLACED
};

void setup_iomux_lcd(void)
{
	if (machine_is_efikasb()) {
		/* DISP RST */
		mxc_request_iomux(MX51_PIN_DISPB2_SER_DIN, IOMUX_CONFIG_GPIO);
		mxc_iomux_set_pad(MX51_PIN_DISPB2_SER_DIN, PAD_CTL_SRE_FAST |
					PAD_CTL_DRV_HIGH | PAD_CTL_PKE_ENABLE);

		/* DISP2 EN# */
		mxc_request_iomux(MX51_PIN_DISPB2_SER_CLK, IOMUX_CONFIG_GPIO);
		mxc_iomux_set_pad(MX51_PIN_DISPB2_SER_CLK, PAD_CTL_SRE_FAST |
					PAD_CTL_DRV_HIGH | PAD_CTL_PKE_ENABLE);
		/* LCD_PWRON */
		mxc_request_iomux(MX51_PIN_CSI1_D9, IOMUX_CONFIG_GPIO);
		mxc_iomux_set_pad(MX51_PIN_CSI1_D9, PAD_CTL_SRE_FAST |
					PAD_CTL_DRV_HIGH | PAD_CTL_PKE_ENABLE);
		/* LCD_PWRON_R */
		mxc_request_iomux(MX51_PIN_CSI1_D8, IOMUX_CONFIG_GPIO);
		mxc_iomux_set_pad(MX51_PIN_CSI1_D8, PAD_CTL_SRE_FAST |
					PAD_CTL_DRV_HIGH | PAD_CTL_PKE_ENABLE);
		/* LCD_BL_PWRON# */
		mxc_request_iomux(MX51_PIN_CSI2_D19, IOMUX_CONFIG_GPIO);
		mxc_iomux_set_pad(MX51_PIN_CSI2_D19, PAD_CTL_SRE_FAST |
					PAD_CTL_DRV_HIGH | PAD_CTL_PKE_ENABLE);
		/* DISP1_EN# */
		mxc_request_iomux(MX51_PIN_DI1_D1_CS, IOMUX_CONFIG_GPIO);
		mxc_iomux_set_pad(MX51_PIN_DI1_D1_CS, PAD_CTL_SRE_FAST |
					PAD_CTL_DRV_HIGH | PAD_CTL_PKE_ENABLE);

		mxc_iomux_set_pad(MX51_PIN_DI2_DISP_CLK, PAD_CTL_SRE_FAST |
					PAD_CTL_DRV_LOW | PAD_CTL_PKE_ENABLE);
	} else {
		/* HDMI Enable */
		mxc_request_iomux(MX51_PIN_DI1_D1_CS, IOMUX_CONFIG_GPIO);
		mxc_iomux_set_pad(MX51_PIN_DI1_D1_CS, PAD_CTL_SRE_FAST |
					PAD_CTL_DRV_HIGH | PAD_CTL_PKE_ENABLE);

		/* VGA Enable */
		mxc_request_iomux(MX51_PIN_DISPB2_SER_CLK, IOMUX_CONFIG_GPIO);
		mxc_iomux_set_pad(MX51_PIN_DISPB2_SER_CLK, PAD_CTL_SRE_FAST |
					PAD_CTL_DRV_HIGH | PAD_CTL_PKE_ENABLE);

		/* HDMI Reset */
		mxc_request_iomux(MX51_PIN_DISPB2_SER_DIN, IOMUX_CONFIG_GPIO);
		mxc_iomux_set_pad(MX51_PIN_DISPB2_SER_DIN, PAD_CTL_SRE_FAST |
					PAD_CTL_DRV_HIGH | PAD_CTL_PKE_ENABLE);

		/* DISP_EN# and DISP2_EN# go to a level shifter which we need to turn on */
		gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DI1_D1_CS), 0);
		gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DISPB2_SER_CLK), 1);
	}
}

void setup_efikasb_lcd(void)
{
	int ret = 10;
	const int size = sizeof(lcd_init_code);

	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DISPB2_SER_DIN), 0);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DISPB2_SER_CLK), 0);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_CSI1_D9), 0);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_CSI1_D8), 0);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DI1_D1_CS), 1);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DI1_D1_CS), 0);//
	udelay(10000);

	/* Reset the LCD */
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI1_D9), 1);
	udelay(10000);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI1_D8), 1);
	udelay(5000);
	gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_DISPB2_SER_CLK), 1);
	udelay(5000);

	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_CSI2_D19), 0);
	udelay(10000);

	/* Program the LCD init code */
	while (--ret && i2c_write(EFIKASB_LVDS_I2C, 0, 1, lcd_init_code, size))
		;

	if (!ret)
		printf("EfikaSB LCD: I2C TX error\n");
}

#define	SIIHDMI_RETRIES	20

#define SIIHDMI_TPI_REG_PIXEL_CLOCK_LSB			(0x00)
#define SIIHDMI_TPI_REG_PIXEL_CLOCK_MSB			(0x01)
#define SIIHDMI_TPI_REG_VFREQ_LSB			(0x02)
#define SIIHDMI_TPI_REG_VFREQ_MSB			(0x03)
#define SIIHDMI_TPI_REG_PIXELS_LSB			(0x04)
#define SIIHDMI_TPI_REG_PIXELS_MSB			(0x05)
#define SIIHDMI_TPI_REG_LINES_LSB			(0x06)
#define SIIHDMI_TPI_REG_LINES_MSB			(0x07)
#define SIIHDMI_TPI_REG_INPUT_BUS_PIXEL_REPETITION	(0x08)
#define SIIHDMI_TPI_REG_AVI_INPUT_FORMAT		(0x09)
#define SIIHDMI_TPI_REG_AVI_OUTPUT_FORMAT		(0x0a)
#define SIIHDMI_TPI_REG_YC_INPUT_MODE			(0x0b)


/* AVI infoframe */
#define SIIHDMI_TPI_REG_AVI_DBYTE0			(0x0c)
#define SIIHDMI_TPI_REG_AVI_DBYTE1			(0x0d)
#define SIIHDMI_TPI_REG_AVI_DBYTE2			(0x0e)
#define SIIHDMI_TPI_REG_AVI_DBYTE3			(0x0f)
#define SIIHDMI_TPI_REG_AVI_DBYTE4			(0x10)
#define SIIHDMI_TPI_REG_AVI_DBYTE5			(0x11)
#define SIIHDMI_TPI_REG_AVI_INFO_END_TOP_BAR_LSB	(0x12)
#define SIIHDMI_TPI_REG_AVI_INFO_END_TOP_BAR_MSB	(0x13)
#define SIIHDMI_TPI_REG_AVI_INFO_START_BOTTOM_BAR_LSB	(0x14)
#define SIIHDMI_TPI_REG_AVI_INFO_START_BOTTOM_BAR_MSB	(0x15)
#define SIIHDMI_TPI_REG_AVI_INFO_END_LEFT_BAR_LSB	(0x16)
#define SIIHDMI_TPI_REG_AVI_INFO_END_LEFT_BAR_MSB	(0x17)
#define SIIHDMI_TPI_REG_AVI_INFO_END_RIGHT_BAR_LSB	(0x18)
#define SIIHDMI_TPI_REG_AVI_INFO_END_RIGHT_BAR_MSB	(0x19)


/* Output Format */
#define SIIHDMI_OUTPUT_FORMAT_HDMI_RGB			(0 << 0)
#define SIIHDMI_OUTPUT_FORMAT_HDMI_YUV_444		(1 << 0)
#define SIIHDMI_OUTPUT_FORMAT_HDMI_YUV_422		(2 << 0)
#define SIIHDMI_OUTPUT_FORMAT_DVI_RGB			(3 << 0)
#define SIIHDMI_OUTPUT_VIDEO_RANGE_COMPRESSION_AUTO	(0 << 2)
#define SIIHDMI_OUTPUT_VIDEO_RANGE_COMPRESSION_ON	(1 << 2)
#define SIIHDMI_OUTPUT_VIDEO_RANGE_COMPRESSION_OFF	(2 << 2)
#define SIIHDMI_OUTPUT_COLOR_STANDARD_BT601		(0 << 4)
#define SIIHDMI_OUTPUT_COLOR_STANDARD_BT709		(1 << 4)
#define SIIHDMI_OUTPUT_DITHERING			(1 << 5)
#define SIIHDMI_OUTPUT_COLOR_DEPTH_8BIT			(0 << 6)
#define SIIHDMI_OUTPUT_COLOR_DEPTH_10BIT		(2 << 6)
#define SIIHDMI_OUTPUT_COLOR_DEPTH_12BIT		(3 << 6)
#define SIIHDMI_OUTPUT_COLOR_DEPTH_16BIT		(1 << 6)



#define	SIIHDMI_TPI_REG_SYS_CTRL				0x1a
#define	SIIHDMI_TPI_REG_DEVICE_ID				0x1b
#define	SIIHDMI_TPI_REG_PWR_STATE				0x1e
#define	SIIHDMI_TPI_REG_IER					0x3c
#define	SIIHDMI_TPI_REG_RQB					0xc7




#define	SIIHDMI_TPI_REG_SYS_CTRL_OUTPUT_MODE_SELECT_HDMI	(1 << 0)
#define	SIIHDMI_TPI_REG_SYS_CTRL_TMDS_OUTPUT_POWER_DOWN		(1 << 4)

#define	SIIHDMI_TPI_REG_DEVICE_ID_B0				0xb0

int siihdmi_read(uint8_t reg, uint8_t *val)
{
	int ret = SIIHDMI_RETRIES;

	while (--ret && i2c_read(EFIKAMX_HDMI_I2C, reg, 1, val, 1))
		udelay(1);

	if (!ret)
		printf("SIIHDMI: I2C Error\n");

	return !ret;
}

int siihdmi_write(uint8_t reg, uint8_t val)
{
	int ret = SIIHDMI_RETRIES;

	while (--ret && i2c_write(EFIKAMX_HDMI_I2C, reg, 1, &val, 1))
		udelay(1);

	if (!ret)
		printf("SIIHDMI: I2C Error\n");

	return !ret;
}

#if 1
#define HDMI_PACKET_TYPE_INFO_FRAME			(0x80)
#define HDMI_PACKET_CHECKSUM				(0x100)

/* AVI InfoFrame (v2) */
#define CEA861_AVI_INFO_FRAME_VERSION			(0x02)

/* InfoFrame type constants */
enum info_frame_type {
	INFO_FRAME_TYPE_RESERVED,
	INFO_FRAME_TYPE_VENDOR_SPECIFIC,
	INFO_FRAME_TYPE_AUXILIARY_VIDEO_INFO,
	INFO_FRAME_TYPE_SOURCE_PRODUCT_DESCRIPTION,
	INFO_FRAME_TYPE_AUDIO,
	INFO_FRAME_TYPE_MPEG,
};


enum scan_information {
	SCAN_INFORMATION_UNKNOWN,
	SCAN_INFORMATION_OVERSCANNED,
	SCAN_INFORMATION_UNDERSCANNED,
	SCAN_INFORMATION_RESERVED,
};

enum bar_info {
	BAR_INFO_INVALID,
	BAR_INFO_VERTICAL,
	BAR_INFO_HORIZONTAL,
	BAR_INFO_BOTH,
};

enum pixel_format {
	PIXEL_FORMAT_RGB,       /* default */
	PIXEL_FORMAT_YUV_422,
	PIXEL_FORMAT_YUV_444,
};

enum active_format_description {
	ACTIVE_FORMAT_DESCRIPTION_UNSCALED      = 0x08,
	ACTIVE_FORMAT_DESCRIPTION_4_3_CENTERED  = 0x09,
	ACTIVE_FORMAT_DESCRIPTION_16_9_CENTERED = 0x10,
	ACTIVE_FORMAT_DESCRIPTION_14_9_CENTERED = 0x11,
};

enum picture_aspect_ratio {
	PICTURE_ASPECT_RATIO_UNSCALED,
	PICTURE_ASPECT_RATIO_4_3,
	PICTURE_ASPECT_RATIO_16_9,
};

enum colorimetry {
	COLORIMETRY_UNKNOWN,
	COLORIMETRY_BT601,
	COLORIMETRY_BT709,
	COLORIMETRY_EXTENDED,
};

enum non_uniform_picture_scaling {
	NON_UNIFORM_PICTURE_SCALING_NONE,
	NON_UNIFORM_PICTURE_SCALING_HORIZONTAL,
	NON_UNIFORM_PICTURE_SCALING_VERTICAL,
	NON_UNIFORM_PICTURE_SCALING_BOTH,
};

/* quantization range are the same flag values for RGB and YCC */
enum quantization_range {
	QUANTIZATION_RANGE_LIMITED,
	QUANTIZATION_RANGE_FULL,
};

enum extended_colorimetry {
	EXTENDED_COLORIMETRY_BT601,
	EXTENDED_COLORIMETRY_BT709,
};

enum video_format {
	VIDEO_FORMAT_UNKNOWN,
};

struct __packed info_frame_header {
	u8 type;
	u8 version;
	u8 length;
	u8 chksum;
};

struct __packed avi_info_frame {
	struct info_frame_header header;

	unsigned scan_information            : 2;
	unsigned bar_info                    : 2;
	unsigned active_format_info_valid    : 1;
	unsigned pixel_format                : 2;
	unsigned dbyte1_reserved0            : 1;

	unsigned active_format_description   : 4;
	unsigned picture_aspect_ratio        : 2;
	unsigned colorimetry                 : 2;

	unsigned non_uniform_picture_scaling : 2;
	unsigned rgb_quantization_range      : 2;
	unsigned extended_colorimetry        : 3;
	unsigned it_content_present          : 1;

	unsigned video_format                : 7;
	unsigned dbyte4_reserved0            : 1;

	unsigned pixel_repetition_factor     : 4;       /* value - 1 */
	unsigned content_type                : 2;
	unsigned ycc_quantizaton_range       : 2;

	u16      end_of_top_bar;
	u16      start_of_bottom_bar;
	u16      end_of_left_bar;
	u16      start_of_right_bar;
} __packed;

static inline void cea861_checksum_hdmi_info_frame(u8 * const info_frame)
{
	struct info_frame_header * const header =
		(struct info_frame_header *) info_frame;

	int i;
	u8 crc;

	crc = (HDMI_PACKET_TYPE_INFO_FRAME + header->type) +
		header->version + (header->length - 1);

	for (i = 1; i < header->length; i++)
		crc += info_frame[i];

	header->chksum = HDMI_PACKET_CHECKSUM - crc;
}



enum {
	SINK_TYPE_DVI,
	SINK_TYPE_HDMI,
} type;

enum {
	SCANNING_EXACT,
	SCANNING_UNDERSCANNED,
	SCANNING_OVERSCANNED,
} scanning;

void
wr_avi_info_frame()
{
	struct avi_info_frame avi = {
		.header = {
			.type    = INFO_FRAME_TYPE_AUXILIARY_VIDEO_INFO,
			.version = CEA861_AVI_INFO_FRAME_VERSION,
			.length  = sizeof(avi) - sizeof(avi.header),
		},

		.active_format_info_valid  = 1,
		.active_format_description = ACTIVE_FORMAT_DESCRIPTION_UNSCALED,

		.picture_aspect_ratio      = PICTURE_ASPECT_RATIO_UNSCALED,

		.video_format              = 1,//XXX// vic,
	};

//	DBG("AVI InfoFrame sending Video Format %d\n", 0);//vic);

//	switch (tx->sink.scanning) {
	switch (SCANNING_UNDERSCANNED) {
	case SCANNING_UNDERSCANNED:
		avi.scan_information = SCAN_INFORMATION_UNDERSCANNED;
		break;
	case SCANNING_OVERSCANNED:
		avi.scan_information = SCAN_INFORMATION_OVERSCANNED;
		break;
	default:
		avi.scan_information = SCAN_INFORMATION_UNKNOWN;
		break;
	}

	cea861_checksum_hdmi_info_frame((u8 *) &avi);
	int i;
	unsigned char *a = &avi;

	for (i = 0; i < 13; i ++) {
		printf("reg[%02x] = %02x, ret=%d\n",
		    SIIHDMI_TPI_REG_AVI_DBYTE0 + i, a[i],
//		    SIIHDMI_TPI_REG_AVI_DBYTE0 + i, 0,
		    siihdmi_write(SIIHDMI_TPI_REG_AVI_DBYTE0 + i, a[i+3]));
//		    siihdmi_write(SIIHDMI_TPI_REG_AVI_DBYTE0 + i, 0));
	}
}

#endif

void siihdmi_reset(void)
{
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DI1_D1_CS), 1);
	udelay(1000);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DI1_D1_CS), 0);
	udelay(1000);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DISPB2_SER_CLK), 1);

	/* Reset SII9022, 1 mS for reset sequence, 10 mS to leave reset */
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DISPB2_SER_DIN), 1);
	udelay(1000);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DISPB2_SER_DIN), 0);
	udelay(10000);
}

int siihdmi_detect_rev(void)
{
	int ret = 0, try;
	uint8_t val;

	ret = siihdmi_write(SIIHDMI_TPI_REG_RQB, 0x0);
	if (ret)
		return ret;

	try = SIIHDMI_RETRIES;
	while (--try) {
		ret = siihdmi_read(SIIHDMI_TPI_REG_DEVICE_ID, &val);
		if (ret)
			goto exit;

		if (val == SIIHDMI_TPI_REG_DEVICE_ID_B0)
			break;
	}

	printf("SII9022 rev %02x\n", val);
	ret = !try;
	ret = 0;//

exit:
	return ret;
}

int siihdmi_init(void)
{
	int ret = 0;

	siihdmi_reset();

	ret = siihdmi_detect_rev();
	if (ret)
		goto exit;

	/* Power up the transceiver, state D0 */
	ret = siihdmi_write(SIIHDMI_TPI_REG_PWR_STATE, 0x0);
	if (ret)
		goto exit;

	ret = siihdmi_write(SIIHDMI_TPI_REG_IER, 0x3);
	if (ret)
		goto exit;

	return 0;

exit:
	return ret;
}

int siihdmi_sink_present(void)
{
	int ret;
	uint8_t val;

	ret = siihdmi_read(SIIHDMI_TPI_REG_IER, &val);
	if (ret)
		return -1;

	/* IRQ Happened */
//	if (val & 0x3)
		return 1;

	/* No IRQ */
	return 0;
}

int siihdmi_set_res(struct fb_videomode *mode)
{
	int ret, try;
	uint8_t ctrl;
	uint32_t pixclk = 1000000000UL / (10 * mode->pixclock);
	uint32_t htotal = mode->xres + mode->left_margin +
				mode->hsync_len + mode->right_margin;
	uint32_t vtotal = mode->yres + mode->upper_margin +
				mode->vsync_len + mode->lower_margin;
	uint32_t refresh = (pixclk * 100000UL) / (htotal * vtotal);
	uint16_t mode_packet[4] = { pixclk, htotal, vtotal, refresh };
	uint8_t empty[0xe] = {0};

	printf("%s:%d:\n", __func__, __LINE__);
	ret = siihdmi_read(SIIHDMI_TPI_REG_SYS_CTRL, &ctrl);
	if (ret)
		return ret;

	/* Set output to DVI and power down TMDS */
	ctrl &= ~SIIHDMI_TPI_REG_SYS_CTRL_OUTPUT_MODE_SELECT_HDMI;
	ctrl |= SIIHDMI_TPI_REG_SYS_CTRL_TMDS_OUTPUT_POWER_DOWN;
	ctrl = 0x11;

	printf("%s:%d:\n", __func__, __LINE__);
	ret = siihdmi_write(SIIHDMI_TPI_REG_SYS_CTRL, ctrl);
	if (ret)
		return ret;

	/* Wait frame drain time */
	udelay(1000);

	printf("%s:%d:\n", __func__, __LINE__);
	/* Send the mode data */
	try = SIIHDMI_RETRIES;
	while (--try) {
		int i;
		ret = 0;
		for (i = 0; i < sizeof(mode_packet); i ++) {
			ret += siihdmi_write(SIIHDMI_TPI_REG_PIXEL_CLOCK_LSB + i, ((uint8_t *)mode_packet)[i]);
		}
//		ret = i2c_write(EFIKAMX_HDMI_I2C,
//				SIIHDMI_TPI_REG_PIXEL_CLOCK_LSB, 1,
//				(uint8_t *)mode_packet, sizeof(mode_packet));
		if (!ret)
			break;

		udelay(1000);
	}

	if (!try)
		return 1;

	printf("%s:%d:\n", __func__, __LINE__);
	ret = siihdmi_write(SIIHDMI_TPI_REG_AVI_INPUT_FORMAT, 0);
	if (ret)
		return ret;

	printf("%s:%d:\n", __func__, __LINE__);
	ret = siihdmi_write(SIIHDMI_TPI_REG_AVI_OUTPUT_FORMAT, 0);
	if (ret)
		return ret;

	printf("%s:%d:\n", __func__, __LINE__);
	try = SIIHDMI_RETRIES;
	while (--try) {
		ret =  i2c_write(EFIKAMX_HDMI_I2C, SIIHDMI_TPI_REG_AVI_DBYTE0,
					1, empty, sizeof(empty));
		if (!ret)
			break;

		udelay(1000);
	}
	printf("%s:%d:\n", __func__, __LINE__);
	if (!try)
		return 1;

//	wr_avi_info_frame();

	ctrl |= SIIHDMI_TPI_REG_SYS_CTRL_OUTPUT_MODE_SELECT_HDMI;
	printf("%s:%d:\n", __func__, __LINE__);
	ret = siihdmi_write(SIIHDMI_TPI_REG_SYS_CTRL, ctrl);
	if (ret)
		return ret;

	ctrl &= ~SIIHDMI_TPI_REG_SYS_CTRL_TMDS_OUTPUT_POWER_DOWN;
	printf("%s:%d:\n", __func__, __LINE__);
	ret = siihdmi_write(SIIHDMI_TPI_REG_SYS_CTRL, ctrl);
	if (ret)
		return ret;

	printf("%s:%d: done\n", __func__, __LINE__);
	return 0;
}

void setup_efikamx_lcd(void)
{
	int ret;

	printf("%s:\n", __func__);

	ret = siihdmi_init();
	if (ret)
		goto exit;

	printf("%s:%d:\n", __func__, __LINE__);
	if (siihdmi_sink_present() != 1) {
		printf("SIIHDMI: No sink present, output not enabled\n");
		return;
	}

	printf("%s:%d:\n", __func__, __LINE__);
	if (siihdmi_set_res(&mx51_efikamx_default))
		goto exit;

	printf("%s:%d:\n", __func__, __LINE__);
	return;

exit:
	printf("SIIHDMI: Controller setup error, powering down\n");
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DISPB2_SER_CLK), 0);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_DI1_D1_CS), 1);
}

void setup_efika_lcd(void)
{
	if (machine_is_efikasb())
		setup_efikasb_lcd();
	else
		setup_efikamx_lcd();
}

extern struct mxc_ccm_reg *mxc_ccm;

void setup_efika_lcd_early(void)
{
	int ret = 0;
	u32 reg, addr;

	if (machine_is_efikasb())
		ret = ipuv3_fb_init(&mx51_efika_mode, 1, IPU_PIX_FMT_RGB565);
	else {
		printf("Efika MX Smartop LCD config\n");
		/* MCG Control Designation Register (MCD) 0x83FD_C000 0x0000_0F00 */
		/* MCG CCM Control Register (MCCMC)       0x83FD_C0D8 0x0000_000C */
		/* MXT Configuration Register (MXT_CONF)  0x83FD_C800 0xF003_008B */
		writel(0x00000F00, 0x83FDC000);
		writel(0x0000000C, 0x83FDC0D8);
		writel(0xF003008B, 0x83FDC800);
		writel(readl(0x73FD8000 + 0x0c) | 0x10, 0x73FD8000 + 0x0c); /* Powerup request to IPU */

		reg = __raw_readl(&mxc_ccm->ccdr);
		reg |= (1 << 18);
		__raw_writel(reg, &mxc_ccm->ccdr);

		setup_iomux_lcd();
		ret = ipuv3_fb_init(&mx51_efikamx_default, 0, IPU_PIX_FMT_RGB24);
		setup_efikamx_lcd();
	}

	if (ret)
		puts("LCD cannot be configured\n");
}
