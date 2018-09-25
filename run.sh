#!/bin/bash
DIR=`dirname $0`
qemu-system-x86_64 -hda $DIR/Image/x64BareBonesImage.qcow2 -m 512 -soundhw pcspk
