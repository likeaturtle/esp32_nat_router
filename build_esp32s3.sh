#!/usr/bin/env bash
#
# Build the ESP32-S3 firmware with the project's ESP-IDF v5.5.4 toolchain.
#
#   ./build_esp32s3.sh                     # build (first run configures the target)
#   ./build_esp32s3.sh --clean             # fullclean, then build
#   ./build_esp32s3.sh --flash             # build, then flash (IDF auto-detects the port)
#   ./build_esp32s3.sh -p /dev/cu.usbmodem1101 --monitor
#   ./build_esp32s3.sh --dry-run           # print the commands without running them
#
# Environment overrides:
#   IDF_TARGET_VERSION  default 5.5.4 (also picks activate_idf_v$IDF_TARGET_VERSION.sh)
#   IDF_ACTIVATE        path to an EIM activation script, if it is not installed
#                       in the default ~/.espressif/tools location
#   BUILD_DIR     default build_esp32s3
#   SDKCONFIG     default sdkconfig.esp32s3
#
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
SELF="$ROOT_DIR/$(basename -- "${BASH_SOURCE[0]}")"
SCRIPT_ARGS=("$@")
cd "$ROOT_DIR"

IDF_TARGET_VERSION="${IDF_TARGET_VERSION:-5.5.4}"
BUILD_DIR="${BUILD_DIR:-build_esp32s3}"
SDKCONFIG="${SDKCONFIG:-sdkconfig.esp32s3}"
SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.esp32s3"
APP_BIN="$BUILD_DIR/esp32_nat_router.bin"
OTA_SLOT_BYTES=$((1536 * 1024))

CLEAN=0
FLASH=0
MONITOR=0
DRY_RUN=0
PORT=""

die() {
    printf 'error: %s\n' "$*" >&2
    exit 1
}

note() {
    printf '==> %s\n' "$*"
}

usage() {
    cat <<EOF
Build the ESP32-S3 firmware with ESP-IDF v${IDF_TARGET_VERSION}.

Usage: $(basename "$0") [options]

Options:
  -c, --clean        fullclean the build directory before building
  -f, --flash        flash the built firmware (implies build)
  -m, --monitor      flash and open the serial monitor (implies --flash)
  -p, --port PORT    serial port for --flash/--monitor, e.g. /dev/cu.usbmodem1101
  -n, --dry-run      print the idf.py commands without running them
  -h, --help         show this help

Every run except --help activates ESP-IDF v5.5.4 automatically, so there is no
need to source the activation script first.

Environment:
  IDF_TARGET_VERSION ESP-IDF version to activate (default: ${IDF_TARGET_VERSION})
  IDF_ACTIVATE       explicit path to activate_idf_v${IDF_TARGET_VERSION}.sh
  BUILD_DIR          build directory (default: ${BUILD_DIR})
  SDKCONFIG          generated sdkconfig (default: ${SDKCONFIG})

Outputs:
  ${APP_BIN}
  ${BUILD_DIR}/bootloader/bootloader.bin
  ${BUILD_DIR}/partition_table/partition-table.bin
  ${BUILD_DIR}/ota_data_initial.bin
EOF
}

while (($#)); do
    case "$1" in
        -c|--clean) CLEAN=1 ;;
        -f|--flash) FLASH=1 ;;
        -m|--monitor) MONITOR=1; FLASH=1 ;;
        -p|--port)
            [[ $# -ge 2 ]] || die "--port needs a value"
            PORT="$2"
            shift
            ;;
        -n|--dry-run) DRY_RUN=1 ;;
        -h|--help) usage; exit 0 ;;
        *) die "unknown option: $1 (try --help)" ;;
    esac
    shift
done

idf_version_line() {
    idf.py --version 2>/dev/null | tail -n 1 || true
}

activate_idf() {
    local script=""
    local candidates=()

    if command -v idf.py >/dev/null 2>&1 &&
       [[ "$(idf_version_line)" == *"v${IDF_TARGET_VERSION}"* ]]; then
        return 0
    fi

    if [[ "${_S3_BUILD_ACTIVATED:-}" == "1" ]]; then
        die "activation finished but idf.py reports '$(idf_version_line)', expected v${IDF_TARGET_VERSION}"
    fi

    if [[ -n "${IDF_ACTIVATE:-}" ]]; then
        candidates+=("$IDF_ACTIVATE")
    fi
    candidates+=("$HOME/.espressif/tools/activate_idf_v${IDF_TARGET_VERSION}.sh")

    for script in "${candidates[@]}"; do
        [[ -f "$script" ]] || continue
        note "Activating ESP-IDF v${IDF_TARGET_VERSION} from $script"
        # EIM activation scripts only accept being sourced by an interactive
        # shell: they look at $0 and refuse to run from another script. Re-run
        # this script in a child bash whose $0 is "bash", source the activation
        # there, then hand over. stdout is muted (the script is very chatty),
        # stderr is kept so real activation errors stay visible.
        CHILD_ARGS=("$script" "$SELF")
        # Bash 3.2 (macOS /bin/bash) treats "${empty[@]}" as unbound under -u.
        if ((${#SCRIPT_ARGS[@]})); then
            CHILD_ARGS+=("${SCRIPT_ARGS[@]}")
        fi
        exec bash -c '
            source "$1" >/dev/null
            shift
            export _S3_BUILD_ACTIVATED=1
            exec "$@"
        ' bash "${CHILD_ARGS[@]}"
    done

    die "ESP-IDF v${IDF_TARGET_VERSION} is not available.
     Install it with EIM, or set IDF_ACTIVATE=/path/to/activate_idf_v${IDF_TARGET_VERSION}.sh.
     A shell with another IDF version active is not enough; start a clean shell
     and source the v${IDF_TARGET_VERSION} activation script first."
}

[[ -f sdkconfig.defaults ]] || die "sdkconfig.defaults not found; run this script from the repository"
[[ -f sdkconfig.defaults.esp32s3 ]] || die "sdkconfig.defaults.esp32s3 not found"

activate_idf
VER_LINE="$(idf_version_line)"
[[ "$VER_LINE" == *"v${IDF_TARGET_VERSION}"* ]] || die "expected ESP-IDF v${IDF_TARGET_VERSION}, got: ${VER_LINE:-unknown}"
note "Using $VER_LINE ($(command -v idf.py))"

IDF_ARGS=(
    -B "$BUILD_DIR"
    -D "SDKCONFIG=$SDKCONFIG"
    -D "SDKCONFIG_DEFAULTS=$SDKCONFIG_DEFAULTS"
    -D "IDF_TARGET=esp32s3"
)

run() {
    if [[ "$DRY_RUN" == 1 ]]; then
        printf '    '
        printf '%q ' "$@"
        printf '\n'
    else
        "$@"
    fi
}

# A build directory configured for another chip makes CMake fail late and
# confusingly; catch it before doing anything.
if [[ -f "$BUILD_DIR/CMakeCache.txt" ]]; then
    CACHED_TARGET="$(sed -n 's/^IDF_TARGET:STRING=//p' "$BUILD_DIR/CMakeCache.txt" | head -n 1)"
    if [[ -n "$CACHED_TARGET" && "$CACHED_TARGET" != "esp32s3" ]]; then
        [[ "$CLEAN" == 1 ]] || die "$BUILD_DIR is configured for '$CACHED_TARGET'; rerun with --clean"
    fi
fi

if [[ "$CLEAN" == 1 ]]; then
    if [[ -f "$BUILD_DIR/CMakeCache.txt" ]]; then
        note "Cleaning $BUILD_DIR"
        run idf.py "${IDF_ARGS[@]}" fullclean
    else
        note "Nothing to clean: $BUILD_DIR is not configured yet"
    fi
fi

note "Building ESP32-S3 firmware"
if ! run idf.py "${IDF_ARGS[@]}" build; then
    cat >&2 <<EOF

Build failed. If CMake stopped while fetching esp_wireguard, the component
manager could not reach GitHub. See README.md (ESP32-S3 section) and retry with
a proxy, for example:

  export https_proxy=http://127.0.0.1:7890 http_proxy=http://127.0.0.1:7890
EOF
    exit 1
fi

if [[ "$DRY_RUN" == 0 ]]; then
    if [[ -f "$APP_BIN" ]]; then
        APP_SIZE="$(wc -c < "$APP_BIN" | tr -d ' ')"
        APP_PERCENT=$((APP_SIZE * 100 / OTA_SLOT_BYTES))
        note "Application: $APP_BIN (${APP_SIZE} bytes, ${APP_PERCENT}% of the 1536 KiB ota_0 slot)"
        if ((APP_PERCENT >= 95)); then
            printf 'warning: only %d%% of ota_0 is left; check idf.py size-components\n' \
                $((100 - APP_PERCENT)) >&2
        fi
    else
        printf 'warning: %s was not produced\n' "$APP_BIN" >&2
    fi
    note "Artifacts: $BUILD_DIR/{bootloader/bootloader.bin,partition_table/partition-table.bin,ota_data_initial.bin}"
fi

if [[ "$FLASH" == 1 ]]; then
    FLASH_ARGS=("${IDF_ARGS[@]}")
    if [[ -n "$PORT" ]]; then
        FLASH_ARGS+=(-p "$PORT")
    fi
    note "Flashing${PORT:+ to $PORT}"
    run idf.py "${FLASH_ARGS[@]}" flash
fi

if [[ "$MONITOR" == 1 ]]; then
    MONITOR_ARGS=("${IDF_ARGS[@]}")
    if [[ -n "$PORT" ]]; then
        MONITOR_ARGS+=(-p "$PORT")
    fi
    note "Opening serial monitor (Ctrl+] to exit)"
    run idf.py "${MONITOR_ARGS[@]}" monitor
fi

if [[ "$FLASH" == 0 && "$DRY_RUN" == 0 ]]; then
    printf '\nFlash with:\n  ./%s%s --flash\n' \
        "$(basename "$0")" "${PORT:+ -p $PORT}"
fi
