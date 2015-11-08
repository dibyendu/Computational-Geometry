all : libMake progMake
	@echo "---------------------------------------------------------------------------\n\nNow execute the following in your terminal :\n\n"
	export LD_LIBRARY_PATH=../gui:$$LD_LIBRARY_PATH
	@echo "cd ./sweep\n./draw\n\n---------------------------------------------------------------------------"

libMake :
	make -C ./gui

progMake :
	make -C ./sweep

clean : libClean progClean

libClean : 
	make -C ./gui clean

progClean :
	make -C ./sweep clean