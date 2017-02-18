
#include <stdio.h>

#include "rgb_leds.h"
#include "remote.h"

void rgb_leds_cmd (int argc, char ** argv){
    int err = 1;
    int chan = 0;
    float r = 0.0, g = 0.0, b = 0.0;

    if (argc != 5) {
        remote_print("Wrong usage. rgb_leds <chan 0-3> <red> <green> <blue>\r\n");
        err = -1;
    }

    /* Get channel number */
    if (err > 0){
        err = sscanf(argv[1], "%d", &chan);
        if (err != 1){
            remote_print("Channel selector (%s) has the wrong format. It must be an integer lower than %d.\r\n",
                         argv[1], RGB_LEDS_N);
            err = -1;
        }
    }

    /* Get red */
    if (err > 0){
        err = sscanf(argv[2], "%f", &r);
        if (err != 1 || r > 1.0 || r < 0.0){
            remote_print("Color selector (%s) has the wrong format. It must be a float 0-1.\r\n",
                         argv[2]);
            err = -1;
        }
    }

    /* Get green */
    if (err > 0){
        err = sscanf(argv[3], "%f", &g);
        if (err != 1 || g > 1.0 || g < 0.0){
            remote_print("Color selector (%s) has the wrong format. It must be a float 0-1.\r\n",
                         argv[3]);
            err = -1;
        }
    }

    /* Get blue */
    if (err > 0){
        err = sscanf(argv[4], "%f", &b);
        if (err != 1 || b > 1.0 || b < 0.0){
            remote_print("Color selector (%s) has the wrong format. It must be a float 0-1.\r\n",
                         argv[4]);
            err = -1;
        }
    }

    /* Set color */
    if (err > 0) {
        remote_print("OK!\r\n");
        rgb_leds_set_float((u8) chan, r, g, b);
    }
}
