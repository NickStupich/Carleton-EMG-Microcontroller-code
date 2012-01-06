cls
cd Native
python sinTableCreator.py 8
make
cd ..
rm NativeFFT/Resources/RLP_test.elf
cp Native/Output/RLP_test.elf NativeFFT/Resources/RLP_test.elf

python spaceAnalysis.py
::python countLines.py