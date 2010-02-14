if [ -d ardb_dist ]; then
rm -rf ardb_dist
fi
mkdir ardb_dist
strip src/ardb.exe
cp src/ardb.exe ardb_dist
cp /c/MinGW/bin/mingwm10.dll ardb_dist
mkdir ardb_dist/cardimages
mkdir ardb_dist/vtesicons
mkdir ardb_dist/xsl
cp resources/*.xsl ardb_dist/xsl
cp resources/vtesicons/*.xpm ardb_dist/vtesicons
cp resources/ardb.ini ardb_dist


