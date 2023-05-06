EXECC = bin/client.exe
EXECS = bin/server.exe
EXECT = bin/test.exe
objC = obj/client.o
objS = obj/server.o
obj = \
obj/utility.o obj/model.o obj/controller.o obj/employee.o obj/args.o \
obj/client_handler.o obj/smart_winapi.o obj/concurrent_writer.o \
obj/menu.o obj/client_menu.o
objT = obj/test.o obj/tutility.o obj/tmodel.o obj/tcontroller.o obj/temployee.o obj/targs.o
CC = g++
CPPFLAGS = -std=c++17 -Wall -Iinclude
TESTLIB = -lgtest -lgtest_main -lgcov
COVERAGE = -fprofile-arcs -ftest-coverage
.PHONY = dirs run build clean test report clean_coverage
# run and build recipes
run: dirs $(EXECC) $(EXECS)
	./$(EXECS)
build: dirs $(EXECC) $(EXECS)
$(EXECC): $(objC) $(obj)
	$(CC) $(CPPFLAGS) $(objC) $(obj) -o $(EXECC)
$(EXECS): $(objS) $(obj)
	$(CC) $(CPPFLAGS) $(objS) $(obj) -o $(EXECS)
obj/%.o: src/%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@
# test recipes
report: clean_coverage test
	-lcov -c --directory obj --output-file coverage.info
	-lcov --extract coverage.info *\src\* -o coverage.info
	-genhtml coverage.info -o cov
	-rm coverage.info
	start cov/index.html
test: dirs $(EXECT)
	./$(EXECT)
$(EXECT): $(objT)
	$(CC) $(CPPFLAGS) $(objT) -o $(EXECT) $(TESTLIB)
obj/%.o: test/%.cc
	$(CC) $(CPPFLAGS) -c $< -o $@ $(TESTLIB)
obj/t%.o: src/%.cpp include/%.h
	$(CC) $(CPPFLAGS) $(COVERAGE) -c $< -o $@
# additional recipes
dirs:
	-mkdir -p obj bin
clean:
	-rm bin/*
	-rm obj/*
	-rm -fr cov/*
clean_coverage:
	-rm obj/*.gcda