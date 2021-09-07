#!/bin/bash

# set current directory to where the script was called from
version=0.1.3
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"

echo "Copy the key to the clipboard and then"
echo "Press any key to continue"
echo
read -n 1 -s -r -p ""

clipboard="$(pbpaste)"

xcrun altool --notarization-info $clipboard -u ${APPLE_ID_EMAIL} -p ${APPLE_APP_PASSWORD}
