.PHONY:default
default:
	@echo "Build Started"
	gcc -g3 sm1.c -o sm1 -lpthread -lrt
	gcc -g3 sm2.c -o sm2 -lpthread -lrt
	@echo "Build Completed"

.PHONY:clean
clean:
	rm -rf sm1 sm2
