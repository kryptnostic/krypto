krypto
======

C++ Implementation of Multivariate Quadratic FHE 

Using Eclipse
==========

This project is configured to create a .project and .cproject file for working with Eclipse Luna with CDT 8.2+

Use the 	``./gradlew cdt`` task to create the Eclipse files

Note: The gradle cdt plugin is under active development and currently requires that the component be named "main" (rather than something like "krypto") in order to work correctly. This is due to a hardcoded setting in this file https://github.com/gradle/gradle/blob/eb4d045f12dab4fa12a7da7ee99b6a70fe024a66/subprojects/ide-native/src/main/groovy/org/gradle/ide/cdt/CdtIdePlugin.groovy#L59 


Building
==========

Use gradle to build with ``./gradlew build``

Testing
==========

Building will run all the unit tests in src/mainTest/cpp using the google-test framework. Do not modify test_main.cpp or add a main() function to any test script. Follow the format of test_trivial.cpp

Test results are published to build/test-results and can be viewed within Eclipse by opening the xml results file.