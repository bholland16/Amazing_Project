EXEC = am

STARTUP_DIR=./AMStartup/
CLIENT_DIR=./avatar_client/

# Commands
$(EXEC): 
	cd $(STARTUP_DIR); make;
	cd $(CLIENT_DIR); make;

clean:
	rm -f *~
	rm -f *#
	rm -f ./$(EXEC)
	rm -f *.o
	cd $(STARTUP_DIR); make clean;
	cd $(CLIENT_DIR); make clean;
