#!/bin/sh
cd data/
num=`date +%Y%j%k%M%S`
echo $num
zip -9r ../korx_data_$num.pk3 * -x ".svn"
cd ..
zip -9r korx_ui_$num.pk3 ui scripts  -x ".svn"
cd build/releas*/base/
zip -9r ../../../korx_vm_$num.pk3 vm/ -x ".svn"
