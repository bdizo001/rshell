#test for test command

echo "Testing: test -e main.cpp"
test -e main.cpp

echo "Testing: test -f main.cpp"
test -f main.cpp

echo "Testing: test -d main.cpp"
test -d main.cpp



echo "Testing: test -e dummy.cpp"
test -e dummy.cpp

echo "Testing: test -f dummy.cpp"
test -f dummy.cpp

echo "Testing: test -d dummy.cpp"
test -d dummy.cpp



echo "Testing: [-e main.cpp]"
[-e main.cpp]

echo "Testing: [-d main.cpp]"
[-d main.cpp]

echo "Testing: [-f main.cpp]"
[-f main.cpp]


echo "Testing: [-e dummy.cpp]"
[-e dummy.cpp]

echo "Testing: [-f dummy.cpp]"
[-f dummy.cpp]

echo "Testing: [d dummy.cpp]"
[-d dummy.cpp]
