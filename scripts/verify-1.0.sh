#!/bin/sh
set -eu

slim_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

"$slim_root/scripts/verify.sh"
"$slim_root/scripts/verify-release.sh"
(
    cd "$slim_root/website"
    npm test
)

echo "SLIM 1.0 verification: repository, release, and website gates passed"
