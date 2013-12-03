EIGEN=/usr/include/eigen3/

INCLUDES=-I$(EIGEN) -Imadlib/src -Imadlib/src/ports -Imadlib/src/ports/metaport -Isrc 

TEST_LIBS=-lImpalaUdf -Llib

all: directories lib/libbismarckarray.so lib/libsvm.so lib/liblogr.so lib/liblinr.so tests

tests: test_bin/svm_test test_bin/logreg_test test_bin/linreg_test

clean:
	rm -rf ./objs
	rm -rf ./lib
	rm -rf ./test_bin

.PHONY: directories 

directories: objs lib test_bin

objs:
	mkdir -p objs
lib:
	mkdir -p lib
test_bin:
	mkdir -p test_bin

lib/libbismarckarray.so: 
	g++ -O3 -c -fPIC -o objs/bismarckarray.o src/bismarckarray.cc $(INCLUDES) 
	g++ -O3 -shared -o lib/libbismarckarray.so objs/bismarckarray.o

lib/liblinr.so: 
	g++ -O3 -c -fPIC -o objs/liblinr.o src/linreg.cc $(INCLUDES) 
	g++ -O3 -shared -o lib/liblinr.so objs/liblinr.o

lib/libsvm.so:
	g++ -O3 -c -fPIC -o objs/libsvm.o src/svm.cc $(INCLUDES) 
	g++ -O3 -shared -o lib/libsvm.so objs/libsvm.o


lib/liblogr.so:
	g++ -O3 -c -fPIC -o objs/liblogr.o src/logreg.cc $(INCLUDES) 
	g++ -O3 -shared -o lib/liblogr.so objs/liblogr.o

documentation:
	doxygen doc/doxconf

test_bin/logreg_test: 
	g++ -I. -o test_bin/logreg_test test/test-logreg.cc -g -O0 $(INCLUDES) 

test_bin/svm_test: 
	g++ -I. -o test_bin/svm_test test/test-svm.cc -g -O0 $(INCLUDES) -Wall $(TEST_LIBS) -lsvm

test_bin/mf_test: 
	g++ -I. -o test_bin/mf_test test/test-mf.cc -g -O0 $(INCLUDES) -Wall $(TEST_LIBS) 

test_bin/linreg_test:
	g++ -I. -o test_bin/linreg_test test/test-linreg.cc -g -O0 $(INCLUDES) $(TEST_LIBS) -llinr

