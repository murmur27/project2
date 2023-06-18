CC=g++
CFLAGS=--std=c++14 
TARGET=final

$(TARGET):
	$(CC) $(CFLAGS) -o $@ final_skeleton_windows.cpp Screen_manager.cpp


clean:
	del $(TARGET).exe