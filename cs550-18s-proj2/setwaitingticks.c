#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int tick;
  tick = atoi(argv[1]);
  setwaitingticks(tick);
  exit();
}
