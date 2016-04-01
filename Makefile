
target = code_generater
objects = main.o

default: clean $(target)

$(target): $(objects)
	gcc -o $(target) $(objects) 
	@echo ">>>> $(target) is ok !"

.PHONY: clean
clean:
	@rm $(objects)



