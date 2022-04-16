#ifndef PTSLEEP_H
#define PTSLEEP_H

#include <pt.h>

#define PT_SLEEP(pts, millis_var, t) do { \
    (millis_var) = millis(); \
    PT_WAIT_UNTIL((pts), millis() - (millis_var) > (t)); \
    } while (0)

#endif /* PTSLEEP_H */
