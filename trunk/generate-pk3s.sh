#!/bin/sh
cd data/
3 = "2"
zip -9r ../korx_data_2.pk3 * -x ".svn"
cd ../ui/
zip -9r ../korx_ui_2.pk3 * -x ".svn"
cd ../build/releas*/base/
zip -9r ../../../korx_vm_2.pk3 vm/ -x ".svn"
