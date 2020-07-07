#include <string>
#include <iostream>

#include <polymake/Main.h>

using namespace polymake;

int main(int argc, const char* argv[])
{
   if (argc < 2) {
      cerr << "usage: " << argv[0] << " SCRIPT_FILE [...]" << endl;
      return 1;
   }
   try {
     // avoid reading any configuration
     Main pm("");
     pm.shell_enable();
     pm.set_application("common");
     auto funcall = prepare_call_function("script");
     for (int i = 1; i < argc; i++) {
        // extra arguments
        funcall << std::string(argv[i]);
     }
     funcall();
   }
   catch (const std::exception& ex) {
      cerr << "ERROR: " << ex.what() << endl;
      return 1;
   }
   return 0;
}

