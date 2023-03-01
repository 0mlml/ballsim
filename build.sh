#!/bin/bash
outname="ballsoftware"

gcc main.c -lGL -lGLU -lglut -lm -o $outname
echo "Build Done: ./$outname"
case "$1" in 
    -r|--run)
        echo "Run flag supplied, also launching"
        ./$outname
        ;;
esac

echo "Done!"