FROM mcr.microsoft.com/devcontainers/base:ubuntu-24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        ca-certificates-java \
        clang \
        cmake \
        curl \
        g++ \
        gcc \
        gdb \
        git \
        jq \
        lld \
        make \
        openjdk-17-jre-headless \
        openssl \
        pkg-config \
        protobuf-compiler \
        python3 \
        unzip \
        zip \
    && rm -rf /var/lib/apt/lists/*

COPY install_bosch_certs.py /tmp/install_bosch_certs.py

RUN python3 /tmp/install_bosch_certs.py \
        || echo "Bosch CA certificate installation skipped; install corporate CA certificates under /usr/local/share/ca-certificates and run update-ca-certificates if Bazel TLS downloads fail." >&2 \
    && update-ca-certificates \
    && printf '%s\n' \
        'startup --host_jvm_args=-Djavax.net.ssl.trustStore=/etc/ssl/certs/java/cacerts' \
        'startup --host_jvm_args=-Djavax.net.ssl.trustStorePassword=changeit' \
        > /etc/bazel.bazelrc

RUN case "$(uname -m)" in \
        x86_64) bazelisk_arch=amd64 ;; \
        aarch64|arm64) bazelisk_arch=arm64 ;; \
        *) echo "Unsupported Bazelisk architecture: $(uname -m)" >&2; exit 1 ;; \
    esac \
    && curl --fail --location --show-error --silent \
        --retry 5 --retry-all-errors --retry-delay 2 --connect-timeout 30 \
        "https://github.com/bazelbuild/bazelisk/releases/latest/download/bazelisk-linux-${bazelisk_arch}" \
        -o /usr/local/bin/bazelisk \
    && chmod +x /usr/local/bin/bazelisk \
    && ln -sf /usr/local/bin/bazelisk /usr/local/bin/bazel

USER vscode

RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs \
        | sh -s -- -y --profile default --default-toolchain stable \
    && /home/vscode/.cargo/bin/rustup component add rustfmt clippy

ENV PATH="/home/vscode/.cargo/bin:${PATH}"
