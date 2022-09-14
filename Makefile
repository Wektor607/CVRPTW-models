all:
	sudo rm -R build/
	sudo python3 setup.py install
	python3 main.py