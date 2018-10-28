#!/bin/bash

function conv_coding()
{
    `rm $1/*_UTF-8`
    for file in `ls $1`
    do
        file_path=$1"/"$file
        echo $file_path
        type=`file $file_path | awk '{print $2}'`
        if [ -f $file_path -a $type != "UTF-8" ]
        then
            iconv -f GBK -t UTF-8 $file_path -o $file_path"_UTF-8"
        fi
    done
}

INIT_PATH="./dictionary/dict"
conv_coding $INIT_PATH
