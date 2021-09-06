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
      int i = 1;
      // avoid reading any configuration by default
      std::string settings{"none"};
      if (argc > 2 && strncmp(argv[1], "--config=", 9) == 0) {
         i++;
         settings = std::string(argv[1]+9);
      }
      Main pm(settings);
      pm.shell_enable();
      pm.set_application("common");
      auto funcall = prepare_call_function("script");
      for (; i < argc; i++) {
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

