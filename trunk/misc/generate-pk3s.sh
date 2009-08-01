#!/bin/sh
#korxdir="korx"
#echo "`sudo su tremulous`"
cd /home/tremulous/korx/
echo "Updating from SVN..."
echo "`svn cleanup`"
echo "`svn update`"
#echo "`hg pull -u`"
echo "Compiling..."
echo "`make`"
#`rm /home/tremulous/korx/*.pk3`
num="`svn info | awk '/Revision:/ {print $2}'`"
#num="`svn id -n`"
num=$(( $num+97 ))
cd data/
#num="`date +%Y%j%M%S`"
echo "Generating package set "$num
echo "Outputting recent changes to ui/help.txt..."
#`echo "\`svn log /home/tremulous/korx/ -l5 -v\`" > /home/tremulous/korx/ui/svnlog.txt`
`php /home/tremulous/updatesvnlog.php`
echo "Generating models pk3..."
zip -9rq ../korx_models_$num.pk3 models -x *.svn* *.xcf* *.blend*
echo "Generating sounds pk3..."
zip -9rq ../korx_sounds_$num.pk3 sound voice -x *.svn* *.xcf* *.blend*
echo "Generating gfx pk3..."
zip -9rq ../korx_gfx_$num.pk3 gfx -x *.svn* *.xcf* *.blend*
echo "Generating emoticons pk3..."
zip -9rq ../korx_emoticons_$num.pk3 emoticons -x *.svn* *.xcf* *.blend*
echo "Generating miscellaneous pk3..."
zip -9rq ../korx_misc_$num.pk3 ../GPL ../COPYING ../CC configs armour fonts icons scripts -x *.svn* *.xcf* *.blend*
cd ..
echo "Generating UI pk3..."
zip -9rq korx_ui_$num.pk3 ui scripts  -x *.svn* *.xcf* *.blend*
cd build/releas*/base/
echo "Generating vm pk3..."
zip -9rq ../../../korx_vm_$num.pk3 vm/ -x *.svn* *.xcf* game.qvm
echo "Generating data_static pk3..."
cd /home/tremulous/korx/data_static
zip -9rq ../korx_data_static_$num.pk3 * ../ui/he -x *.svn* *.xcf*

echo "Moving pk3s and qvms..."

echo "`sudo cp -v /home/tremulous/korx/build/re*/base/vm/game.qvm /usr/share/tremulous/korx/vm/game.qvm`"
echo "`sudo mv -v /home/tremulous/korx/*.pk3 /usr/share/tremulous/korx/`"

echo "Compilation is DONE!"
