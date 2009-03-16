#!/bin/sh
#cd $1
cd data/
<<<<<<< .mine
zip -9r ../korx_data_$1.pk3 * -x *.svn* *.xcf /*~ *.blend
=======
zip -9r ../korx_data_$3.pk3 ../GPL ../CC ../COPYING * -x *.svn* *.xcf /*~ *.blend*
cd ../ui/
zip -9r ../korx_ui_$3.pk3 * -x *.svn* *.xcf /*~ *.blend*
>>>>>>> .r388
cd ..
<<<<<<< .mine
zip -9r korx_ui_$1.pk3 ui/ -x *.svn* *.xcf /*~ *.blend
#zip -9r ../korx_ui_$1.pk3 * -x *.svn* *.xcf /*~ *.blend
zip -9r korx_vm_$1.pk3 vm/ -x *.svn* *.xcf /*~ *.blend
=======
zip -9r korx_vm_$3.pk3 vm/ -x *.svn* *.xcf /*~ *.blend*
>>>>>>> .r388
