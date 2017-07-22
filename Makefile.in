
all    : gocr src 
.PHONY : gocr src examples install uninstall help clean 
 
gocr   : gocr/Makefile
	@echo; echo "@@ building gocr libraries... @@"; sleep 1
	$(MAKE) -C gocr libs 
src    : src/Makefile gocr
	@echo; echo "@@ building psbot0 library... @@"; sleep 1
	$(MAKE) -C src 
examples: 
	@echo; echo "@@ building examples... @@"; sleep 1
	$(MAKE) -C examples
install: 
	@echo; echo "@@ installing psbot0... @@"; sleep 1
	$(MAKE) -C src install
uninstall:
	@echo; echo "@@ uninstalling psbot0... @@"; sleep 1
	$(MAKE) -C src uninstall
clean  :	
	@echo; echo "@@ cleaning all... @@"; sleep 1
	$(MAKE) -C src clean   
	$(MAKE) -C gocr clean 
	#$(MAKE) -C examples clean
help   :
	@printf "  make           - compile all\n"
	@printf "  make gocr      - create gocr libraries\n"
	@printf "  make src       - create psbot0 library\n"
	@printf "  make examples  - compile examples in ./examples/\n"
	@printf "  make clean     - cleaning almost everything\n"
	@printf "  make install   - install psbot0 library to /usr/local\n"
	@printf "  make uninstall - remove installed files from the system\n"
	@printf "\n"
