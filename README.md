## MADlib Port

This project brings in-DBMS data analytics to Impala. This leverages previous
work done by two projects:

* MADlib (http://madlib.net/)

* Bismarck (http://hazy.cs.wisc.edu/hazy/victor/bismarck/)

Each of these projects use User Defined Aggregates (UDAs) to train analytic
models using an existing DBMS's data management and processing ability. 


### Dependencies

* `eigen3` (`yum install eigen3-devel`)

* `boost>=1.54.0` (manual install on CentOS 6)

* `impala-udf-devel` (`yum`-installable with Cloudera repo)


### Installation

```bash
make
python python/deploy.py <relevant options>
```

### Code Base

This is a fork of MADlib 1.0 which has been modified for use with Impala.
The specific changes were:

* `madlib/test` with tests for the new code

* `madlib/Makefile` to make the tests

* `madlib/src/ports/metaport` which is a modified MADlib backend for main memory


### Example

To run the example SVM,:

1. Create database `toysvm`

2. Run `make`.  If it fails with an error like `lib/libsvm.so: undefined reference to 'impala_udf::FunctionContext::Allocate(int)'`, it's ok.

3. Register the UDFs with a database (without re-making the binaries), execute: `python python/deploy.py -p -o /path/for/libs toysvm`

4. Create a synthetic table of examples in the database `toysvm` with the table `toy`: `python python/gen_classify_data.py toysvm toy`

5. `python python/impala_svm.py lbl e0 e1 e2 --db toysvm --table toy -e 1`

6. `impala-shell -q 'use toysvm; select iter, printarray(decodearray(model)) from history;'`

Also see example usage in the impyla repo.
