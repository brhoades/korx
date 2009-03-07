#! /bin/bash
VERSION=$(svnversion)
DATA="GPL README armour/ vm/ scripts/ configs/ ui/*.* -x *.svn* *.xcf /*~ *.blend"
DATANAME=trepidus-r$VERSION
TEXTURES='emoticons/ icons/ models/ sound/ fonts/ gfx/ ui/assets/ -x *.svn* *.xcf /*~ *.blend'
TEXTUREPAK=trepidus-textures-r$VERSION

echo "*** DIFF AND COMPILE  ***"
make clean toolsclean
#./generate-diff.sh
#[ -e ./build.log ] && rm ./build.log
make 2>&1
#if [ "$?" -ne "0" ]
#then
#	rm current.patch
#	exit 1
#fi
#mv current.patch mgdev-r$VERSION.patch
# rm -v *.so
# cp build/release-linux-x86/base/*.so .
rm -rf vm/
mkdir vm
cp build/release-linux-x86/base/vm/*.qvm vm/
echo "*** CLEARING OLD PK3S ***"
rm -v *.pk3
echo "***  MAKING NEW PK3S  ***"
zip -rq $DATANAME.pk3 $DATA && echo made: \'$DATANAME.pk3\'
zip -rq $TEXTUREPAK.pk3 $TEXTURES && echo made: \'$TEXTUREPAK.pk3\'
#rm ~/.tremulous/trepidus/*.pk3
#cp trepidus-*.pk3 ~/.tremulous/trepidus

