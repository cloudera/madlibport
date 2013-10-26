EIGEN=/usr/include/eigen3/

SRC=-I$(EIGEN) -Imadlib/src -Imadlib/src/ports -Imadlib/src/ports/metaport -Isrc -Iudf/

all: directories objs/udf.o lib/libbismarckarray.so lib/libsvm.so lib/liblogr.so lib/liblinr.so

clean:
	rm -rf ./objs
	rm -rf ./lib

.PHONY: directories 

directories: objs lib

objs:
	mkdir -p objs
lib:
	mkdir -p lib

objs/udf.o: 
	g++ -I. udf/udf/udf.cc -c -fPIC -o objs/udf.o $(SRC) -DIMPALA_UDF_SDK_BUILD=1

uda_test: objs/udf.o
	g++ -I. -o uda_test -lgtest udf/udf/uda-test.cc objs/udf.o $(SRC) -DIMPALA_UDF_SDK_BUILD=1

udf_test: objs/udf.o
	g++ -I. -o udf_test -lgtest udf/udf/udf-test.cc objs/udf.o $(SRC)

lib/libbismarckarray.so: objs/udf.o
	g++ -O3 -c -fPIC -o objs/bismarckarray.o src/bismarckarray.cc $(SRC) 
	g++ -O3 -shared -o lib/libbismarckarray.so objs/bismarckarray.o objs/udf.o

lib/liblinr.so: objs/udf.o
	g++ -O3 -c -fPIC -o objs/liblinr.o src/linreg.cc $(SRC) 
	g++ -O3 -shared -o lib/liblinr.so objs/liblinr.o objs/udf.o

lib/libsvm.so: objs/udf.o
	g++ -O3 -c -fPIC -o objs/libsvm.o src/svm.cc $(SRC) 
	g++ -O3 -shared -o lib/libsvm.so objs/libsvm.o objs/udf.o


lib/liblogr.so: objs/udf.o
	g++ -O3 -c -fPIC -o objs/liblogr.o src/logreg.cc $(SRC) 
	g++ -O3 -shared -o lib/liblogr.so objs/liblogr.o objs/udf.o


documentation:
	doxygen doc/doxconf

bin/logreg_test: 
	g++ -I. -o bin/logreg_test test/test-logreg.cc -g -O0 $(SRC)

bin/svm_test: 
	g++ -I. -o bin/svm_test test/test-svm.cc -g -O0 $(SRC) -Wall

bin/mf_test: 
	g++ -I. -o bin/mf_test test/test-mf.cc -g -O0 $(SRC) -Wall

bin/linreg_test: objs/udf.o
	g++ -I. -o bin/linreg_test -lgtest test/test-linreg.cc objs/udf.o -g -O0 $(SRC)

