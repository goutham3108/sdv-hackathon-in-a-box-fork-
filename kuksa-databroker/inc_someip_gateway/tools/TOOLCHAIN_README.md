Local aarch64 toolchain (for Raspberry Pi)
===========================================

This directory contains a helper script to create a reusable aarch64 cross-toolchain tarball.

Files
- `make_local_aarch64_toolchain.sh` — downloads an ARM GNU toolchain, rsyncs a minimal sysroot from a Raspberry Pi, and packages them into `aarch64-local-toolchain.tar.gz` (located in the script's output directory).

Overview
1. Configure the variables at the top of `make_local_aarch64_toolchain.sh` (set `PI_USER`, `PI_HOST`, and optionally `LINARO_URL` and `OUTDIR`).
2. Run the script on your x86 workstation. It will require `rsync` and SSH access to the Pi.
3. The script produces `aarch64-local-toolchain.tar.gz` which contains:
   - `toolchain/` — extracted Linaro cross compiler
   - `sysroot/` — captured headers and libs from the Pi
   - `local/bin/` — small wrapper scripts (`aarch64-linux-gnu-gcc`, `aarch64-linux-gnu-g++`) which invoke the toolchain with the sysroot and Debian ARM64-specific header path

The toolchain is installed on the x86 build workstation. Do not deploy it to the Pi to run the gateway. Use it to compile aarch64 artifacts, then deploy only those artifacts and their runtime configuration to the Pi.

Quick test

```bash
cd /path/to/toolchain/output
tar -xzf aarch64-local-toolchain.tar.gz
export PATH=$PWD/local/bin:$PATH
aarch64-linux-gnu-gcc --version
# compile a tiny test
cat > hello.c <<'C'
#include <stdio.h>
int main(){ printf("hello aarch64\n"); return 0; }
C
aarch64-linux-gnu-gcc -o hello_arm64 hello.c
file hello_arm64
# copy to Pi and run to confirm
scp hello_arm64 pi@<PI_IP>:/tmp/
ssh pi@<PI_IP> /tmp/hello_arm64
```

Using with Bazel

- Easiest: place the tarball in a directory and run Bazel with `--distdir=/path/to/dir` so Bazel can use local archives instead of downloading remote toolchains.
- Advanced: create a `local_repository` or `cc_toolchain` BUILD wrapper and register the toolchain with Bazel's `register_toolchains(...)`. I can generate these Bazel files if you want to integrate the toolchain directly into the repo.

Notes
- The script copies public and ARM64-specific headers, shared/static libraries, C runtime startup objects (`crt*.o`), and the ARM64 dynamic loader from the Debian ARM64 multiarch directories. It intentionally does not copy all of `/lib` or `/usr/lib`, which can contain protected service files unrelated to compilation.
- The sysroot copy uses SSH keepalives and resumable rsync transfers. If Wi-Fi or SSH disconnects, rerun the script; completed files and partial large files are retained and reused.
- Ensure the Pi and workstation have compatible OS versions for the sysroot snapshot.
- If your Pi uses a 32-bit OS, this toolchain will not produce compatible binaries (use aarch32 toolchain instead).
