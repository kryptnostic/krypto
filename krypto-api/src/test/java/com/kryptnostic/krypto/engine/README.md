KryptnosticEngineTest
===========

## Eclipse Run Configurations

To run the JUnit tests properly in Eclipse,
several arguments have to be added to the run
configurations to avoid a JVM crash due to stack overflow.


## Program arguments

ulimit -s 16384


## VM arguments

-Xms1024m
-Xmx1024m
-Xss64m