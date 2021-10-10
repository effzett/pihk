#!//bin/bash
# wird aufgerufen mit bumpversion.sh <x.x.x>
# z.B. bumpversion.sh 2.0.5
#
# Originaldateien werden mit .oldversion gekennzeichnet

# Script verändert per sed folgende Dateien
texfile1="./documentation/pihk3.tex"
cppfile1="./mainwindow.cpp"
nv=$1
date=$(date +%d.%m.%Y)

#Folgende Zeilen werden verändert:
# cppfile1:
#    const QString appversion="2.10.10";
#    const QString appdate="26.06.2016";
# texfile1:
#\newcommand{\vnr}{2.10.10}


if [ -w "$texfile1" ] && [ -w "$cppfile1" ];
	then
		echo "...bumping version to $nv..."
		/usr/bin/sed -E -i.oldversion "s/(^.newcommand..vnr..)[0-9]+\.[0-9]+\.[0-9]+/\1$nv/" "$texfile1"
		/usr/bin/sed -E -i.oldversion "s/(const.+QString.+appversion=.)[0-9]+\.[0-9]+\.[0-9]+/\1$nv/" "$cppfile1"
		/usr/bin/sed -E -i.oldversion "s/(const.+QString.+appdate=.)[0-9]+\.[0-9]+\.[0-9][0-9]+/\1$date/" "$cppfile1"
		echo "...ready! New version is now $nv"
	else
		echo "nicht _alle_ Dateien gefunden -> habe nichts gemacht!!!"
	fi
