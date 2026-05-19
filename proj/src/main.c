#include <lcom/lcf.h>

#include "video.h"
#include "my_timer.h"
#include "i8042.h"
#include "keyboard.h"
#include "mouse.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");

  lcf_trace_calls("/home/lcom/labs/grupo_2leic16_2/proj/trace.txt");

  lcf_log_output("/home/lcom/labs/grupo_2leic16_2/proj/output.txt");

  if (lcf_start(argc, argv))
    return 1;
    
  lcf_cleanup();
  return 0;
}

int(proj_main_loop)(int argc, char* argv[]) {
  return 0;
}
