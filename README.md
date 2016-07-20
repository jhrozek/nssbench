# nssbench
This is a simple C program and associated systemtap script that measures
the time spent in a libc NSS operation and in the particular operations
in the files and sss NSS modules.

## build
just `make`

## usage
You need systemtap installed along with kernel-headers and
kernel-devel. Then, in one window, run the nssbench program and in another
window, run the systemtap script.

The nssbench program accepts two, optionally three positional arguments
- the NSS function to run, the input of the function and optionally the
number of repetitions. For example:
```bash
./nssbench getpwnam jhrozek 100
```

The systemtap script is run without any arguments:
```bash
sudo stap ./nssbench.stp
```

You don't need to restart the script between successive runs of nssbench,
the variables are cleared automatically. The script should produce output
such as:
```bash
sudo stap ./nssbench.stp
nss operation getpwnam(jhrozek) took 2654 us
_nss_files_getpwnam cnt:100 avg:15 min:13 max:53 sum:1548 us
_nss_sss_getpwnam cnt:100 avg:3 min:2 max:45 sum:305 us
```
