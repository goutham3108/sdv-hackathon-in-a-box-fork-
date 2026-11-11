# Cross-Compile Ankaios for AArch64

This guide builds the Ankaios server, agent, and CLI for the
`aarch64-unknown-linux-musl` target.

## Get the source

Clone the Ankaios repository and enter the checkout:

```sh
git clone https://github.com/eclipse-ankaios/ankaios.git
cd ankaios
```

## Apply the Cross configuration

Apply `Cross.toml.patch` from the repository root:

```sh
git apply Cross.toml.patch
```

## Build

Run the following command from the repository root:

```sh
cross build --release --target aarch64-unknown-linux-musl \
    -p ank-server -p ank-agent -p ank
```

## Build output

The binaries and related build artifacts are written to:

```text
target/
└── aarch64-unknown-linux-musl/
    └── release/
        ├── ank              # CLI
        ├── ank-agent        # agent
        ├── ank-server       # server
        ├── deps/            # intermediate .rlib and object files
        ├── build/           # build.rs outputs, including generated proto code
        └── incremental/     # incremental compilation cache
```