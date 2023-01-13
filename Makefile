clean:
	rm -f GIS.exe

run:
	rm -f GIS.exe; \
	g++ -g -pthread -std=c++20  *.cpp DatabaseManagement/*.cpp DatabaseInterface/*.cpp -o GIS.exe ./GIS.exe
	#GIS.exe dbfile.txt script01.txt log02.txt
	#if subdirectories should be included for header files and such, use -I. Try including all files using I for each folder to prevent needing relative paths in includes.