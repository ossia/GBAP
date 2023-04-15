#!/bin/bash
rm -rf release
mkdir -p release

cp -rf MBAP *.{hpp,cpp,txt,json} LICENSE release/

mv release score-addon-mbap
7z a score-addon-mbap.zip score-addon-mbap
