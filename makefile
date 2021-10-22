run:
	g++ Main.cpp -std=c++11 -o main -I /usr/include/opencv2 -L /usr/lib -lopencv_imgcodecs -lopencv_core -lopencv_imgproc
	./main

shell:
	g++ ShellScript.cpp -std=c++11 -o shell 
	./shell
