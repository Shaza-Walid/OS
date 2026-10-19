# Linux Kernel Modules: jiffies, seconds and a simple module

Programming Project #1 for **CSE323: Operating Systems** (Faculty of Engineering, Zagazig University).

This project is an introduction to Linux kernel modules. It contains three loadable kernel modules that print kernel information to the kernel log and expose kernel timing data through the `/proc` file system.

## Modules

| File | What it does |
|------|--------------|
| `simple.c` | Logs a message on load/unload. On load it prints `GOLDEN_RATIO_PRIME`, `jiffies` and `HZ`. On unload it prints `gcd(3300, 24)`, the `jiffies` value, and the time elapsed since the module was loaded. |
| `jiffies_module.c` | Creates `/proc/jiffies`. Reading it returns the current value of `jiffies`. |
| `seconds_module.c` | Creates `/proc/seconds`. Reading it returns the number of seconds since the module was loaded, computed as `(jiffies - start_jiffies) / HZ`. |

Both `/proc` entries are removed when their module is unloaded.

## Requirements

- A Linux machine or virtual machine (tested on Kali Linux 2026.2 in VMware Workstation, kernel `7.1.5+kali-amd64`)
- Build tools and headers for the running kernel

```bash
# Debian / Ubuntu / Kali
sudo apt update
sudo apt install build-essential linux-headers-$(uname -r)
```

> The code uses `struct proc_ops`, which is required on Linux 5.6 and newer. Older kernels need `struct file_operations` instead.

> Kernel modules run with full privileges and can crash the system. Use a virtual machine and take a snapshot first.

## Build

```bash
make
```

This produces `simple.ko`, `jiffies_module.ko` and `seconds_module.ko`.

To clean up (remove modules first if they are loaded):

```bash
make clean
```

## Usage

### simple

```bash
sudo insmod simple.ko
lsmod | grep simple
sudo dmesg | tail -n 30
sudo rmmod simple
sudo dmesg | tail -n 30
```

### /proc/jiffies

```bash
sudo insmod jiffies_module.ko
cat /proc/jiffies
cat /proc/jiffies
sudo rmmod jiffies_module
test ! -e /proc/jiffies && echo "Removed successfully"
```

### /proc/seconds

```bash
sudo insmod seconds_module.ko
cat /proc/seconds
sleep 3
cat /proc/seconds
sudo rmmod seconds_module
test ! -e /proc/seconds && echo "Removed successfully"
```

## Sample output

`simple` module (`HZ = 250`):

```
simple: Loading Kernel Module
simple: GOLDEN_RATIO_PRIME = 7046029254386353131
simple: jiffies at load = 4294956765
simple: HZ = 250
...
simple: GCD(3300, 24) = 12
simple: jiffies at unload = 4294993942
simple: elapsed jiffies = 37177
simple: elapsed seconds = 148
simple: Removing Kernel Module
```

`/proc/jiffies`: two consecutive reads return increasing values, for example `4295023522` and then `4295025438`.

`/proc/seconds`: returns the whole seconds since the module was loaded, for example `25` and, a little later, `45`.

## Notes

- `make` may print a warning that the `pahole` version differs from the one used to build the kernel. It only affects BTF debug info and does not stop the build.
- Loading the modules logs `module verification failed ... tainting kernel`. This is expected for unsigned, out-of-tree modules.
- If loading fails with `Key was rejected by service`, Secure Boot is blocking unsigned modules. Disable it in the VM settings or sign the modules.
- If you get `Invalid module format`, rebuild against the headers that match `uname -r`.

## What I learned

- The structure of a kernel module (`module_init`, `module_exit`, license and author macros)
- Building modules with a Makefile and managing them with `insmod`, `rmmod`, `lsmod` and `dmesg`
- Using kernel-only values and functions such as `jiffies`, `HZ`, `GOLDEN_RATIO_PRIME` and `gcd()`
- Creating and removing `/proc` entries with `proc_create()`, `struct proc_ops` and `remove_proc_entry()`

## Author

Shaza Waleed

## License

The modules are licensed under GPL, as declared by `MODULE_LICENSE("GPL")` in the source files.
