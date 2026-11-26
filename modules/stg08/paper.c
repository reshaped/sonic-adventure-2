/* shared/paper.c for Security Hall's banknotes, with a gravity of its own,
 * and security_paper_launch, which takes a build */
#include "stg08/paper.h"

#define PAPER_DIRECT
#define PAPER_GRAVITY 0.03F
#define PAPER_TASK_NAME "EfShPaper3DExec"
#include "shared/paper.c" /* NOLINT(bugprone-suspicious-include) */
