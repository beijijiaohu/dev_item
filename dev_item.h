#ifndef _DEV_ITEM_H_
#define _DEV_ITEM_H_

#include "common.h"

enum dev_value_type {
	value_type_char,
	value_type_uchar,
	value_type_short,
	value_type_ushort,
	value_type_int,
	value_type_uint,
	value_type_llong,
	value_type_ullong,
	value_type_string,
	value_type_enum,

	value_type_nums,
};

struct dev_value_enum_item {
	const char *name;
	const char *help_info;
	long value;
};

struct dev_value_enum {
	struct dev_value_enum_item *enum_items;
	int value;
};

struct dev_value_box {
    int type;
	unsigned long long default_value;
	union {
		char v_char;
		unsigned char v_uchar;
		short v_short;
		unsigned short v_ushort;
		int v_int;
		unsigned int v_uint;
		long long v_llong;
		unsigned long long v_ullong;
		const char *v_string;
		struct dev_value_enum v_enum;
	} box;
};

enum dev_item_type {
	item_type_end = 0,
	item_type_item,
	item_type_empty,
	item_type_box,
	item_type_menu,
	item_type_choice,

	item_type_nums,
};

struct dev_item_item {
	struct dev_value_box value;
};

struct dev_item_menu {
    unsigned int reserve;
};

struct dev_item_choice {
    unsigned int index;
};

enum {
	item_status_by_user,
	item_status_force_select,
	item_status_force_unselect,

	item_status_nums,
};

/**
 * @breif device descripte item
 */
struct dev_item {
	unsigned char type;
	unsigned int is_selected:1;
    const char *name;
	const char *help_info;
	union {
		struct dev_item_item item;
		struct dev_item_menu menu;
		struct dev_item_choice choice;
	} value;
};

struct dev_item_config;

struct dev_item_ops {
	int (*item_value_is_valid)(struct dev_item_config, unsigned int index_of_item, struct dev_value_box *value);
};

struct dev_config {
    struct dev_item_ops item_ops;
	struct dev_item *items;
	unsigned int item_len;
};

#define DEV_END() \
{ \
    .type = item_type_end,\
}

#define DEV_MENU_END()   DEV_END()
#define DEV_BOX_END()    DEV_END()
#define DEV_CHOICE_END() DEV_END()

#define DEV_EMPTY(_name,  _help_info) \
{ \
    .type = item_type_empty,\
    .name = (_name),\
    .help_info = (_help_info),\
}

#define DEV_ITEM(_name, _type, _default_value, _help_info) \
{ \
    .type = item_type_item,\
    .name = (_name),\
    .help_info = (_help_info),\
    .value.item.value.type = value_type_##_type,\
    .value.item.value.default_value = (unsigned long long)(_default_value),\
}

#define DEV_ITEM_ENUM(_name, _default_value, _enum_items, _value_is_valid, _help_info) \
{ \
    .type = item_type_item,\
    .name = (_name),\
    .help_info = (_help_info),\
    .value.item.value.type = value_type_enum,\
    .value.item.value.default_value = (unsigned long long)(_default_value),\
    .value.item.value.enum_items = (_enum_items),\
}

#define DEV_BOX(_name, _help_info) \
{ \
    .type = item_type_box,\
    .name = (_name),\
    .help_info = (_help_info),\
}

#define DEV_MENU(_name, _help_info) \
{ \
    .type = item_type_menu,\
    .name = (_name),\
    .help_info = (_help_info),\
}

#define DEV_CHOICE(_name,  _help_info) \
{ \
    .type = item_type_choice,\
    .name = (_name),\
    .help_info = (_help_info),\
}

int dev_item_is_selected(struct dev_item *item)
{
	return item->is_selected;
}

int dev_item_get_type(struct dev_item *item)
{
	return item->type;
}

const char *dev_item_get_name(struct dev_item *item)
{
	return item->name;
}

const char *dev_item_get_help_info(struct dev_item *item)
{
	return item->help_info;
}

struct dev_value_box *dev_item_get_value(struct dev_item *item)
{
	return &item->value.item.value;
}

struct dev_item *dev_config_get_item(struct dev_config *config, unsigned int index)
{
	return config->items + index;
}

int dev_config_is_item_selected(struct dev_config *config, unsigned int index)
{
	return dev_item_is_selected(config->items + index);
}

int dev_config_get_item_type(struct dev_config *config, unsigned int index)
{
	return dev_item_get_type(config->items + index);
}

const char *dev_config_get_item_name(struct dev_config *config, unsigned int index)
{
	return dev_item_get_name(config->items + index);
}

const char *dev_config_get_item_help_info(struct dev_config *config, unsigned int index)
{
	return dev_item_get_help_info(config->items + index);
}

struct dev_value_box *dev_config_get_item_value(struct dev_config *config, unsigned int index)
{
	return dev_item_get_value(config->items + index);
}

/**
 * mostly it is a example to show you how to get each struct dev_item from a struct dev_config
 */
int for_each_dev_item(struct dev_config *config, int (*func)(struct dev_config *config, unsigned int index))
{
	int ret;
	unsigned int i;

	assert(config);
	assert(func);

	for (i = 0; i < config->item_len; ++i) {
		ret = func(config, i);
		if (ret)
			return ret;
	}

	return 0;
}

static inline dev_item_is_container(struct dev_item *item)
{
	return item->type == item_type_box ||
           item->type == item_type_menu ||
           item->type == item_type_choice;
}

static inline dev_item_is_end(struct dev_item *item)
{
	return item->type == item_type_end;
}

int dev_config_check_item_tree_valid(struct dev_config *config)
{
	int count = 0;
	struct dev_item *items;
	unsigned int len;
	unsigned int i;

	assert(config);

	items = config->items;
	len = config->item_len;

	if (items == NULL) {
		printf ("items is NULL\n");
		return -1;
	}
	
	if (!dev_item_is_container(&items[0])) {
		printf ("the frist item is not container\n");
		return -1;
	}
	
	if (!dev_item_is_end(&items[len - 1])) {
		printf ("the last item is not the end for frist item\n");
		return -1;
	}
	
	for (i = 0; i < len; ++i) {
		if (dev_item_is_container(&items[i]))
			count++;
		else if (dev_item_is_end(&items[i]))
			count--;
		if (count < 0)
			break;
	}
	
	if (count < 0) {
		printf ("the %dth item is DEV_END, but it do not match a container\n", i + 1);
		return -1;
	}

	if (count > 0) {
		printf ("the frist %d containers is not terminate with the DEV_END\n", count);
		return -1;
	}

	return 0;
}

struct dev_item *dev_item_dump(struct dev_item *items)
{
	do {
			switch (dev_item_get_type(item)) {
			case item_type_end:
			case item_type_item:
			case item_type_empty:
			case item_type_box:
			case item_type_menu:
			case item_type_choice:
			default:
				assert(0);
			}
	} while (!dev_item_is_end(++item));

	return item;
}

void dev_config_dump(struct dev_config *config)
{
	unsigned int i;

	assert(!dev_config_check_item_tree_valid(config));

	dev_item_dump(config->items);

}

#endif /* _DEV_ITEM_H_ */
