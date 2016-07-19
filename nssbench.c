#include <sys/types.h> 
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <pwd.h>

struct nss_fn_name_map {
    const char *fname;
    enum nss_func *fn;
};

enum nss_funcs {
    NB_GETPWNAM,
};

static void nss_wrapper(enum nss_funcs nb_fn,
                        long long id_arg,
                        const char *str_arg)
{
    struct passwd *pwd = NULL;

    switch (nb_fn) {
    case NB_GETPWNAM:
        pwd = getpwnam(str_arg);
        break;
    }
}

static void nss_wrapper_loop(enum nss_funcs nb_fn,
                             long long id_arg,
                             const char *str_arg,
                             size_t count)
{
    for (size_t c = 0; c < count; c++) {
        nss_wrapper(nb_fn, id_arg, str_arg);
    }
}

int main(int argc, const char *argv[])
{
    id_t id_arg;
    const char *fname;
    size_t count = 1;

    if (argc < 3) {
        fprintf(stderr,
                "usage: nssbench fname arg [count]\n"
                "Example: nssbench getpwnam foobar\n"
                "Example: nssbench getpwuid 1234 10\n");
        return 1;
    }

    if (argc == 4) {
        count = atoi(argv[3]);
    }

    fname = argv[1];
    if (strcasecmp(fname, "getpwnam") == 0) {
        nss_wrapper_loop(NB_GETPWNAM, -1, argv[2], count);
    } else {
        fprintf(stderr, "nssbench does not handle %s\n", fname);
    }

    return 0;
}
