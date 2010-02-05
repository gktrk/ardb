xmllint resources/*.xsl --output res.txt
if [ -a res.txt ]
then
    rm -f res.txt
fi

