#!/usr/bin/env bash
set -euo pipefail

# make_local_aarch64_toolchain.sh
# Create a reusable aarch64 toolchain tarball for cross-compiling to Raspberry Pi (aarch64).
# Usage: edit variables below, then run on your x86 workstation.

# === Configuration (edit as needed) ===
PI_USER="mseti"
PI_HOST="10.212.3.101"   # or IP address
OUTDIR="$HOME/aarch64_toolchain"   # output directory for the toolchain tarball

# ARM GNU Toolchain for an x86_64 build host targeting 64-bit Linux ARM.
ARM_GNU_TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu/12.3.rel1/binrel/arm-gnu-toolchain-12.3.rel1-x86_64-aarch64-none-linux-gnu.tar.xz"

# === End configuration ===

RSYNC_SSH="ssh -o ServerAliveInterval=30 -o ServerAliveCountMax=12 -o ConnectTimeout=15"
RSYNC_OPTIONS=(--archive --hard-links --copy-links --partial --append-verify --info=progress2 -e "$RSYNC_SSH")

mkdir -p "$OUTDIR"
cd "$OUTDIR"

echo "Output directory: $OUTDIR"

# 1) Download a valid ARM GNU toolchain archive.
if [ -f arm-gnu-toolchain.tar.xz ] && ! xz -t arm-gnu-toolchain.tar.xz; then
  echo "Removing invalid cached archive: arm-gnu-toolchain.tar.xz" >&2
  rm -f arm-gnu-toolchain.tar.xz
fi
if [ ! -f arm-gnu-toolchain.tar.xz ]; then
  echo "Downloading ARM GNU toolchain from: $ARM_GNU_TOOLCHAIN_URL"
  curl -L --fail --retry 3 -o arm-gnu-toolchain.tar.xz.partial "$ARM_GNU_TOOLCHAIN_URL"
  xz -t arm-gnu-toolchain.tar.xz.partial
  mv arm-gnu-toolchain.tar.xz.partial arm-gnu-toolchain.tar.xz
else
  echo "Found valid cached ARM GNU toolchain archive"
fi

# 2) Extract toolchain
rm -rf toolchain
mkdir -p toolchain
echo "Extracting ARM GNU toolchain to $OUTDIR/toolchain"
tar -xJf arm-gnu-toolchain.tar.xz -C toolchain --strip-components=1

# 3) Capture headers and development/runtime libraries from the Pi as a sysroot.
# Do not copy complete /lib or /usr/lib: those trees contain restricted CUPS,
# SSL, and systemd files unrelated to compilation.
mkdir -p sysroot/usr sysroot/lib/aarch64-linux-gnu sysroot/usr/lib/aarch64-linux-gnu

echo "Syncing sysroot files from Pi ($PI_USER@$PI_HOST); rerun this script to resume after a disconnect"
rsync "${RSYNC_OPTIONS[@]}" "$PI_USER@$PI_HOST:/usr/include/" sysroot/usr/include/
rm -rf sysroot/usr/include/aarch64-linux-gnu
rsync "${RSYNC_OPTIONS[@]}" \
  "$PI_USER@$PI_HOST:/usr/include/aarch64-linux-gnu/" sysroot/usr/include/aarch64-linux-gnu/
mkdir -p sysroot/lib
rsync "${RSYNC_OPTIONS[@]}" \
  "$PI_USER@$PI_HOST:/lib/ld-linux-aarch64.so.1" sysroot/lib/ld-linux-aarch64.so.1
rsync "${RSYNC_OPTIONS[@]}" \
  --exclude='libdrm_intel.so' --exclude='qt-default/' \
  --include='*/' --include='*.so' --include='*.so.*' --include='*.a' --include='crt*.o' --exclude='*' \
  "$PI_USER@$PI_HOST:/lib/aarch64-linux-gnu/" sysroot/lib/aarch64-linux-gnu/
rsync "${RSYNC_OPTIONS[@]}" \
  --exclude='libdrm_intel.so' --exclude='qt-default/' \
  --include='*/' --include='*.so' --include='*.so.*' --include='*.a' --include='crt*.o' --exclude='*' \
  "$PI_USER@$PI_HOST:/usr/lib/aarch64-linux-gnu/" sysroot/usr/lib/aarch64-linux-gnu/

# Optional: remove large unneeded directories
rm -rf sysroot/usr/share/doc sysroot/usr/share/man || true

# Debian multiarch headers expect compatibility links such as /usr/include/bits.
# Keep the sysroot self-contained by pointing those generic include paths at the
# AArch64 multiarch directory copied above.
for _d in bits gnu asm sys; do
  if [ ! -e "sysroot/usr/include/$_d" ] && [ -d "sysroot/usr/include/aarch64-linux-gnu/$_d" ]; then
    ln -s "aarch64-linux-gnu/$_d" "sysroot/usr/include/$_d"
  fi
done

if [ -d "sysroot/usr/include/aarch64-linux-gnu/sys" ]; then
  for _f in sysroot/usr/include/aarch64-linux-gnu/sys/*; do
    _base="$(basename "$_f")"
    if [ ! -e "sysroot/usr/include/sys/$_base" ]; then
      ln -s "../aarch64-linux-gnu/sys/$_base" "sysroot/usr/include/sys/$_base"
    fi
  done
fi

if [ ! -e "sysroot/usr/include/openssl/opensslconf.h" ] \
   && [ -e "sysroot/usr/include/aarch64-linux-gnu/openssl/opensslconf.h" ]; then
  ln -s "../aarch64-linux-gnu/openssl/opensslconf.h" \
    "sysroot/usr/include/openssl/opensslconf.h"
fi

if [ ! -e "sysroot/lib/aarch64-linux-gnu/libmvec.so.1" ] \
   && [ -e "sysroot/usr/lib/aarch64-linux-gnu/libmvec.so.1" ]; then
  ln -s "../../usr/lib/aarch64-linux-gnu/libmvec.so.1" \
    "sysroot/lib/aarch64-linux-gnu/libmvec.so.1"
fi

# 4) Create small wrapper bin that points to Linaro cross tools and sets --sysroot
rm -rf local
mkdir -p local/bin
cat > local/bin/aarch64-linux-gnu-gcc <<'EOF'
#!/usr/bin/env bash
ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
TOOLCHAIN_DIR="$ROOT_DIR/toolchain"
SYSROOT_DIR="$ROOT_DIR/sysroot"
exec "$TOOLCHAIN_DIR/bin/aarch64-none-linux-gnu-gcc" --sysroot="$SYSROOT_DIR" \
  -isystem "$SYSROOT_DIR/usr/include/aarch64-linux-gnu" \
  -B "$SYSROOT_DIR/usr/lib/aarch64-linux-gnu" \
  -L "$SYSROOT_DIR/usr/lib/aarch64-linux-gnu" "$@"
EOF
chmod +x local/bin/aarch64-linux-gnu-gcc

cat > local/bin/aarch64-linux-gnu-g++ <<'EOF'
#!/usr/bin/env bash
ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
TOOLCHAIN_DIR="$ROOT_DIR/toolchain"
SYSROOT_DIR="$ROOT_DIR/sysroot"
exec "$TOOLCHAIN_DIR/bin/aarch64-none-linux-gnu-g++" --sysroot="$SYSROOT_DIR" \
  -isystem "$SYSROOT_DIR/usr/include/aarch64-linux-gnu" \
  -B "$SYSROOT_DIR/usr/lib/aarch64-linux-gnu" \
  -L "$SYSROOT_DIR/usr/lib/aarch64-linux-gnu" "$@"
EOF
chmod +x local/bin/aarch64-linux-gnu-g++

# 5) Package the toolchain
TOOL_TAR="aarch64-local-toolchain.tar.gz"
rm -f "$TOOL_TAR"
echo "Packaging toolchain into: $OUTDIR/$TOOL_TAR"
tar -czf "$TOOL_TAR" toolchain sysroot local

echo "Toolchain created: $OUTDIR/$TOOL_TAR"

cat <<EOF
Next steps / quick test:
  cd $OUTDIR
  tar -xzf $TOOL_TAR
  export PATH=\$PWD/local/bin:\$PATH
  aarch64-linux-gnu-gcc --version

Quick compile test:
  cat > hello.c <<'C'\n#include <stdio.h>\nint main(){ printf("hello aarch64\n"); return 0; }\nC
  aarch64-linux-gnu-gcc -o hello_arm64 hello.c
  file hello_arm64
  # scp hello_arm64 to Raspberry Pi and run it there to confirm
EOF

exit 0
