#!/usr/bin/env python3

import hashlib
import subprocess
import urllib.request
from pathlib import Path


class Cert:
    def __init__(
        self,
        url: str,
        filename: str,
        fingerprint_sha1: str,
        fingerprint_sha256: str,
        filehash_sha256: str,
    ) -> None:
        self.url = url
        self.filename = filename
        self.fingerprint_sha1 = fingerprint_sha1.lower()
        self.fingerprint_sha256 = fingerprint_sha256.lower()
        self.filehash_sha256 = filehash_sha256.lower()

    def download_to(self, path: Path) -> None:
        with urllib.request.urlopen(self.url, timeout=60) as response:
            path.write_bytes(response.read())
        self.verify(path)

    def verify(self, path: Path) -> None:
        raw = path.read_bytes()
        sha256 = hashlib.sha256(raw).hexdigest()
        if sha256 != self.filehash_sha256:
            raise ValueError(
                f"Hash mismatch for {self.filename}: expected {self.filehash_sha256}, got {sha256}"
            )

        check_fingerprint(path, "sha1", self.fingerprint_sha1)
        check_fingerprint(path, "sha256", self.fingerprint_sha256)


def check_fingerprint(cert_path: Path, digest: str, expected: str) -> None:
    result = subprocess.run(
        ["openssl", "x509", "-in", str(cert_path), "-noout", f"-{digest}", "-fingerprint"],
        check=True,
        capture_output=True,
        text=True,
    )
    actual = result.stdout.strip().split("=")[1].replace(":", "").lower()
    if actual != expected:
        raise ValueError(
            f"{digest.upper()} fingerprint mismatch for {cert_path.name}: expected {expected}, got {actual}"
        )


CERTIFICATES = [
    Cert(
        url="https://inside-docupedia.bosch.com/confluence2/download/attachments/314572940/rb_rootca_ecc_g01.crt?version=2&modificationDate=1741273682000&api=v2",
        filename="rb_rootca_ecc_g01.crt",
        fingerprint_sha1="e2193de57aab4c0c9e9bbcbe212e28360cf205cb",
        fingerprint_sha256="80630d1ffdac69622b039d490e910f5e7130f54606a6d73c9eed9522d06ef869",
        filehash_sha256="77A022260927DB365D565EF58FA9AF78D3C864DBFAC68400E3896FEBE8D0CCC8",
    ),
    Cert(
        url="https://inside-docupedia.bosch.com/confluence2/download/attachments/314572940/rb_rootca_rsa_g01.crt?version=1&modificationDate=1741339361000&api=v2",
        filename="rb_rootca_rsa_g01.crt",
        fingerprint_sha1="8e468cb78e150cdb2473536693e14732b5075919",
        fingerprint_sha256="befea4b1f8754f18c449a0d550ffe76daba609f410b65fed1bb4064aa6d9a306",
        filehash_sha256="46D26CE24971172304C94BD5D5308866DC394391EDEF3AF55D20282E8A2F1AEC",
    ),
]


def main() -> None:
    install_dir = Path("/usr/local/share/ca-certificates/bosch")
    install_dir.mkdir(parents=True, exist_ok=True)

    for cert in CERTIFICATES:
        target = install_dir / cert.filename
        cert.download_to(target)
        print(f"Installed {target}")


if __name__ == "__main__":
    main()
