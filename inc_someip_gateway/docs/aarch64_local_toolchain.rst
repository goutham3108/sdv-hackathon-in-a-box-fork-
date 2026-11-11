Local AArch64 Toolchain Setup
=============================

This document explains the prerequisites and connection setup for
``tools/make_local_aarch64_toolchain.sh``.

The script is used on an x86_64 development machine to assemble a reusable
cross-compilation toolchain for Raspberry Pi class AArch64 targets. It downloads
the ARM GNU toolchain, copies the target sysroot from a live Raspberry Pi or a
compatible QEMU-based AArch64 VM, and packages the result into a local tarball.

What the script needs
---------------------

Host-side prerequisites:

- ``bash``
- ``curl`` for downloading the ARM GNU toolchain archive
- ``xz`` for validating and extracting the archive
- ``tar`` for unpacking and packaging the toolchain
- ``rsync`` for copying the sysroot from the target device
- ``ssh`` or ``openssh-client`` for remote access through rsync
- standard GNU core utilities such as ``mkdir``, ``rm``, and ``cat``

Target-side prerequisites:

- a reachable AArch64 Linux target
- SSH server enabled on the target
- ``rsync`` available on the target
- a target filesystem that matches the runtime you want to build against

Recommended package installation
---------------------------------

On Debian or Ubuntu style development hosts, install the following packages:

.. code-block:: bash

   sudo apt update
   sudo apt install -y bash curl xz-utils tar rsync openssh-client

If your host already has these tools, no additional host packages are required
by the script itself.

SSH connection setup
--------------------

The script uses rsync over SSH. That means the target must be reachable as
``PI_USER@PI_HOST`` and must allow SSH logins.

Edit the variables at the top of the script:

.. code-block:: bash

   PI_USER="your-user"
   PI_HOST="192.168.x.x"

For unattended use, set up SSH keys from the host to the target:

.. code-block:: bash

   ssh-keygen -t ed25519
   ssh-copy-id your-user@192.168.x.x

After that, verify the login before running the script:

.. code-block:: bash

   ssh your-user@192.168.x.x

If the login works, the rsync steps in the script should also work.

What the script copies
----------------------

The script does not copy the full target root filesystem. It only gathers the
parts needed for cross-compilation:

- ``/usr/include``
- ``/usr/include/aarch64-linux-gnu``
- ``/lib/ld-linux-aarch64.so.1``
- ``/lib/aarch64-linux-gnu`` libraries and startup objects
- ``/usr/lib/aarch64-linux-gnu`` libraries and startup objects

This creates a local sysroot that matches the target ABI closely enough for
building binaries on the development host.

How to run it
-------------

1. Open the script and set ``PI_USER``, ``PI_HOST``, and ``OUTDIR``.
2. Make sure the host prerequisites are installed.
3. Make sure the target is reachable over SSH.
4. Run the script from the repository root or from any shell on the host.
5. Use the packaged ``aarch64-local-toolchain.tar.gz`` output for builds.

Quick verification
------------------

After extraction, the wrapper binaries should be available on ``PATH``:

.. code-block:: bash

   export PATH="$PWD/local/bin:$PATH"
   aarch64-linux-gnu-gcc --version

You can also compile a small test program and copy it to the target to confirm
that the generated binaries run on the same AArch64 environment.

QEMU Raspberry Pi VM support
----------------------------
(need to verify)
The script can also work with a QEMU-based Raspberry Pi VM if the VM behaves like
an AArch64 Linux target and exposes SSH plus rsync access. In that case, set
``PI_HOST`` to the VM address and keep the same host-side prerequisites.

The script will not work if the VM is not AArch64, does not expose SSH, or does
not provide compatible libraries for the sysroot capture step.
