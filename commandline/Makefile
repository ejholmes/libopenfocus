all:
	g++ -o openfocus openfocus.cpp -lusb -lopenfocus
	g++ -I.. -o bootloader bootloader.cpp -lusb -lopenfocus

clean:
	rm -f bootloader openfocus
