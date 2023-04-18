EXECC = bin/client.exe
EXECS = bin/server.exe
EXECT = bin/test.exe
objC = obj/client.o
objS = obj/server.o
obj = obj/utility.o obj/model.o obj/controller.o
objT = obj/test.o obj/tutility.o
CC = g++
CPPFLAGS = -std=c++17 -Wall -Iinclude
TESTLIB = -lgtest -lgtest_main -lgcov
COVERAGE = -fprofile-arcs -ftest-coverage
.PHONY = dirs run build clean test report clean_coverage
# run and build recipes
run: dirs $(EXECC) $(EXECS)
	./$(EXECC)
build: dirs $(EXECC) $(EXECS)
$(EXECC): $(objC) $(obj)
	$(CC) $(CPPFLAGS) $(objC) $(obj) -o $(EXECC)
$(EXECS): $(objS) $(obj)
	$(CC) $(CPPFLAGS) $(objS) $(obj) -o $(EXECS)
obj/%.o: src/%.cpp include/%.h
	$(CC) $(CPPFLAGS) -c $< -o $@
# test recipes
report: clean_coverage test
	-lcov -c --directory obj --output-file coverage.info
	-lcov --extract coverage.info *\src\* -o coverage.info
	-genhtml coverage.info -o cov
	-rm coverage.info
	start cov/index.html
test: dirs $(TEXEC)
	./$(TEXEC)
$(TEXEC): $(objT)
	$(CC) $(CPPFLAGS) $(objT) -o bin/test.exe $(TESTLIB)
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