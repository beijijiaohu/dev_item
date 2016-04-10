#ifndef _DEV_ITEM_H_
#define _DEV_ITEM_H_

#include "common.h"

enum dev_value_type {
	value_type_bool,
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
		unsigned char v_bool;
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

	item_type_start_of_item,
	item_type_bool = item_type_start_of_item,
	item_type_char,
	item_type_uchar,
	item_type_short,
	item_type_ushort,
	item_type_int,
	item_type_uint,
	item_type_llong,
	item_type_ullong,
	item_type_string,
	item_type_enum,
	item_type_end_of_item = item_type_enum,

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

struct dev_tree;
struct dev_item_config;

struct dev_item_ops {
	int (*item_value_is_valid)(struct dev_config *config, unsigned int index_of_item, struct dev_value_box *value);
};

struct dev_config {
	struct dev_item *items;
	unsigned int item_len;
	struct dev_item_ops item_ops;
	struct dev_tree *tree;
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
	.type = item_type_##_type,\
	.name = (_name),\
	.help_info = (_help_info),\
	.value.item.value.type = value_type_##_type,\
	.value.item.value.default_value = (unsigned long long)(_default_value),\
}

#define DEV_ITEM_ENUM(_name, _default_value, _enum_items, _value_is_valid, _help_info) \
{ \
	.type = item_type_enum,\
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

int item_type_is_container(unsigned int type)
{
	return type == item_type_box ||
		type == item_type_menu ||
		type == item_type_choice;
}

int item_type_is_item(unsigned int type)
{
	return type >= item_type_start_of_item && type <= item_type_end_of_item;
}

int dev_item_is_selected(struct dev_item *item)
{
	return item->is_selected;
}

void dev_item_set_selected(struct dev_item *item, int selected)
{
	item->is_selected = !!selected;
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
	return item_type_is_container(item->type);
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

static inline print_ntimes_str(const char *str, unsigned int n)
{
	while (n-- != 0) {
		printf ("%s", str);
	}
}

#define print_with_depth(depth, x...) \
do {\
	print_ntimes_str( "  ", depth);\
	printf (x);					  \
} while (0)

const char *str_of_item_types[] = {
	[item_type_end] = "end",

	[item_type_char] = "char",
	[item_type_uchar] = "uchar",
	[item_type_short] = "short",
	[item_type_ushort] = "ushort",
	[item_type_int] = "int",
	[item_type_uint] = "uint",
	[item_type_llong] = "llong",
	[item_type_ullong] = "ullong",
	[item_type_string] = "string",
	[item_type_enum] = "enum",

	[item_type_empty] = "empty",
	[item_type_box] = "box",
	[item_type_menu] = "menu",
	[item_type_choice] = "choice",
};

void dev_item_dump_one(struct dev_item *item, unsigned int depth)
{
	unsigned int type;
	const char *str;

	type = dev_item_get_type(item);
	str = str_of_item_types[type];

	if (type != item_type_end)
		print_with_depth (depth, "%s - %s\n", str, dev_item_get_name(item));
	else
		print_with_depth (depth - 1, "%s\n", str);
}

struct dev_item *dev_item_dump(struct dev_item *items, unsigned int depth)
{
	unsigned int type;

	/* dump container */
	dev_item_dump_one(items, depth);
	depth += 1;

	/* dump the child items */
	while (1) {
		type = dev_item_get_type(++items);

		if (type == item_type_end)
			goto out;

		if (item_type_is_container(type))
			items = dev_item_dump(items, depth);
		else
			dev_item_dump_one(items, depth);
	};

out:
	/* dump end */
	dev_item_dump_one(items, depth);

	return items;
}

void dev_config_dump(struct dev_config *config)
{
	unsigned int i;

	assert(!dev_config_check_item_tree_valid(config));

	dev_item_dump(config->items, 0);
}

struct dev_tree {
	unsigned char type;
	struct dev_item *item;
};

struct dev_tree_item {
	struct dev_tree tree;
};

struct dev_tree_container {
	struct dev_tree tree;
	struct dev_tree **list;
	unsigned int list_len;
};

unsigned int dev_item_container_get_child_count(struct dev_item *items)
{
	unsigned int type;
	unsigned int count = 0;
	int depth = 1;

	while (depth != 0) {
		type = dev_item_get_type(++items);
		
		if (depth == 1)
			count++;

		if (item_type_is_container(type))
			depth++;
		else if (type == item_type_end)
			depth--;
	}

	/* remove the last end, it is end for the top container */
	return count - 1;
}

struct dev_tree_container *dev_tree_malloc_container(struct dev_item *item)
{
	struct dev_tree_container *container;
	unsigned int list_len;

	list_len = dev_item_container_get_child_count(item);
	container = malloc(sizeof(struct dev_tree_container) + (list_len * sizeof(struct dev_tree *)));
	assert(container);

	container->tree.item = item;
	container->tree.type = dev_item_get_type(item);
	container->list = (void *)&container[1];
	container->list_len = list_len;

	return container;
}

struct dev_tree_item *dev_tree_malloc_item(struct dev_item *item)
{
	struct dev_tree_item *it;

	it = malloc(sizeof(struct dev_tree_item));
	assert(it);

	it->tree.item = item;
	it->tree.type = dev_item_get_type(item);

	return it;
}

struct dev_tree *dev_tree_malloc(struct dev_item *item)
{
	unsigned char type = dev_item_get_type(item);
	struct dev_tree *tree;

	assert(type != item_type_end);

	if (item_type_is_container(type))
		tree = (struct dev_tree *) dev_tree_malloc_item(item);
	else
		tree = (struct dev_tree *) dev_tree_malloc_item(item);

	return tree;
}

struct dev_tree *dev_tree_create_inner(struct dev_item **items_p)
{
	struct dev_tree_container *container;
	struct dev_tree *tree;
	struct dev_item *items = *items_p;
	struct dev_tree **list;
	unsigned int type;

	assert(dev_item_is_container(items));

	container = dev_tree_malloc_container(items);
	list = container->list;

	while (1) {
		type = dev_item_get_type(++items);

		if (type == item_type_end)
			goto out;

		if (item_type_is_container(type))
			tree = dev_tree_create_inner(&items);
		else
			tree = (struct dev_tree *) dev_tree_malloc_item(items);

		*list++ = tree;
	}

out:
	*items_p = items;
	return (struct dev_tree *) container;
}

struct dev_tree *dev_tree_create(struct dev_item *items)
{
	return dev_tree_create_inner(&items);
}

void dev_tree_dump_one(struct dev_tree *tree, unsigned int depth)
{
	dev_item_dump_one(tree->item, depth);
}

void dev_tree_dump(struct dev_tree *tree, unsigned int depth)
{
	unsigned int i;
	unsigned int type;
	struct dev_tree **list;
	unsigned int list_len;
	struct dev_tree_container *container = (struct dev_tree_container *) tree;

	assert(item_type_is_container(container->tree.type));
	list = container->list;
	list_len = container->list_len;

	dev_tree_dump_one((struct dev_tree *)container, depth);
	depth += 1;

	for (i = 0; i < list_len; ++i) {
		type = list[i]->type;
		if (item_type_is_container(type))
			dev_tree_dump(list[i], depth);
		else
			dev_tree_dump_one(list[i], depth);
	}
}

const char *cmdline_str_of_item_types[] = {
	[item_type_end] = "\n",

	[item_type_char] = "char",
	[item_type_uchar] = "uchar",
	[item_type_short] = "short",
	[item_type_ushort] = "ushort",
	[item_type_int] = "int",
	[item_type_uint] = "uint",
	[item_type_llong] = "llong",
	[item_type_ullong] = "ullong",
	[item_type_string] = "string",
	[item_type_enum] = "enum",

	[item_type_empty] = "empty",
	[item_type_box] = "box",
	[item_type_menu] = "menu",
	[item_type_choice] = "choice",
};

void dev_config_cmdline_config_box(struct dev_config *config, struct dev_tree_container *container, unsigned int depth)
{
	unsigned int i;
	unsigned int type;
	struct dev_tree **list;
	unsigned int list_len;
	struct dev_tree_container *container = (struct dev_tree_container *) tree;

	printf ("\n[%s] configurations\n", dev_item_get_name(tree.item));

	list = container->list;
	list_len = container->list_len;
	depth += 1;

	for (i = 0; i < list_len; ++i) {
		dev_item_set_selected(list[i]->item, 1);

		type = list[i]->type;
		if (item_type_is_container(type))
			dev_config_cmdline_config_inner(config, list[i], depth);
		else if (item_type_is_item(type))
			dev_tree_dump_one(list[i], depth);
	}
}

void dev_config_cmdline_config_inner(struct dev_config *config, struct dev_tree_container *container, unsigned int depth)
{
	unsigned int i;
	unsigned int type;
	struct dev_tree **list;
	unsigned int list_len;

	type = container->tree.type;
	assert(item_type_is_container(type));

	list = container->list;
	list_len = container->list_len;

	switch (type) {
	case item_type_box:

		break;
	case item_type_menu:
		printf ("\n[%s] configurations\n", dev_item_get_name(tree.item));
		break;
	case item_type_choice:
		printf ("\n[%s] configuration\n", dev_item_get_name(tree.item));
		break;
	default:
		printf ("TODO: add case for your new item type\n");
		assert(0);
	}

	depth += 1;

	for (i = 0; i < list_len; ++i) {
		type = list[i]->type;
		if (item_type_is_container(type))
			dev_tree_dump(list[i], depth);
		else
			dev_tree_dump_one(list[i], depth);
	}
}

void dev_config_cmdline_config(struct dev_config *config)
{
	dev_config_cmdline_config_inner(config, (struct dev_tree_container *)config->tree);
}

void dev_tree_set_select_all_child(struct dev_tree **list, unsigned int list_len, int selected)
{
	for (; list_len-- > 0;) {
		dev_item_set_selected(list[list_len].item, selected);
	}
}

void dev_tree_select_only_one_child(struct dev_tree **list, unsigned int list_len, unsigned int index)
{
	assert(index < list_len);
	dev_tree_set_select_all_child(list, list_len, 0);
	dev_item_set_selected(list[index].item, selected);
}

void dev_value_set_value(struct dev_value_box *box, unsigned long long value)
{
	unsigned int len;
	void *mem = (void *)&box->box;

	switch (container->tree.type) {
	case value_type_bool:
	case value_type_char:
	case value_type_uchar:
		
	case value_type_short:
	case value_type_ushort:
	case value_type_int:
	case value_type_uint:
	case value_type_llong:
	case value_type_ullong:
	case value_type_string:
	case value_type_enum:
	}
}

void dev_tree_init_item(struct dev_tree *tree)
{

	struct dev_value_box 

}

void dev_tree_init_container(struct dev_tree_container *container)
{
	unsigned int i;
	unsigned int type;
	struct dev_tree **list;
	unsigned int list_len;

	type = container->tree.type;
	assert(item_type_is_container(type));

	list = container->list;
	list_len = container->list_len;

	switch (type) {
	case item_type_box:
		dev_tree_set_select_all_child(list, list_len, 1);
		break;
	case item_type_menu:
		dev_tree_set_select_all_child(list, list_len, 0);
		break;
	case item_type_choice:
		dev_tree_select_only_one_child(list, list_len, 0);
		break;
	default:
		printf ("TODO: add case for your new item type\n");
		assert(0);
	}

	for (i = 0; i < list_len; ++i) {
		type = list[i]->type;
		if (item_type_is_container(type))
			dev_tree_init_container(list[i]);
		else if (item_type_is_item(type))
			dev_tree_init_item(list[i], depth);
	}
}

void dev_config_init_items(struct dev_config *config)
{
	dev_tree_init_container((struct dev_tree_container *)config->tree);
}

void dev_config_init(struct dev_config *config)
{
	assert(!dev_config_check_item_tree_valid(config));
	config->tree = dev_tree_create(config->items);
	dev_config_init_items(config);
}

#endif /* _DEV_ITEM_H_ */
