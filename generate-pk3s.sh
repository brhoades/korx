#!/bin/sh
cd $1
cd data/
zip -9r ../korx_data_$3.pk3 * -x *.svn* *.xcf /*~ *.blend*
cd ../ui/
zip -9r ../korx_ui_$3.pk3 * -x *.svn* *.xcf /*~ *.blend*
cd ..
zip -9r korx_vm_$3.pk3 vm/ -x *.svn* *.xcf /*~ *.blend*
