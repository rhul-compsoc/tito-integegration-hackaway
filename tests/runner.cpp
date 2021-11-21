#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../src/id_card_gen.h"

#include "testIdCardGen.h"
#include "testTito.h"

#ifndef TEST
#include "testNcursesUtils.h" // is broken on github actions as they have bad tty
#endif

#define XMLOUT

// register test suite
CppUnit::Test *suite() {
    CppUnit::TestFactoryRegistry &registry =
        CppUnit::TestFactoryRegistry::getRegistry();
    registry.registerFactory(
        &CppUnit::TestFactoryRegistry::getRegistry("test_tito_integration"));
    return registry.makeTest();
}

int main(int argc, char* argv[]) {
    loadFont();
    
    std::string testPath = (argc > 1) ? std::string(argv[1]) : "";
    // Create the event manager and test controller
    CppUnit::TestResult controller;
    
    // Add a listener that colllects test result
    CppUnit::TestResultCollector result;
    controller.addListener(&result);
    
    // Add a listener that print dots as test run.
    //CppUnit::TextTestProgressListener progress;
    
    // Add a listener that print name of methods as test run.
    CppUnit::BriefTestProgressListener progress;
    controller.addListener(&progress);
    
    // Add the top suite to the test runner
    CppUnit::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    
    try {
        std::cout << std::endl;
        std::cout << "test tito integration with cppunit" << std::endl;
        std::cout << "========================="  << std::endl;
        runner.run(controller, testPath);
        std::cout << "=========================" << std::endl;
        std::cout << std::endl;
        
        // Print test in XML or compiler compatible format.
        #ifdef XMLOUT
        CppUnit::XmlOutputter outputter( &result, std::cerr );
        #else
        CppUnit::CompilerOutputter outputter( &result, std::cerr );
        #endif
        outputter.write();
    } catch ( std::invalid_argument& e ) { 
        // Test path not resolved
        std::cerr << std::endl << "ERROR: " << e.what() << std::endl;
        return 0;
    }
    
    return result.wasSuccessful() ? 0 : 1;
}
