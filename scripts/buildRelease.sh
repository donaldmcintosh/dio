#!/bin/bash
# 
# Written by Donald McIntosh, Open Technology
# Date 12/03/15
#

while getopts "v:s:" o; do
    case "${o}" in
        v)
            VERSION=${OPTARG}
            ;;
        s)
            SRC_ROOT=${OPTARG}
            ;;
    esac
done

if [[ -z "${VERSION}" ]] ; then
    echo "Must provide a VERSION"
    exit -1
elif [[ -z "${SRC_ROOT}" ]] ; then
    echo "Must provide a SRC_ROOT"
    exit -1
fi

REL_ROOT=${SRC_ROOT}/rel
SRC=${SRC_ROOT}/src
REL_NAME=dio-${VERSION}
VERSION_SRC=${REL_ROOT}/${REL_NAME}
TARFILE=${REL_ROOT}/${REL_NAME}.tar
GZFILE=${TARFILE}.gz

echo "Linking ${SRC} to ${VERSION_SRC}"
rm -f ${VERSION_SRC}
ln -s ${SRC} ${VERSION_SRC}
echo "Archiving ${VERSION_SRC} to ${TARFILE}"
tar cvf ${TARFILE} --dereference --directory ${REL_ROOT} ${REL_NAME}
echo "Compressing ${TARFILE}"
gzip -f ${TARFILE}
echo "Created new Release file ${GZFILE}"
ls -l ${GZFILE}
