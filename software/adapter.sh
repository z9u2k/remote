#!/bin/bash

# constants
CONFIG_FILE="`dirname $0`/adapter.conf"

# configurable values
ENABLED="true"
HOST="localhost"
PORT="8765"

# invoked from udev?
if [ -z "${DEVNAME}" -o -z "${ACTION}" ]; then
    echo "$0 must be invoked from within a udev rule" 1>&2
    exit 1
fi

# read config
if [ -r ${CONFIG_FILE} ]; then
    . ${CONFIG_FILE}
fi

# non-configurable values
SYSLOG_TAG="LIRC-ADAPTER[$$]"
SYSLOG_FACILITY="user"

LOCK_DIR="/var/lock/lirc-adapter"
LOCK_FILE="${LOCK_DIR}/`basename ${DEVNAME}`.lock"

function log {
    logger \
        --tag "${SYSLOG_TAG}" \
        --priority "${SYSLOG_FACILITY}.$1" \
        -- \
        "$2"
}

function loge {
    log "err" "$1 $2 $3"
}

function logw {
    log "warning" "$1 $2 $3"
}

function logn {
    log "notice" "$1 $2 $3"
}

function logi {
    log "info" "$1 $2 $3"
}

# adapter enabled?
if [ "${ENABLED}" != "true" ]; then
    CONF_ABS="`readlink -f ${CONFIG_FILE}`"
    logn "LIRC UDB-to-UDP adapter disabled. Did you enable it in ${CONF_ABS} ?"
    exit 0
fi

mkdir -p "${LOCK_DIR}"

logi "Started with action: '${ACTION}' on device: '${DEVNAME}'"
if [ "${ACTION}" = "add" -o "${ACTION}" = "change" ]; then
    if [ -r "${DEVNAME}" ]; then

        if [ -e "${LOCK_FILE}" ]; then
            logn "Found lock file in '${LOCK_FILE}', either adapter already" \
                "started for '${DEVNAME}', or the lock is stale and should" \
                "be deleted"
            exit 0
        fi

        logi "Configuring ${DEVNAME} ..."
        STTY_OUTPUT="`stty --file=${DEVNAME} 115200 raw 2>&1`"
        if [ $? -ne 0 ]; then
                loge "stty on ${DEVNAME} failed"
                loge "${STTY_OUTPUT}"
                exit 1
        fi

        logi "Starting 'socat' on: ${DEVNAME}"
        socat \
            -ly \
            -lp${SYSLOG_TAG} \
            -u \
            -L${LOCK_FILE} \
            OPEN:${DEVNAME},creat=0,rdonly=1 \
            UDP4-SENDTO:${HOST}:${PORT},sndbuf=2 > /dev/null 2>&1 &

        logi "Acquired lock file '${LOCK_FILE}'"
    else
        logn "Can't open '${DEVNAME}' for reading"
        exit 0
    fi
else
    logn "Unknown action '${ACTION}', known actions are 'add' and 'change'."
    exit 0
fi
