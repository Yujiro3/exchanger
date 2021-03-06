PROGRAM  = exchanger
TARGETS  = main ini_parse log_logger fcgipp mem_rediscli exch_server exch_client exch_exchanger
DEP      = .depend
SRCS     = $(TARGETS:%=%.cpp)
OBJS     = $(addsuffix .o, $(basename $(SRCS)))

CXXFLAGS = -L/usr/local/lib -I/usr/local/include -Wall -g
CXXLIBS  = -lcrypto -lssl -levent -levent_openssl -lfcgi
CXX      = g++ -O2

all: dep $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(CXXLIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

dep:
ifeq ($(DEP),$(wildcard $(DEP)))
-include $(DEP)
else
	$(CXX) -MM $(CXXFLAGS) $(SRCS) > $(DEP)
endif

clean:
	rm -f $(PROGRAM) $(OBJS) $(DEP)

install: 
	cp ./$(PROGRAM) /usr/local/bin/
	mkdir -p /etc/$(PROGRAM)
	cp ./config/exch.conf /etc/$(PROGRAM)/exch.conf
	cp ./scripts/$(PROGRAM) /etc/init.d/
	chmod a+x /etc/init.d/$(PROGRAM)

uninstall: 
	rm /usr/local/bin/$(PROGRAM)
	rm /etc/init.d/$(PROGRAM)
	rm /etc/$(PROGRAM)/exch.conf
	rmdir /etc/$(PROGRAM)

