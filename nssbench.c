#include <sys/types.h> 
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

struct nss_fn_name_map {
    const char *fname;
    enum nss_func *fn;
};

enum nss_funcs {
    NB_GETPWNAM,
    NB_GETGRNAM,
    NB_GETPWUID,
    NB_GETGRGID,
    NB_INITGR,
};

/* This function is separate and the enum values are used to allow systemtap
 * to only measure the single function. I wasn't able to figure out how to
 * run systemtap for either getpwnam() or getpwnam_r() directly w/o a wrapper..
 */
static void nss_wrapper(enum nss_funcs nb_fn,
                        long long id_arg,
                        const char *str_arg)
{
    struct passwd *pwd = NULL;
    struct group *grp = NULL;
    int ret;

    switch (nb_fn) {
    case NB_GETPWNAM:
        pwd = getpwnam(str_arg);
        break;
    case NB_GETGRNAM:
        grp = getgrnam(str_arg);
        break;
    case NB_GETPWUID:
        pwd = getpwuid(id_arg);
        break;
    case NB_GETGRGID:
        grp = getgrgid(id_arg);
        break;
    case NB_INITGR:
        ret = initgroups(str_arg, 0);
        break;
    default:
        break;
    }
}

static void nss_wrapper_loop(enum nss_funcs nb_fn,
                             const char *fn_name, /* for systemtap.. */
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
        nss_wrapper_loop(NB_GETPWNAM, fname, -1, argv[2], count);
    } else if (strcasecmp(fname, "getgrnam") == 0) {
        nss_wrapper_loop(NB_GETGRNAM, fname, -1, argv[2], count);
    } else if (strcasecmp(fname, "getpwuid") == 0) {
        nss_wrapper_loop(NB_GETPWUID, fname, atoi(argv[2]), argv[2], count);
    } else if (strcasecmp(fname, "getgrgid") == 0) {
        nss_wrapper_loop(NB_GETGRGID, fname, atoi(argv[2]), argv[2], count);
    } else if (strcasecmp(fname, "initgroups") == 0) {
        nss_wrapper_loop(NB_INITGR, fname, -1, argv[2], count);
    } else {
        fprintf(stderr, "nssbench does not handle %s\n", fname);
    }

    return 0;
}
