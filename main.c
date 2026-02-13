#include "render/render.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage:\n"
                        "\t%s [index]\n"
                        "Indices:\n"
                        "\t1: static triangle\n"
                        "\t2: color-changing triangle #1\n"
                        "\t3: color-changing triangle #2\n",
                        argv[1]);
        return 1;
    }
    long index = strtol(argv[1], nullptr, 10);
    int ret = 0;
    switch (index) {
    case 1: {
        ret = render_static_triangle();
        break;
    }
    case 2: {
        ret = render_color_changing_triangle();
        break;
    }
    case 3: {
        ret = render_color_changing_triangle_2();
        break;
    }
    }
    return ret;
}
