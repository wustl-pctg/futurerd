#include <cstdlib> // atexit
#include <cassert>

#include "tsan.hpp"
#include "futurerd.hpp"
#include "debug.hpp"

extern "C" {

//bool check_enable_instrumentation = true;
// When either is set to false, no errors are output
//bool instrumentation = false;
// needs to be reentrant due to reducer operations; 0 means checking
//bool t_checking_disabled = false;
bool t_checking_disabled = true;


} // extern "C"
