#include "dev_item.h"

struct i2c_config {
	long scl_pin_select;
	long sda_pin_select;
	long i2c_speed;
	long pullmode;
	unsigned char i2c_address_10_or_7_bits;
	unsigned short slave_addres1;
	unsigned short slave_addres2;
};

#define BOX DEV_BOX
#define EMPTY DEV_EMPTY
#define CHOICE DEV_CHOICE
#define MENU DEV_MENU
#define ITEM DEV_ITEM
#define BOX_END DEV_BOX_END
#define MENU_END DEV_MENU_END
#define CHOICE_END DEV_CHOICE_END

struct dev_item i2c2_item[] = {
BOX("i2c1", "i2c1 board configuration\n"),

 CHOICE("scl_pin", "choice the i2c scl pin"),
  EMPTY("gpio pb06", "pb06 AF04 to i2c1_scl"),
  EMPTY("gpio pb08", "pb08 AF04 to i2c1_scl"),
 CHOICE_END(),

 CHOICE("sda_pin", "choice the i2c sda pin"),
  EMPTY("gpio pb07", "pb07 AF04 to i2c1_sda"),
  EMPTY("gpio pb09", "pb09 AF04 to i2c1_sda"),
 CHOICE_END(),

 CHOICE("i2c speed KHZ", "choice the i2c transfer speed(scl clk frequence) in KHZ"),
  EMPTY("400 KHZ", "i2c normal transfer mode"),
  EMPTY("100 KHZ", "i2c slow transfer mode"),
  ITEM("set a i2c speed", uint, 400, "i2c normal transfer mode"),
 CHOICE_END(),

 CHOICE("gpio inner pull mode", "choice the pin pull mode for scl and sda"),
  EMPTY("not inner pull up", "not inner pull up for the scl and sda pin"),
  EMPTY("inner pull up", "inner pull up for the scl and sda pin"),
 CHOICE_END(),

 CHOICE("i2c address length", "choice the i2c address length"),
  EMPTY("7 bits address", "the normal i2c address bit length"),
  EMPTY("10 bits address", "10 bits length "),
 CHOICE_END(),

 CHOICE("i2c slave mode address",
        "choice the i2c slave mode address mode, and set the address\n"
        "if 7 bits mode the slave address is range from 0 to 127, contains 0 and 127\n"
        "if 10 bits mode the slave address is range from 0 to 1023, contains 0 and 1023\n"),
  ITEM("single slave address", uint, 0x24, "slave address in single slave address mode"),
  BOX("dual slave address",
      "dual slave address if you have the speical needs\n"
      "you'd better do not let the two addrss have the same value"),
   ITEM("slave address 1", uint, 0x24, "the first slave address in dual slave address mode"),
   ITEM("slave address 2", uint, 0x25, "the second slave address in dual slave address mode"),
  BOX_END(),
 CHOICE_END(),

BOX_END(),
};

struct dev_config i2c2_dev_config = {
	.items = i2c2_item,
	.item_len = ARRAY_SIZE(i2c2_item),
};

void i2c2_dev_config_add(void)
{
	int ret;

	ret = dev_config_check_item_tree_valid(&i2c2_dev_config);

	printf ("i2c config is ok\n");
}
