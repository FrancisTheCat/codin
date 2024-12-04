#define SYS_read          0
#define SYS_write         1
#define SYS_open          2
#define SYS_close         3
#define SYS_stat          4
#define SYS_fstat         5
#define SYS_mmap          9
#define SYS_munmap        11
#define SYS_dup2          33
#define SYS_nanosleep     35
#define SYS_socket        41
#define SYS_accept        43
#define SYS_sendto        44
#define SYS_recvfrom      45
#define SYS_shutdown      48
#define SYS_bind          49
#define SYS_listen        50
#define SYS_setsockopt    54
#define SYS_fork          57
#define SYS_execve        59
#define SYS_exit          60
#define SYS_wait4         61
#define SYS_kill          62
#define SYS_getdents      78
#define SYS_getcwd        79
#define SYS_chdir         80
#define SYS_rmdir         84
#define SYS_unlink        87
#define SYS_arch_prctl    158
#define SYS_gettid        186
#define SYS_time          201
#define SYS_futex         202
#define SYS_clock_settime 227
#define SYS_clock_gettime 228
#define SYS_clock_getres  229
#define SYS_exit_group    231
#define SYS_waitid        247
#define SYS_openat        257
#define SYS_unlinkat      263
#define SYS_clone3        435

#define AF_INET 2
#define SOCK_STREAM 1

#define SO_REUSEADDR 2
#define SO_REUSEPORT 15

#define SOL_SOCKET 1

#define ARCH_SET_FS 0x1002

#define PROT_READ	 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC	 0x4
#define PROT_NONE	 0x0

#define MAP_SHARED          0x01
#define MAP_PRIVATE	        0x02
#define MAP_SHARED_VALIDATE 0x03
#define MAP_TYPE            0x0f

#define MAP_FIXED 0x10
#define MAP_FILE  0
#define MAP_ANONYMOUS	0x20

#define CSIGNAL              0x000000ff
#define CLONE_VM             0x00000100
#define CLONE_FS             0x00000200
#define CLONE_FILES          0x00000400
#define CLONE_SIGHAND        0x00000800
#define CLONE_PIDFD          0x00001000
#define CLONE_PTRACE         0x00002000
#define CLONE_VFORK          0x00004000
#define CLONE_PARENT         0x00008000
#define CLONE_THREAD         0x00010000
#define CLONE_NEWNS          0x00020000
#define CLONE_SYSVSEM        0x00040000
#define CLONE_SETTLS         0x00080000
#define CLONE_PARENT_SETTID  0x00100000
#define CLONE_CHILD_CLEARTID 0x00200000
#define CLONE_DETACHED       0x00400000
#define CLONE_UNTRACED       0x00800000
#define CLONE_CHILD_SETTID   0x01000000
#define CLONE_NEWCGROUP      0x02000000
#define CLONE_NEWUTS         0x04000000
#define CLONE_NEWIPC         0x08000000
#define CLONE_NEWUSER        0x10000000
#define CLONE_NEWPID         0x20000000
#define CLONE_NEWNET         0x40000000
#define CLONE_IO             0x80000000

#define __WNOTHREAD 0x20000000
#define __WALL      0x40000000
#define __WCLONE    0x80000000
