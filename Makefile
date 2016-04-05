
target = code_generater
objects = main.o i2c_dev_item.o

default: clean $(target)

$(target): $(objects)
	gcc -o $(target) $(objects) 
	@echo ">>>> $(target) is ok !"

.PHONY: clean
clean:
	@rm -f $(objects)



